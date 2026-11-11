#!/bin/sh
# Build libc99.a, the C99 functions Mac OS X 10.0 to 10.2 lack, and install
# it with its headers into a sysroot for those versions. C++ (libc++) needs
# it there; C programs can use it too.
#
#   PICOLIBC_SRC  picolibc checkout     BIN  directory with clang
#   SYSROOT   the 10.0 (or 10.1, 10.2) sysroot to install into
#   OSX_VERSION  the version SYSROOT is for (default: 10.0)
#   WORK      build directory (default: ./build-c99-macosx)
#   NM, LIBTOOL  Apple's nm and libtool from scripts/build-cctools-ppc.sh
#             (default: powerpc-apple-darwin8-nm and -libtool on PATH)
#
# libc99.a has:
#   - the float versions of the <math.h> functions (sinf, roundf, ...) and
#     the C99 double ones the system lacks (round, trunc, fma, ...), from
#     picolibc's libm,
#   - the long double versions; long double is double before 10.4, so these
#     call the double functions,
#   - the <fenv.h> functions, from picolibc,
#   - llabs, lldiv, atoll, strtoll, strtoull, strtof, strtold, _Exit and
#     the <inttypes.h> functions, on the BSD strtoq and strtouq.
# It takes nothing from picolibc that the system library already has.
#
# The headers go to SYSROOT/usr/local/include, which clang searches before
# SYSROOT/usr/include, and each includes the system one first. The library
# goes to SYSROOT/usr/local/lib, which the linker searches; link with -lc99.
set -e
: "${PICOLIBC_SRC:?set PICOLIBC_SRC}" "${BIN:?set BIN}" "${SYSROOT:?set SYSROOT}"
NM=${NM:-powerpc-apple-darwin8-nm}
LIBTOOL=${LIBTOOL:-powerpc-apple-darwin8-libtool}
WORK=${WORK:-$PWD/build-c99-macosx}
version=${OSX_VERSION:-10.0}
target=powerpc-apple-macosx$version
here=$(cd "$(dirname "$0")/../macosx/c99" && pwd)
dylib=$SYSROOT/usr/lib/libSystem.B.dylib
[ -f "$dylib" ] || { echo "$dylib not found" >&2; exit 1; }

# picolibc's libm, compiled with picolibc's own headers. Only libm is built;
# picolibc's C library is not for Mac OS X.
cmake -G Ninja -S "$PICOLIBC_SRC" -B "$WORK/picolibc" \
  -DCMAKE_SYSTEM_NAME=Generic -DCMAKE_SYSTEM_PROCESSOR=powerpc \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER="$BIN/clang" -DCMAKE_C_COMPILER_TARGET=$target \
  -DCMAKE_ASM_COMPILER_TARGET=$target \
  -DCMAKE_C_FLAGS="-nostdlibinc -mcpu=750" \
  -DCMAKE_AR="$BIN/llvm-ar" -DCMAKE_RANLIB="$BIN/llvm-ranlib" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -D__THREAD_LOCAL_STORAGE=OFF -D__SINGLE_THREAD=ON
libm=$(ninja -C "$WORK/picolibc" -t targets all |
  sed -n 's|^\(CMakeFiles/c.dir/libm/[^:]*\.obj\):.*|\1|p')
# shellcheck disable=SC2086
ninja -C "$WORK/picolibc" $libm

cd "$WORK/picolibc"
# shellcheck disable=SC2086
python3 -I "$here/select-objects.py" "$(command -v "$NM")" "$dylib" $libm \
  >"$WORK/selected"
objs=$(sed -n 1p "$WORK/selected")
nodefs=$(sed -n 2p "$WORK/selected" | sed 's/[^ ][^ ]*/-DNO_&/g')

# Our own part, against the system headers and ours.
mkdir -p "$WORK/own"
for f in fenv long-double stdlib; do
  # shellcheck disable=SC2086
  "$BIN/clang" --target=$target -isysroot "$SYSROOT" -isystem "$here/include" \
    -mcpu=750 -O2 -Wall $nodefs -c "$here/src/$f.c" -o "$WORK/own/$f.o"
done

mkdir -p "$SYSROOT/usr/local/lib" "$SYSROOT/usr/local/include"
# shellcheck disable=SC2086
"$LIBTOOL" -static -no_warning_for_no_symbols -o "$SYSROOT/usr/local/lib/libc99.a" \
  $objs "$WORK"/own/*.o
cp "$here"/include/*.h "$SYSROOT/usr/local/include/"
echo "installed $SYSROOT/usr/local/lib/libc99.a and its headers"
