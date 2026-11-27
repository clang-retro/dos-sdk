#!/bin/sh
# Put a clang-dos package together.
#
#   packaging/assemble.sh HOST OUTDIR
#
#   HOST          linux or windows
#   OUTDIR        the package directory (replaced)
#   LLVM_PREFIX   the install directory from build-llvm.sh for HOST
#   JWLINK        jwlink (or jwlink.exe) for HOST
#   WIDL_DIR      widl, midl and tlbimport for HOST, from build-widl.sh
#                 (optional)
#   SYSROOTS      directory with the sysroots from build-sysroot.sh, one
#                 subdirectory per name
#   LLVM_SRC      clang-dos/llvm checkout (licence)
#   PICOLIBC_SRC  clang-dos/picolibc checkout (README, licence)
#   JWLINK_SRC    clang-dos/JWlink checkout (licence)
#   WIN16_SDK     clang-dos/win16-sdk checkout (config files, tools, README)
#   WIN32_SDK     clang-dos/win32-sdk checkout (config files, tools, README)
#   AMIGA_SDK     clang-dos/amiga-sdk checkout (NDK installer, examples, README)
#   ATARI_SDK     clang-dos/atari-sdk checkout (Hatari runner, examples, README)
#   UNIX_SDK      clang-retro/unix-sdk checkout (IRIX examples, qemu-irix and
#                 import scripts, README); optional, with the mips-sgi-irix
#                 sysroot
#   SCO_SDK       clang-retro/sco-sdk checkout (scorun, examples, README);
#                 optional, with the i386-pc-sco sysroot
#   AIX_SDK       clang-retro/aix-sdk checkout (aixrun, examples, README);
#                 optional, with the powerpc-ibm-aix sysroot
#   BRANCHES      optional: BRANCHES.txt to include
set -e
host=${1:?usage: assemble.sh linux|windows OUTDIR} out=${2:?}
: "${LLVM_PREFIX:?}" "${JWLINK:?}" "${SYSROOTS:?}" "${LLVM_SRC:?}" \
  "${PICOLIBC_SRC:?}" "${JWLINK_SRC:?}" "${WIN16_SDK:?}" "${WIN32_SDK:?}" \
  "${AMIGA_SDK:?}" "${ATARI_SDK:?}"
here=$(cd "$(dirname "$0")" && pwd)
sdk=$(cd "$here/.." && pwd)
inst=$LLVM_PREFIX
x=; [ "$host" = windows ] && x=.exe
rm -rf "$out"
mkdir -p "$out/bin" "$out/lib" "$out/sysroots" "$out/share/clang-dos/docs" \
  "$out/share/licenses"

keep="clang clang++ clang-cpp clang-cl clang-format clang-scan-deps lld ld.lld
 lld-link llvm-ar llvm-ranlib llvm-nm llvm-objcopy llvm-objdump llvm-strip
 llvm-size llvm-readobj llvm-readelf llvm-rc llvm-windres llvm-dlltool llvm-lib
 llvm-cxxfilt llvm-strings llvm-symbolizer llvm-ml llvm-elf2hunk
 llvm-elf2tos llvm-exe2cmd llvm-mlb"
for t in $keep; do
  if [ -e "$inst/bin/$t$x" ] || [ -L "$inst/bin/$t$x" ]; then
    if [ -n "$x" ]; then
      cp -L "$inst/bin/$t$x" "$out/bin/"   # Windows has no symbolic links
    else
      cp -P "$inst/bin/$t$x" "$out/bin/"
    fi
  else
    echo "warning: $t$x is not in $inst/bin" >&2
  fi
done
# On Linux clang is a link to clang-<major>.
if [ -z "$x" ]; then
  for f in "$inst"/bin/clang-[0-9]*; do
    if [ -e "$f" ]; then cp -P "$f" "$out/bin/"; fi
  done
