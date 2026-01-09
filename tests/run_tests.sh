#!/bin/sh
set -u

BIN_PATH="${BIN_PATH:-}"
OUTPUT_FILE="${OUTPUT_FILE:-/tmp/42sh_out_$$}"
COVERAGE="${COVERAGE:-}"

passed=0
total=0

run_test() {
  name="$1"
  shift
  total=$((total + 1))
  if "$@"; then
    passed=$((passed + 1))
    printf "[OK] %s\n" "$name"
  else
    printf "[KO] %s\n" "$name"
  fi
}


if [ -n "$BIN_PATH" ]; then
  # echo hello test
  run_test "func: echo hello" sh -c \
    'out="$(printf "echo hello\n" | "$BIN_PATH" 2>/dev/null)"; [ "$out" = "hello" ]'
  # echo Hello World! test
  run_test "func: echo Hello World!" sh -c \
    'out="$(printf "echo Hello World!\n" | "$BIN_PATH" 2>/dev/null)"; [ "$out" = "Hello World!" ]'
  # true test
  run_test "func: true exit=0" sh -c \
    'printf "true\n" | "$BIN_PATH" >/dev/null 2>/dev/null; [ "$?" -eq 0 ]'
  # false test
  run_test "func: false exit=1" sh -c \
    'printf "false\n" | "$BIN_PATH" >/dev/null 2>/dev/null; [ "$?" -eq 1 ]'
  # if -c test
  run_test "func: if true; then false; fi" sh -c \
    'out="$(printf "if true; then false; fi\n" | "$BIN_PATH" 2>/dev/null)"; [ "$?" -eq 1 ]'
  
fi


if [ "$COVERAGE" = "yes" ]; then
  [ -x "./test_echo" ] && run_test "unit: echo" ./test_echo
  [ -x "./test_io_string_to_file" ] && run_test "unit: io_string_to_file" ./test_io_string_to_file
  [ -x "./test_io_stdin_to_file" ] && run_test "unit: io_stdin_to_file" ./test_io_stdin_to_file
fi


pct=0
if [ "$total" -gt 0 ]; then
  pct=$((passed * 100 / total))
fi
if [ -n "$OUTPUT_FILE" ]; then
  printf "%d" "$pct" > "$OUTPUT_FILE"
fi

exit 0
