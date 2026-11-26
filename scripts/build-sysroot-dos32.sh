#!/bin/sh
# Builds the i386-msdos sysroot for 32-bit DOS programs (i386-pc-msdos, LE
# executables under the DOS/32A extender):
#
#   SYSROOT/include         picolibc, with dos.h, i86.h, dpmi.h and conio.h
#   SYSROOT/lib/crt0.o      startup code
#   SYSROOT/lib/libc.a      picolibc, with its DOS layer (libos/msdos/dpmi)
#   SYSROOT/lib/libm.a
#   SYSROOT/lib/libclang_rt.builtins.a
#   SYSROOT/lib/libc++.a, libc++abi.a, libunwind.a and include/c++/v1,
#                           with threads (not with NO_CXX=1)
#   SYSROOT/lib/dos32a.exe  the DOS/32A extender, which the driver makes the
#                           stub of each program
#   SYSROOT/share/licenses/DOS32A-license.txt
#
#   BIN           directory with clang, llvm-ar and the other LLVM tools,
#                 and jwlink
#   LLVM_SRC      clang-retro/llvm checkout (compiler-rt, libc++)
#   PICOLIBC_SRC  clang-retro/picolibc checkout
#   DOS32A_ZIP    dos32a-735-bin.zip (DOS/32A 7.35, from
#                 https://sourceforge.net/projects/dos32a/)
#   SYSROOT       output directory (default: ./i386-msdos)
#   WORK          build directory (default: ./build-dos32)
#   NO_CXX        1 to leave out libc++
set -e
: "${BIN:?set BIN}" "${LLVM_SRC:?set LLVM_SRC}" "${PICOLIBC_SRC:?set PICOLIBC_SRC}"
: "${DOS32A_ZIP:?set DOS32A_ZIP to dos32a-735-bin.zip}"
here=$(cd "$(dirname "$0")/.." && pwd)
SYSROOT=${SYSROOT:-$PWD/i386-msdos}
WORK=${WORK:-$PWD/build-dos32}
mkdir -p "$SYSROOT/include" "$SYSROOT/lib" "$SYSROOT/share/licenses" "$WORK"
SYSROOT=$(cd "$SYSROOT" && pwd) WORK=$(cd "$WORK" && pwd)
PATH=$BIN:$PATH
export PATH
# The cross file runs the linker as MSDOS_LD (JWlink next to clang).
MSDOS_LD=${MSDOS_LD:-$BIN/jwlink}
export MSDOS_LD
target=i386-pc-msdos

# The extender. The zip has binw/dos32a.exe and license.txt.
rm -rf "$WORK/dos32a"
mkdir -p "$WORK/dos32a"
(cd "$WORK/dos32a" && unzip -q "$DOS32A_ZIP" binw/dos32a.exe license.txt)
cp "$WORK/dos32a/binw/dos32a.exe" "$SYSROOT/lib/dos32a.exe"
cp "$WORK/dos32a/license.txt" "$SYSROOT/share/licenses/DOS32A-license.txt"

# picolibc.
sed "s|@DIRNAME@|$PICOLIBC_SRC/scripts|" "$PICOLIBC_SRC/scripts/cross-clang-i386-msdos.txt" \
  >"$WORK/cross.txt"
rm -rf "$WORK/picolibc" "$WORK/picolibc-install"
meson setup "$WORK/picolibc" "$PICOLIBC_SRC" --cross-file "$WORK/cross.txt" \
  -Dos-msdos=true -Dmultilib=false -Dpicocrt=false -Dsemihost=false \
  -Dtests=false -Dposix-console=true -Dsingle-thread=true \
  -Dthread-local-storage=false -Dspecsdir=none \
  --prefix="$WORK/picolibc-install" >"$WORK/picolibc.log" 2>&1 ||
  { tail -20 "$WORK/picolibc.log"; exit 1; }
ninja -C "$WORK/picolibc" install >>"$WORK/picolibc.log" 2>&1 ||
  { grep -E 'error:|FAILED' "$WORK/picolibc.log" | head -20; exit 1; }
cp -R "$WORK/picolibc-install/include/." "$SYSROOT/include/"
for f in crt0.o libc.a libm.a; do
  cp "$WORK/picolibc-install/lib/$f" "$SYSROOT/lib/"
done

# compiler-rt builtins: 64-bit division and the rest, for the 386.
rm -rf "$WORK/builtins"
cmake -G Ninja -S "$LLVM_SRC/compiler-rt/lib/builtins" -B "$WORK/builtins" \
  -DCMAKE_SYSTEM_NAME=Generic -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER="$BIN/clang" -DCMAKE_ASM_COMPILER="$BIN/clang" \
  -DCMAKE_CXX_COMPILER="$BIN/clang++" \
  -DCMAKE_C_COMPILER_TARGET=$target -DCMAKE_CXX_COMPILER_TARGET=$target \
  -DCMAKE_ASM_COMPILER_TARGET=$target \
  -DCMAKE_C_FLAGS="-march=i386 --sysroot=$SYSROOT" \
  -DCMAKE_ASM_FLAGS="-march=i386 --sysroot=$SYSROOT" \
  -DCMAKE_AR="$BIN/llvm-ar" -DCMAKE_RANLIB="$BIN/llvm-ranlib" \
  -DCMAKE_NM="$BIN/llvm-nm" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON -DCOMPILER_RT_BAREMETAL_BUILD=ON \
  -DCOMPILER_RT_OS_DIR=msdos \
  -DLLVM_CMAKE_DIR="${LLVM_CMAKE_DIR:-$BIN/../lib/cmake/llvm}" \
  >"$WORK/builtins.log" 2>&1 || { tail -20 "$WORK/builtins.log"; exit 1; }