fi
cp -R "$inst/lib/clang" "$out/lib/"
cp "$JWLINK" "$out/bin/jwlink$x"
chmod +x "$out/bin/jwlink$x"
# The IDL compiler: widl, and midl, which takes MIDL's options and runs it;
# and tlbimport, which clang runs for #import of a type library.
if [ -n "${WIDL_DIR:-}" ]; then
  cp "$WIDL_DIR/widl$x" "$WIDL_DIR/midl$x" "$WIDL_DIR/tlbimport$x" "$out/bin/"
  mkdir -p "$out/share/licenses/widl"
  cp "$WIDL_DIR/widl-NOTICE.txt" "$WIDL_DIR/widl-LGPL-2.1.txt" "$out/share/licenses/widl/"
else
  echo "warning: no WIDL_DIR, so no widl or midl" >&2
fi
cp "$here"/cfg/*.cfg "$WIN16_SDK"/cfg/*.cfg "$WIN32_SDK"/cfg/*.cfg "$out/bin/"

cp -R "$SYSROOTS/i8086-msdos" "$out/i8086-msdos"
# 32-bit DOS: the sysroot next to bin, with the DOS/32A extender in it.
cp -R "$SYSROOTS/i386-msdos" "$out/i386-msdos"
cp "$SYSROOTS/i386-msdos/share/licenses/DOS32A-license.txt" "$out/share/licenses/"
# The driver looks for the AmigaOS sysroot next to bin, as for DOS.
cp -R "$SYSROOTS/m68k-amigaos" "$out/m68k-amigaos"
cp -R "$SYSROOTS/m68k-atari-tos" "$out/m68k-atari-tos"
irix=
if [ -n "${UNIX_SDK:-}" ] && [ -d "$SYSROOTS/mips-sgi-irix" ]; then irix=1; fi
if [ -n "$irix" ]; then cp -R "$SYSROOTS/mips-sgi-irix" "$out/mips-sgi-irix"; fi
solaris=
bsd=
for b in freebsd netbsd; do
  if [ -n "${UNIX_SDK:-}" ] && [ -d "$SYSROOTS/i386-unknown-$b" ]; then
    bsd=1; cp -R "$SYSROOTS/i386-unknown-$b" "$out/i386-unknown-$b"
  fi
done
sco=
if [ -n "${SCO_SDK:-}" ] && [ -d "$SYSROOTS/i386-pc-sco" ]; then
  sco=1; cp -R "$SYSROOTS/i386-pc-sco" "$out/i386-pc-sco"
fi
aix=
if [ -n "${AIX_SDK:-}" ] && [ -d "$SYSROOTS/powerpc-ibm-aix" ]; then
  aix=1; cp -R "$SYSROOTS/powerpc-ibm-aix" "$out/powerpc-ibm-aix"
fi
if [ -n "${UNIX_SDK:-}" ]; then
  for t in sparc-sun-solaris i386-pc-solaris; do
    if [ -d "$SYSROOTS/$t" ]; then solaris=1; cp -R "$SYSROOTS/$t" "$out/$t"; fi
  done
fi
for s in win9x-msvcrt win9x-crtdll win2k-xp win32-picolibc win-vista win-vista-x64; do
  cp -R "$SYSROOTS/$s" "$out/sysroots/"
done

cp "$WIN16_SDK/tools/ne-check.py" "$WIN16_SDK/tools/win16-imports.py" \
  "$WIN32_SDK/tools/check-imports.py" "$WIN32_SDK/tools/msvcrt-versions.txt" \
  "$WIN32_SDK/scripts/build-modern-sysroot.sh" "$out/share/clang-dos/"
# Kernel-mode drivers (win32-sdk's *-ddk.cfg), SCSI miniports and VxDs: the
# checkers' data, the source of libntseh.a (_except_handler3) for
# build-modern-sysroot.sh, the signing tool, the linker for VxDs and the test
# driver and VxD, and mapsym.py (.SYM files for WDEB386 and SoftICE).
for f in tools/kernel-versions.txt tools/wdm9x-exports.txt \
    tools/scsiport9x-exports.txt tools/mapsym.py \
    scripts/fix-ddk-headers.sh kernel/except3.S tools/drvsign.py \
    tools/vxd-link.py tools/vxd-check.py; do
  if [ -f "$WIN32_SDK/$f" ]; then cp "$WIN32_SDK/$f" "$out/share/clang-dos/"; fi
done
if [ -d "$WIN32_SDK/tests/ddk" ]; then cp -R "$WIN32_SDK/tests/ddk" "$out/share/clang-dos/ddk"; fi
# The DDK headers mingw-w64 lacks (hidport.h, 1394.h ...), which
# fix-ddk-headers.sh copies into a sysroot from ddk-include next to it.
if [ -d "$WIN32_SDK/kernel/include" ]; then cp -R "$WIN32_SDK/kernel/include" "$out/share/clang-dos/ddk-include"; fi
if [ -d "$WIN32_SDK/vxd" ]; then cp -R "$WIN32_SDK/vxd" "$out/share/clang-dos/vxd"; fi
# Makes sysroots from the user's own copies of Microsoft's SDKs and DDKs.
if [ -f "$WIN32_SDK/tools/import-ms-sdk.py" ]; then
  cp "$WIN32_SDK/tools/import-ms-sdk.py" "$WIN32_SDK/tools/ms_oldlib.py" \
    "$WIN32_SDK/tools/ms-intrin-shim.h" "$out/share/clang-dos/"
  # It converts the Windows 3.1 DDK's MASM include files with inc2h.py.
  if [ -f "$WIN32_SDK/tools/inc2h.py" ]; then
    cp "$WIN32_SDK/tools/inc2h.py" "$out/share/clang-dos/"
  fi
  cp -R "$WIN32_SDK/tests/ms-sdk" "$out/share/clang-dos/ms-sdk"
fi
# amiga-ndk.py can install Hyperion's NDK headers into ../../m68k-amigaos
# from here, in place of the SDK's own.
cp "$AMIGA_SDK/tools/amiga-ndk.py" "$AMIGA_SDK/tools/lha.py" \
  "$AMIGA_SDK/tools/fd2stub.py" "$out/share/clang-dos/"
# The link library helpers, which amiga-ndk.py builds into its libamiga.a.
if [ -d "$AMIGA_SDK/os/alib" ]; then
  cp -R "$AMIGA_SDK/os/alib" "$out/share/clang-dos/alib"
fi
mkdir -p "$out/share/clang-dos/amiga-examples"
cp "$AMIGA_SDK"/examples/*.c "$out/share/clang-dos/amiga-examples/"
mkdir -p "$out/share/clang-dos/atari-examples"
cp "$ATARI_SDK"/examples/*.c "$out/share/clang-dos/atari-examples/"
cp "$ATARI_SDK/tools/hatari-run.sh" "$out/share/clang-dos/"
if [ -n "$irix" ]; then
  mkdir -p "$out/share/clang-dos/irix-examples"
  cp "$UNIX_SDK"/irix/examples/*.c "$out/share/clang-dos/irix-examples/"
  cp "$UNIX_SDK/irix/tools/build-qemu-irix.sh" "$UNIX_SDK/irix/tools/import-irix.sh" \
    "$out/share/clang-dos/"
  cp "$UNIX_SDK/irix/README.md" "$out/share/clang-dos/docs/irix-README.md"
fi
if [ -n "$solaris" ]; then
  mkdir -p "$out/share/clang-dos/solaris-examples"
  cp "$UNIX_SDK"/solaris/examples/*.c "$out/share/clang-dos/solaris-examples/"
  cp "$UNIX_SDK/solaris/README.md" "$out/share/clang-dos/docs/solaris-README.md"
fi
cp "$sdk/README.md" "$out/share/clang-dos/docs/dos-sdk-README.md"
cp "$WIN16_SDK/README.md" "$out/share/clang-dos/docs/win16-README.md"
cp "$WIN32_SDK/README.md" "$out/share/clang-dos/docs/win32-README.md"
cp "$AMIGA_SDK/README.md" "$out/share/clang-dos/docs/amiga-README.md"
cp "$ATARI_SDK/README.md" "$out/share/clang-dos/docs/atari-README.md"
cp "$sdk/docs/memory-models.md" "$out/share/clang-dos/docs/"
if [ -n "$bsd" ]; then
  mkdir -p "$out/share/clang-dos/bsd-examples"
  cp "$UNIX_SDK"/bsd/examples/*.c "$out/share/clang-dos/bsd-examples/"
  cp "$UNIX_SDK/bsd/tools/bsdrun.c" "$out/share/clang-dos/"
  cp "$UNIX_SDK/bsd/README.md" "$out/share/clang-dos/docs/bsd-README.md"
fi
if [ -n "$sco" ]; then
  mkdir -p "$out/share/clang-dos/sco-examples"
  cp "$SCO_SDK"/examples/*.c "$out/share/clang-dos/sco-examples/"
  cp "$SCO_SDK/tools/scorun.c" "$out/share/clang-dos/"
  cp "$SCO_SDK/README.md" "$out/share/clang-dos/docs/sco-README.md"
fi
if [ -n "$aix" ]; then
  mkdir -p "$out/share/clang-dos/aix-examples"
  cp "$AIX_SDK"/examples/*.c "$out/share/clang-dos/aix-examples/"
  cp "$AIX_SDK/tools/aixrun.c" "$out/share/clang-dos/"
  cp "$AIX_SDK/README.md" "$out/share/clang-dos/docs/aix-README.md"
fi
for os in msdos amigaos tos irix solaris bsd sco aix; do
  if [ -f "$PICOLIBC_SRC/libos/$os/README.md" ]; then
    cp "$PICOLIBC_SRC/libos/$os/README.md" \
      "$out/share/clang-dos/docs/picolibc-$os-README.md"
  fi
done

cp -R "$here/examples" "$out/examples"
cp "$here/smoke-test.sh" "$here/README.md" "$out/"
# DOSBox runs the 32-bit DOS programs: the runner and the tests.
cp "$here/dosbox-run.sh" "$out/share/clang-dos/"
mkdir -p "$out/share/clang-dos/dos32-tests"
cp "$sdk"/tests/dos32/* "$out/share/clang-dos/dos32-tests/"
cp "$sdk/docs/dos32.md" "$out/share/clang-dos/docs/"
if [ -n "${BRANCHES:-}" ]; then cp "$BRANCHES" "$out/BRANCHES.txt"; fi

cp "$sdk/LICENSE" "$out/share/licenses/dos-sdk-LICENSE"
# The Windows SDKs: their licence, and NOTICE.md, which lists their parts
# and the licence of each (every sysroot of win32-sdk carries its
# mingw-w64 notices in share/licenses too).
for s in win16-sdk:$WIN16_SDK win32-sdk:$WIN32_SDK; do
  n=${s%%:*} d=${s#*:}
  mkdir -p "$out/share/licenses/$n"
  cp "$d/LICENSE" "$out/share/licenses/$n/LICENSE"
  if [ -f "$d/NOTICE.md" ]; then cp "$d/NOTICE.md" "$out/share/licenses/$n/"; fi
done
cp "$ATARI_SDK/LICENSE" "$out/share/licenses/atari-sdk-LICENSE"
if [ -n "$irix$solaris$bsd" ]; then cp "$UNIX_SDK/LICENSE" "$out/share/licenses/unix-sdk-LICENSE"; fi
if [ -n "$sco" ]; then cp "$SCO_SDK/LICENSE" "$out/share/licenses/sco-sdk-LICENSE"; fi
if [ -n "$aix" ]; then cp "$AIX_SDK/LICENSE" "$out/share/licenses/aix-sdk-LICENSE"; fi
cp "$LLVM_SRC/llvm/LICENSE.TXT" "$out/share/licenses/LLVM-LICENSE.TXT"
cp "$PICOLIBC_SRC/COPYING.picolibc" "$out/share/licenses/" 2>/dev/null || true
cp "$JWLINK_SRC/Ver/license.txt" "$out/share/licenses/JWlink-OWPL.txt" 2>/dev/null || true
du -sh "$out"
