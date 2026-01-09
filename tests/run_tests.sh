#!/bin/sh
set -u

BIN_PATH="${BIN_PATH:-}"
OUTPUT_FILE="${OUTPUT_FILE:-/tmp/42sh_out_$$}"
COVERAGE="${COVERAGE:-}"
REF_SH="bash --posix"

passed=0
total=0

run_test() {
  name="$1"
  input="$2"

  out_ref="/tmp/ref_$$.out"
  out_goat="/tmp/goat_$$.out"

  touch "$out_ref"
  touch "$out_goat"

  failed=0

  printf "%s" "$input" | $REF_SH > "$out_ref"
  code_ref=$?

  printf "%s" "$input" | "$BIN_PATH" > "$out_goat"
  code_goat=$?

  if [ "$code_ref" -ne "$code_goat" ]; then
    failed=1
    echo "- FAILED: $name (EXIT CODE): ref=$code_ref, goat=$code_goat"
  fi

  if ! cmp -s "$out_ref" "$out_goat"; then
    failed=1
    echo "- FAILED: $name (output diff)"
    diff "$out_ref" "$out_goat"
  fi

  if [ "$failed" -eq 0 ]; then
    echo "[OK] $name"
    passed=$((passed + 1))
  fi

  total=$((total + 1))
  rm -f "$out_ref" "$out_goat"

}

run_unit()
{
  name="$1"
  bin="$2"

  total=$((total + 1))

  if [ ! -x "$bin" ]; then
    echo "Binary file: $bin not found"
    return
  fi

  if "$bin"; then
    echo "[OK] $name"
    passed=$((passed + 1))
  else
    echo "- FAILED: $name"
  fi
}

run_test "true test" "true"
run_test "false test" "false"

run_test "simple echo" "echo HOMMERR"
run_test "echo -n" "echo -n DONNUUT"

run_test "Simple comment" "echo SUCRE # AU SUCRE"
run_test "Comment inside" "echo thibault#bikini"

run_test "Simple list" "echo paul; echo baptiste"


if [ "$COVERAGE" = "yes" ]; then
  run_unit "unit: echo" "./test_echo"
  run_unit "unit: io_string_to_file" "./test_io_string_to_file"
fi


pct=0
if [ "$total" -gt 0 ]; then
  pct=$((passed * 100 / total))
fi

echo "----------------------------------------"
echo "Result: $passed / $total => $pct%"

if [ -n "$OUTPUT_FILE" ]; then
  printf "%d" "$pct" > "$OUTPUT_FILE"
fi

exit 0