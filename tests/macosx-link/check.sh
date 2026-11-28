#!/bin/sh
# Link checks for the macosx10.4 sysroot: build each program of this
# folder for PowerPC and Intel, with PIC and with -mdynamic-no-pic, at -O0
# and -O2, and link it with ld64 against libc++. The checks do not run the
# programs. They find code that ld64 does not accept (for example a pointer
# difference to a global weak symbol).
#
#   BIN     directory with clang++
#   SDK     MacOSX10.4u.sdk
#   PREFIX  the sysroot (scripts/build-runtime-macosx.sh): PREFIX/include,
#           PREFIX/lib and PREFIX/lib/darwin
#   WORK    build directory (default: ./build-macosx-link)
#
# Needs powerpc-apple-darwin8-ld on PATH (scripts/build-cctools-ppc.sh).
: "${BIN:?set BIN}" "${SDK:?set SDK}" "${PREFIX:?set PREFIX}"
here=$(cd "$(dirname "$0")" && pwd)
WORK=${WORK:-$PWD/build-macosx-link}
ld=$(command -v powerpc-apple-darwin8-ld) || {
  echo "error: powerpc-apple-darwin8-ld is not on PATH" >&2
  exit 2
}
mkdir -p "$WORK"
fail=0
for src in "$here"/*.cpp; do
  name=$(basename "$src" .cpp)
  for arch in ppc i386; do
    case $arch in
    ppc) target=powerpc-apple-macosx10.4 rt=libclang_rt.ppc_osx.a ;;
    i386) target=i386-apple-macosx10.4 rt=libclang_rt.osx.a ;;
    esac
    for reloc in pic dynamic-no-pic; do
      rf=
      [ $reloc = dynamic-no-pic ] && rf=-mdynamic-no-pic
      for o in O0 O2; do
        out=$WORK/$name-$arch-$reloc-$o
        if "$BIN/clang++" --target=$target -isysroot "$SDK" -std=c++17 \
             -nostdinc++ -isystem "$PREFIX/include/c++/v1" -$o $rf "$src" \
             -fuse-ld="$ld" -nostdlib++ -L"$PREFIX/lib" -lc++ -lc++abi \
             -lunwind "$PREFIX/lib/darwin/$rt" -o "$out" >"$out.log" 2>&1; then
          echo "PASS $name $arch $reloc -$o"
        else
          echo "FAIL $name $arch $reloc -$o"
          sed 's/^/    /' "$out.log" | head -20
          fail=$((fail + 1))
        fi
      done
    done
  done
done
[ $fail = 0 ] || { echo "$fail link checks failed" >&2; exit 1; }
