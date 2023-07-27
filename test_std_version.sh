#!/bin/sh
#OPTS="$OPTS -std=c++23 -std=c++2b"
OPTS="$OPTS -std=c++20 -std=c++2a"
OPTS="$OPTS -std=c++17 -std=c++1z"
OPTS="$OPTS -std=c++14 -std=c++1y"
OPTS="$OPTS -std=c++11 -std=c++0x"

for s in $OPTS; do
  if "$1" "$s" -E /dev/null 2>/dev/null; then
    echo "$s"
    exit
  fi
done
# Default option
echo -std=c++11
