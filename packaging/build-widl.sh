#!/bin/sh
# Build widl (Wine's IDL compiler, from mingw-w64), midl (win32-sdk's
# MIDL-compatible front end for it) and tlbimport (win32-sdk's type library
# importer, which clang runs for #import) for one host.
#
#   packaging/build-widl.sh HOST OUTDIR
#
#   HOST       linux (64-bit, GCC) or windows (32-bit .exe files, cross-built
#              with the mingw-w64 GCC)
#   OUTDIR     where to put widl, midl, tlbimport, widl's notice and its
#              licence
#   MINGW_SRC  mingw-w64 checkout
#   WIN32_SDK  clang-retro/win32-sdk checkout (tools/midl.c,
#              tools/tlbimport.cpp)
#   WORK       build directory (default: ./build-widl-HOST)
#   LGPL_TEXT  the LGPL 2.1 text (default /usr/share/common-licenses/LGPL-2.1)
set -e
host=${1:?usage: build-widl.sh linux|windows OUTDIR} out=${2:?}
: "${MINGW_SRC:?set MINGW_SRC}" "${WIN32_SDK:?set WIN32_SDK}"
mkdir -p "$out"
out=$(cd "$out" && pwd)
WORK=${WORK:-$PWD/build-widl-$host}
rm -rf "$WORK"
mkdir -p "$WORK"
case $host in
linux) cc=gcc cxx=g++ conf= x= ;;
windows) cc=i686-w64-mingw32-gcc cxx=i686-w64-mingw32-g++ conf=--host=i686-w64-mingw32 x=.exe ;;
*) echo "unknown host $host" >&2; exit 1 ;;
esac
(
  cd "$WORK"
  # The target only names the default include directories and the
  # default pointer size, which midl always sets.
  CC=$cc CFLAGS=-O2 LDFLAGS="-static -s" "$MINGW_SRC/mingw-w64-tools/widl/configure" \
    $conf --target=i686-w64-mingw32 --prefix="$WORK/inst" >configure.log 2>&1 ||
    { cat configure.log; exit 1; }
  make -j"$(nproc)" >make.log 2>&1 || { tail -50 make.log; exit 1; }
  make install >install.log 2>&1
)
cp "$WORK/inst/bin/i686-w64-mingw32-widl$x" "$out/widl$x"
$cc -O2 -static -s -o "$out/midl$x" "$WIN32_SDK/tools/midl.c"
$cxx -std=c++17 -O2 -static -s -o "$out/tlbimport$x" "$WIN32_SDK/tools/tlbimport.cpp"
sha=$(git -C "$MINGW_SRC" rev-parse HEAD 2>/dev/null || echo unknown)
cat >"$out/widl-NOTICE.txt" <<EOF
widl is Wine's IDL compiler, as kept in mingw-w64 (mingw-w64-tools/widl).
It is licensed under the GNU Lesser General Public License, version 2.1 or
later (widl-LGPL-2.1.txt next to this file). Its source is mingw-w64
commit $sha,
https://github.com/mingw-w64/mingw-w64/tree/$sha/mingw-w64-tools/widl
EOF
lgpl=${LGPL_TEXT:-/usr/share/common-licenses/LGPL-2.1}
[ -f "$lgpl" ] || { echo "set LGPL_TEXT to the LGPL 2.1 text" >&2; exit 1; }
cp "$lgpl" "$out/widl-LGPL-2.1.txt"
echo "widl, midl and tlbimport ($host) built in $out"