ninja -C "$WORK/builtins" >>"$WORK/builtins.log" 2>&1 ||
  { grep -E 'error:|FAILED' "$WORK/builtins.log" | head -20; exit 1; }
cp "$WORK/builtins/lib/msdos/libclang_rt.builtins-i386.a" \
  "$SYSROOT/lib/libclang_rt.builtins.a"

[ -z "${NO_CXX:-}" ] || { echo "i386-msdos sysroot in $SYSROOT (no C++)"; exit 0; }

# libc++ and libc++abi, with exceptions, RTTI and threads (the cooperative
# threads of picolibc's pthread.h for DOS).
flags="-march=i386 --sysroot=$SYSROOT"
rm -rf "$WORK/libcxx" "$WORK/libcxx-install"
cmake -G Ninja -S "$LLVM_SRC/runtimes" -B "$WORK/libcxx" \
  -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" \
  -DCMAKE_SYSTEM_NAME=Generic -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_C_COMPILER="$BIN/clang" -DCMAKE_CXX_COMPILER="$BIN/clang++" \
  -DCMAKE_ASM_COMPILER="$BIN/clang" \
  -DCMAKE_C_COMPILER_TARGET=$target -DCMAKE_CXX_COMPILER_TARGET=$target \
  -DCMAKE_ASM_COMPILER_TARGET=$target \
  -DCMAKE_C_FLAGS="$flags" -DCMAKE_CXX_FLAGS="$flags -D_GNU_SOURCE" \
  -DCMAKE_AR="$BIN/llvm-ar" -DCMAKE_RANLIB="$BIN/llvm-ranlib" \
  -DCMAKE_NM="$BIN/llvm-nm" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCMAKE_INSTALL_PREFIX="$WORK/libcxx-install" \
  -DLIBCXX_ENABLE_SHARED=OFF -DLIBCXX_ENABLE_EXCEPTIONS=ON \
  -DLIBCXX_ENABLE_RTTI=ON -DLIBCXX_ENABLE_THREADS=ON \
  -DLIBCXX_HAS_PTHREAD_API=ON -DLIBCXX_ENABLE_MONOTONIC_CLOCK=ON -DLIBCXX_ENABLE_LOCALIZATION=ON \
  -DLIBCXX_ENABLE_WIDE_CHARACTERS=ON -DLIBCXX_ENABLE_UNICODE=ON \
  -DLIBCXX_ENABLE_FILESYSTEM=ON -DLIBCXX_ENABLE_RANDOM_DEVICE=OFF \
  -DLIBCXX_ENABLE_TIME_ZONE_DATABASE=OFF -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
  -DLIBCXX_INCLUDE_TESTS=OFF -DLIBCXX_USE_COMPILER_RT=ON \
  -DLIBCXX_CXX_ABI=libcxxabi -DRUNTIMES_USE_LIBC=picolibc \
  -DLIBCXXABI_ENABLE_SHARED=OFF -DLIBCXXABI_ENABLE_EXCEPTIONS=ON \
  -DLIBCXXABI_ENABLE_THREADS=ON -DLIBCXXABI_HAS_PTHREAD_API=ON \
  -DLIBCXXABI_HAS_CXA_THREAD_ATEXIT_IMPL=ON \
  -DLIBCXXABI_BAREMETAL=ON \
  -DLIBCXXABI_ENABLE_DEMANGLER=OFF -DLIBCXXABI_NON_DEMANGLING_TERMINATE=ON \
  -DLIBCXXABI_USE_LLVM_UNWINDER=OFF -DLIBCXXABI_USE_COMPILER_RT=ON \
  -DLIBCXXABI_INCLUDE_TESTS=OFF >"$WORK/libcxx.log" 2>&1 ||
  { tail -20 "$WORK/libcxx.log"; exit 1; }
ninja -C "$WORK/libcxx" install-cxx install-cxxabi >>"$WORK/libcxx.log" 2>&1 ||
  { grep -E 'error:|FAILED' "$WORK/libcxx.log" | head -20; exit 1; }
cp "$WORK/libcxx-install/lib/libc++.a" "$WORK/libcxx-install/lib/libc++abi.a" "$SYSROOT/lib/"
mkdir -p "$SYSROOT/include/c++"
rm -rf "$SYSROOT/include/c++/v1"
cp -R "$WORK/libcxx-install/include/c++/v1" "$SYSROOT/include/c++/v1"
# The unwinder: SjLj function contexts, one list per thread.
"$BIN/clang" --target=$target $flags -fexceptions -Os -ffunction-sections \
  -c "$here/runtime/unwind-sjlj32.c" -o "$WORK/unwind-sjlj.o"
rm -f "$SYSROOT/lib/libunwind.a"
"$BIN/llvm-ar" rcs "$SYSROOT/lib/libunwind.a" "$WORK/unwind-sjlj.o"
echo "i386-msdos sysroot in $SYSROOT"
