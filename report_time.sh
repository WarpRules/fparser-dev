#!/bin/sh
time="$(time "$@" 2>&1 1>&7)" 7>&1
m=$?
if [ ! $m = 0 ]; then
  echo "$time"
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
