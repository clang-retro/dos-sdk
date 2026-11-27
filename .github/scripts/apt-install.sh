#!/bin/sh
# Install Ubuntu packages on a GitHub Actions runner.
#
# Usage: sh apt-install.sh PACKAGE...
#
# The runner image has package sources that the clang-dos jobs do not use,
# for example packages.microsoft.com. When one of these sources fails,
# "apt-get update" fails and the job stops. Also, a mirror can remove a
# package version after "apt-get update" read the index; then
# "apt-get install" gets "404 Not Found". This script prevents the two
# failures:
#   - It removes the sources that are not Ubuntu sources.
#   - It reads the index again and tries again when an install fails.
set -eu

# Remove the package sources that are not Ubuntu sources.
# The jobs install only Ubuntu packages, so they do not need these sources.
remove_other_sources() {
  for f in /etc/apt/sources.list.d/*; do
    [ -f "$f" ] || continue
    if grep -qE 'packages\.microsoft\.com|azure\.archive' "$f"; then
      echo "apt-install: remove $f"
      sudo rm -f "$f"
    fi
  done
}

# Read the package index. Try again three times when a download fails.
update_index() {
  n=1
  until sudo apt-get update -q -o Acquire::Retries=3; do
    [ "$n" -ge 3 ] && return 1
    n=$((n + 1))
    echo "apt-install: apt-get update failed, try $n of 3"
    sleep 10
  done
}

# Install the packages that the arguments name. When the install fails,
# read the index again and try again, three tries at most.
install_packages() {
  n=1
  until sudo DEBIAN_FRONTEND=noninteractive apt-get install -y -q \
      --no-install-recommends -o Acquire::Retries=3 "$@"; do
    [ "$n" -ge 3 ] && return 1
    n=$((n + 1))
    echo "apt-install: apt-get install failed, try $n of 3"
    sleep 10
    update_index
  done
}

[ "$#" -gt 0 ] || { echo "usage: $0 PACKAGE..." >&2; exit 2; }
remove_other_sources
update_index
install_packages "$@"
