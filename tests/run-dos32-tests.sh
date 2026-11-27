#!/bin/sh
# Builds the 32-bit DOS tests (tests/dos32) at each optimization level and
# runs them in DOSBox (packaging/dosbox-run.sh). Reports which fail.
#
#   BIN      directory with clang and jwlink
#   SYSROOT  the i386-msdos sysroot (default: BIN/../i386-msdos)
#   DOSBOX   DOSBox (default: dosbox on PATH)
#   OPTS     optimization levels (default: O0 Os O2)
#   NO_CXX   set to skip the C++ tests
: "${BIN:?set BIN}"
tests=$(cd "$(dirname "$0")" && pwd)
run=$tests/../packaging/dosbox-run.sh
out=${WORK:-$PWD/build-dos32-tests}
mkdir -p "$out"
cd "$out" || exit 1
sys=${SYSROOT:+--sysroot=$SYSROOT}
pass=0 fail=0
# check NAME PATTERN COMMAND...: the command must succeed, and its output
# must have PATTERN (when it is not empty).
check() {
  name=$1 pattern=$2
  shift 2
  if "$@" >"$name.out" 2>&1 && { [ -z "$pattern" ] || grep -q -- "$pattern" "$name.out"; }; then
    pass=$((pass + 1))
  else
    fail=$((fail + 1))
    echo "FAIL $name"
    sed 's/^/    /' "$name.out" | head -20
  fi
}
for o in ${OPTS:-O0 Os O2}; do
  check "build libc-$o" "" "$BIN/clang" --target=i386-pc-msdos $sys -$o "$tests/dos32/libc.c" -o "libc$o.exe"
  check "run libc-$o" "PASS dos32 libc" sh "$run" "libc$o.exe" hello
  check "build hw-$o" "" "$BIN/clang" --target=i386-pc-msdos $sys -$o "$tests/dos32/hw.c" -o "hw$o.exe"
  check "run hw-$o" "PASS dos32 hw" sh "$run" "hw$o.exe"
  if [ -z "${NO_CXX:-}" ]; then
    check "build cxx-$o" "" "$BIN/clang++" --target=i386-pc-msdos $sys -$o "$tests/dos32/cxx.cpp" -o "cxx$o.exe"
    check "run cxx-$o" "PASS dos32 cxx" sh "$run" "cxx$o.exe"
    check "build threads-$o" "" "$BIN/clang++" --target=i386-pc-msdos $sys -$o "$tests/dos32/threads.cpp" -o "threads$o.exe"
    check "run threads-$o" "PASS dos32 threads" sh "$run" "threads$o.exe"
  fi
done
echo "== 32-bit DOS: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
