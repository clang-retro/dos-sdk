#!/bin/sh
# Build libc++, libc++abi and libunwind as static libraries for 32-bit
# Mac OS X (PowerPC or Intel), for C++ programs that want a modern standard
# library in place of Apple's libstdc++.
#
#   LLVM_SRC  llvm-project checkout     BIN  directory with clang
#   ARCH      ppc (default) or i386. Intel needs MacOSX10.4u.sdk.
#   SDK       MacOSX10.4.0.sdk (or 10.4u), or a 10.0 sysroot
#   OSX_VERSION  the version to build for (default: 10.4)
#   WORK      build directory (default: ./build-libcxx-macosx)
#   PREFIX    install directory (default: ./libcxx-macosx); headers go to
#             PREFIX/include/c++/v1 and libraries to PREFIX/lib
#
# For 10.0, first run scripts/fix-macosx-10.0-headers.sh and
# scripts/build-c99-macosx.sh on the sysroot, and link programs with -lc99
# too.
#
# Needs powerpc-apple-darwin8-ar and -ranlib on PATH (scripts/build-cctools-
# ppc.sh): ld64 can't read the symbol table llvm-ar writes for PowerPC.
# These tools also make the archives for Intel. scripts/build-runtime-
# macosx.sh builds both and joins them into fat libraries.
#
# For 10.4 and later, the libraries have threads, locales, wide characters,
# the file system library and the monotonic clock. They need clang-retro's
# llvm-project (it fills the gaps of the 10.4 C library) and the builtins
# of scripts/build-builtins-macosx.sh built with SDK set (emulated TLS and
# the atomic library functions). For versions before 10.4, or with
# MINIMAL=1, the libraries leave these parts out. The parallel algorithms
# run serially (libdispatch arrived in 10.6). Build a program with:
#   clang++ --target=powerpc-apple-macosx10.4 -isysroot $SDK -nostdinc++ \
#     -isystem $PREFIX/include/c++/v1 x.cpp -nostdlib++ -L$PREFIX/lib \
#     -lc++ -lc++abi -lunwind
set -e
: "${LLVM_SRC:?set LLVM_SRC}" "${BIN:?set BIN}" "${SDK:?set SDK}"
WORK=${WORK:-$PWD/build-libcxx-macosx}
PREFIX=${PREFIX:-$PWD/libcxx-macosx}
version=${OSX_VERSION:-10.4}
arch=${ARCH:-ppc}
case $arch in
  ppc) target=powerpc-apple-macosx$version ;;
  i386) target=i386-apple-macosx$version ;;
  *) echo "ARCH is ppc or i386" >&2; exit 2 ;;
esac
# PRIx64 and friends: the 10.4 inttypes.h only defines them for C++ when
# asked.
flags=-D__STDC_FORMAT_MACROS
# The parts that need more of the C library than 10.0 to 10.3 have.
case $version in
  10.[0-3]) full=OFF ;;
  *) full=ON ;;
esac
[ "${MINIMAL:-0}" = 1 ] && full=OFF

cmake -G Ninja -S "$LLVM_SRC/runtimes" -B "$WORK" \
  -DLLVM_ENABLE_RUNTIMES="libunwind;libcxxabi;libcxx" \
  -DCMAKE_SYSTEM_NAME=Darwin -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DCMAKE_C_COMPILER="$BIN/clang" -DCMAKE_CXX_COMPILER="$BIN/clang++" \
  -DCMAKE_ASM_COMPILER="$BIN/clang" \
  -DCMAKE_C_COMPILER_TARGET=$target -DCMAKE_CXX_COMPILER_TARGET=$target \
  -DCMAKE_ASM_COMPILER_TARGET=$target \
  -DCMAKE_OSX_SYSROOT="$SDK" -DCMAKE_OSX_ARCHITECTURES=$arch \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=$version \
  -DCMAKE_AR="$(command -v powerpc-apple-darwin8-ar)" \
  -DCMAKE_RANLIB="$(command -v powerpc-apple-darwin8-ranlib)" \
  -DCMAKE_C_FLAGS="$flags" -DCMAKE_CXX_FLAGS="$flags" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DLIBUNWIND_ENABLE_SHARED=OFF -DLIBUNWIND_ENABLE_THREADS=$full \
  -DLIBCXXABI_ENABLE_SHARED=OFF -DLIBCXXABI_ENABLE_THREADS=$full \
  -DLIBCXXABI_HAS_CXA_THREAD_ATEXIT_IMPL=OFF \
  -DLIBCXXABI_USE_LLVM_UNWINDER=ON -DLIBCXXABI_ENABLE_STATIC_UNWINDER=ON \
  -DLIBCXX_ENABLE_SHARED=OFF -DLIBCXX_ENABLE_THREADS=$full \
  -DLIBCXX_ENABLE_STATIC_ABI_LIBRARY=ON -DLIBCXX_CXX_ABI=libcxxabi \
  -DLIBCXX_ENABLE_LOCALIZATION=$full -DLIBCXX_ENABLE_WIDE_CHARACTERS=$full \
  -DLIBCXX_ENABLE_FILESYSTEM=$full -DLIBCXX_ENABLE_RANDOM_DEVICE=OFF \
  -DLIBCXX_ENABLE_MONOTONIC_CLOCK=$full -DLIBCXX_PSTL_BACKEND=serial \
  -DLIBCXX_ENABLE_TIME_ZONE_DATABASE=OFF \
  -DLIBCXX_ENABLE_VENDOR_AVAILABILITY_ANNOTATIONS=OFF \
  -DLIBCXX_INCLUDE_TESTS=OFF -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
  -DLIBCXXABI_INCLUDE_TESTS=OFF -DLIBUNWIND_INCLUDE_TESTS=OFF
ninja -C "$WORK" install-unwind install-cxxabi install-cxx
echo "installed $PREFIX"
