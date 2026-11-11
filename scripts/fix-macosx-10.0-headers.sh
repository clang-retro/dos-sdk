#!/bin/sh
# Fix Mac OS X 10.0's headers in a sysroot so C++ (libc++, libunwind)
# builds against them, the way GCC's fixincludes did.
#
#   fix-macosx-10.0-headers.sh SYSROOT
#
# 10.0's <ppc/types.h> declares intptr_t and uintptr_t as pointers; the 10.2
# SDK has them as integers, as C99 requires. Run it once on a 10.0 sysroot; it
# does nothing to headers that are already fixed. The types keep their
# size, so nothing compiled before changes.
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
