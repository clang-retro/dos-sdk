#!/bin/sh
# Runs a 32-bit DOS program (or any DOS program) in DOSBox without a window
# and writes its output (stdout) here:
#
#   dosbox-run.sh PROGRAM.EXE [ARGUMENTS]
#
# The program runs as C:\RUN.EXE (or RUN.COM) in an empty temporary
# directory. The exit status is 0 when the DOS exit code is 0, else 1.
#
#   DOSBOX   the DOSBox program (default: dosbox)
#   TIMEOUT  seconds before DOSBox is stopped (default: 60)
#   MEMSIZE  MB of memory (default: 16)
set -u
prog=${1:?usage: dosbox-run.sh PROGRAM [ARGUMENTS]}
shift
dir=$(mktemp -d)
trap 'rm -rf "$dir"' EXIT
# The copy gets a short name, because DOS names have 8 + 3 characters.
case $prog in *.com | *.COM) name=RUN.COM ;; *) name=RUN.EXE ;; esac
cp "$prog" "$dir/$name"
cat >"$dir/run.conf" <<CONF
[sdl]
output=surface
[dosbox]
memsize=${MEMSIZE:-16}
[cpu]
cycles=max
[mixer]
nosound=true
[midi]
mpu401=none
mididevice=none
[sblaster]
sbtype=none
[gus]
gus=false
[speaker]
pcspeaker=false
tandy=off
disney=false
[autoexec]
mount c "$dir"
c:
$name $* > OUT.TXT
if errorlevel 1 echo fail > RC.TXT
exit
CONF
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout "${TIMEOUT:-60}" \
  "${DOSBOX:-dosbox}" -conf "$dir/run.conf" -exit >/dev/null 2>&1
# DOS writes CR LF. DOSBox makes RC.TXT also when the "if" is false, so
# the exit code is 0 when RC.TXT is empty.
[ -f "$dir/OUT.TXT" ] && tr -d '\r' <"$dir/OUT.TXT"
[ ! -s "$dir/RC.TXT" ] && [ -f "$dir/OUT.TXT" ]
