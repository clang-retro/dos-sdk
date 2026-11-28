#!/bin/sh
# Build the C++ runtime and the builtins for 32-bit Mac OS X 10.4 and later,
# for PowerPC and Intel, and join the libraries of the two processors into
# fat libraries.
#
#   LLVM_SRC  llvm-project checkout     BIN  directory with clang and llvm-ar
#   SDK       MacOSX10.4u.sdk (it has the PowerPC and the Intel libraries)
#   WORK      build directory (default: ./build-runtime-macosx)
#   PREFIX    install directory (default: ./macosx10.4). The headers go to
#             PREFIX/include/c++/v1, the fat libc++, libc++abi and
#             libunwind to PREFIX/lib, and the builtins to
#             PREFIX/lib/darwin (libclang_rt.ppc_osx.a and
#             libclang_rt.osx.a).
#
# Needs powerpc-apple-darwin8-ar, -ranlib, -libtool and -lipo on PATH
# (scripts/build-cctools-ppc.sh), and -ld for the link checks of
# tests/macosx-link/check.sh. Apple's SDK is not copied into PREFIX.
#
# Copy PREFIX/lib/darwin/* into <clang resource dir>/lib/darwin. Build a
# fat program with one clang++ per processor, then join them with lipo:
#   clang++ --target=i386-apple-macosx10.4 -isysroot $SDK -nostdinc++ \
#     -isystem $PREFIX/include/c++/v1 x.cpp -nostdlib++ -L$PREFIX/lib \
#     -lc++ -lc++abi -lunwind \
#     -fuse-ld=$(command -v powerpc-apple-darwin8-ld) -o x-i386
#   powerpc-apple-darwin8-lipo -create x-ppc x-i386 -output x
set -e
: "${LLVM_SRC:?set LLVM_SRC}" "${BIN:?set BIN}" "${SDK:?set SDK}"
WORK=${WORK:-$PWD/build-runtime-macosx}
PREFIX=${PREFIX:-$PWD/macosx10.4}
here=$(cd "$(dirname "$0")" && pwd)

for arch in ppc i386; do
  libcxx_work=$WORK/libcxx-$arch libcxx_prefix=$WORK/install-$arch
  builtins_work=$WORK/builtins-$arch builtins_dest=$WORK/builtins
  ARCH=$arch WORK=$libcxx_work PREFIX=$libcxx_prefix OSX_VERSION=10.4 \
    sh "$here/build-libcxx-macosx.sh"
  ARCH=$arch WORK=$builtins_work DEST=$builtins_dest \
    sh "$here/build-builtins-macosx.sh"
done

rm -rf "$PREFIX"
mkdir -p "$PREFIX/lib/darwin"
cp -R "$WORK/install-ppc/include" "$PREFIX/"
for lib in libc++.a libc++abi.a libunwind.a; do
  powerpc-apple-darwin8-lipo -create "$WORK/install-ppc/lib/$lib" \
    "$WORK/install-i386/lib/$lib" -output "$PREFIX/lib/$lib"
done
cp "$WORK"/builtins/libclang_rt.*.a "$PREFIX/lib/darwin/"
# Link small C++ programs against the new libraries with ld64, so that the
# build stops when ld64 does not accept the code in them.
BIN=$BIN SDK=$SDK PREFIX=$PREFIX WORK=$WORK/link-check \
  sh "$here/../tests/macosx-link/check.sh"
echo "installed $PREFIX"
