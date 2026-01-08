#!/bin/sh
set +e

BINPATH="${BINPATH:-$(pwd)/../src/42sh}"
TOTAL=0
PASS=0

# --- 1) Functional tests (Python) ---
TOTAL=$((TOTAL + 1))
timeout 50s env BINPATH="$BINPATH" ./run_tests.sh
if [ $? -eq 0 ]; then
  PASS=$((PASS + 1))
fi

# --- 2) Unit tests (C) uniquement en coverage mode ---
if [ "x$COVERAGE" = "xyes" ]; then
  for t in unit/test_echo unit/test_io_stdin_to_file unit/test_io_string_to_file; do
    if [ -x "$t" ]; then
      TOTAL=$((TOTAL + 1))
      timeout 2s "./$t"
      if [ $? -eq 0 ]; then
        PASS=$((PASS + 1))
      fi
    fi
  done
fi

# Pourcentage entier 0..100, tronqué
if [ "$TOTAL" -eq 0 ]; then
  PCT=0
else
  PCT=$((PASS * 100 / TOTAL))
fi

if [ -n "$OUTPUTFILE" ]; then
  printf "%s" "$PCT" > "$OUTPUTFILE"
fi

exit 0
