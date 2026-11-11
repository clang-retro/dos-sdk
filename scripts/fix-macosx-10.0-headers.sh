#!/bin/sh
# Fix Mac OS X 10.0's headers in a sysroot so C++ (libc++, libunwind)
# builds against them, the way GCC's fixincludes did.
#
#   fix-macosx-10.0-headers.sh SYSROOT
#
# - 10.0's <ppc/types.h> declares intptr_t and uintptr_t as pointers; the
#   10.2 SDK has them as integers, as C99 requires. The types keep their
#   size, so nothing compiled before changes.
# - CarbonCore's <fp.h> declares FP_ZERO and the like in an enum, which
#   clashes with the C99 macros of the <math.h> from build-c99-macosx.sh;
#   skip the enum when they are already there (the 10.2 SDK's fp.h leaves
#   them to math.h).
#
# Run it once on a 10.0 sysroot; it does nothing to headers already fixed.
set -e
: "${1:?usage: $0 SYSROOT}"
h=$1/usr/include/ppc/types.h
[ -f "$h" ] || { echo "$h not found" >&2; exit 1; }
sed -i.orig \
  -e 's/^typedef int[[:space:]]*\*intptr_t;/typedef long int\t\tintptr_t;/' \
  -e 's/^typedef unsigned long[[:space:]]*\*uintptr_t;/typedef unsigned long int\tuintptr_t;/' \
  "$h"
cmp -s "$h" "$h.orig" && echo "$h: already fixed" || echo "fixed $h"
rm -f "$h.orig"

fp=$1/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/fp.h
if [ -f "$fp" ] && ! grep -q '^#ifndef FP_ZERO' "$fp"; then
  awk '
    /^enum \{/ { held = $0; next }
    held != "" {
      if ($0 ~ /FP_SNAN/) { print "#ifndef FP_ZERO /* the C99 <math.h> may have them */"; fix = 1 }
      print held; held = ""
    }
    { print }
    fix && /^};/ { print "#endif"; fix = 0 }
  ' "$fp" >"$fp.new"
  mv "$fp.new" "$fp"
  echo "fixed $fp"
fi
