#!/bin/sh
export TIME='[38;5;171muser [38;5;223m%U[38;5;171m, system [38;5;223m%S[38;5;171m, elapsed [38;5;223m%E[38;5;171m, CPU [38;5;223m%P[38;5;171m, mem [38;5;223m%Mk[m'
time="$(time /bin/sh -c "$* 1>&8 2>&7" 2>&1)" 8>&1 7>&2
m=$?
if [ ! $m = 0 ]; then
  exit $m
fi
next=0
out="?"
for s in "$@"; do
  if [ "$next" = 1 ]; then
    out="$s"
  fi
  next=0
  if [ "$s" = "-o" ]; then
    next=1
  fi
done
echo "$out	$time"
