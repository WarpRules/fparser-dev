#!/bin/sh
time="$(/usr/bin/time "$@" 2>&1 >&7)" 7>&1
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
