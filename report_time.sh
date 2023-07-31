#!/bin/sh
export TIME='[38;5;171muser [38;5;223m%U[38;5;171m, system [38;5;223m%S[38;5;171m, elapsed [38;5;223m%E[38;5;171m, CPU [38;5;223m%P[38;5;171m, mem [38;5;223m%Mk[m'

test="$(/bin/sh -c 'true >&3' 2>/dev/null)" 3>&1
if [ $? = 0 ]; then
  # This command works in dash and zsh, but not in bash.
  # It appears bash does not handle redirections in a variable assignment command.
  #
  #        time="$(wc)" < Makefile
  time="$(time /bin/sh -c "$* 1>&8 2>&7" 2>&1)" 8>&1 7>&2
  m=$?
else
  # So we use a temporary file instead.
  #
  TEMPFN="${TMPDIR:-/tmp}"/.report_time.$$
  trap "rm \"$TEMPFN\"" 1 2 11 13 15
  "`which time`" /bin/sh -c "$* 1>&8 2>&7" 2>$TEMPFN 8>&1 7>&2
  m=$?
  time="$(< "$TEMPFN")"
  rm -f "$TEMPFN"
fi

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
