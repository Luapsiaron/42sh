#!/bin/sh
set -u

BIN_PATH="${BIN_PATH:-}"
OUTPUT_FILE="${OUTPUT_FILE:-/tmp/42sh_out_$$}"
COVERAGE="${COVERAGE:-}"
REF_SH="bash --posix"

passed=0
total=0

#COLORS
RED='\033[38;5;210m'
GREEN='\033[38;5;114m'
ORANGE='\033[38;5;215m'
BLUE='\033[38;5;117m'
YELLOW='\033[38;5;226m'
CANCEL='\033[0m'

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

  printf "%s" "$input" | timeout 1s "$BIN_PATH" > "$out_goat"
  code_goat=$?

  if [ "$code_ref" -ne "$code_goat" ]; then
    failed=1
    echo "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(EXIT CODE)${CANCEL}: ref=${GREEN}$code_ref${CANCEL}, loosers=${RED}$code_goat${CANCEL}"
  fi

  if ! cmp -s "$out_ref" "$out_goat"; then
    failed=1
    echo "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(OUTPUT)${CANCEL}"
    diff "$out_ref" "$out_goat"
  fi

  if [ "$failed" -eq 0 ]; then
    echo "[${GREEN}OK${CANCEL}] $name"
    passed=$((passed + 1))
  fi

  if [ "$code_goat" -eq 124 ]; then #124 = $? of timeout
	  echo "[${BLUE}TIMEOUT${CANCEL}] $name timeout !!!!!!!!!"
  fi

  total=$((total + 1))
  rm -f "$out_ref" "$out_goat"

}


run_script()
{
  name="$1"
  script="$2"

  total=$((total + 1))

  out_ref="/tmp/ref_script_$$.out"
  out_goat="/tmp/goat_script_$$.out"

  printf "%s\n" "./$script" | $REF_SH > "$out_ref"
  code_ref=$?

  printf "%s\n" "./$script" | timeout 1s "$BIN_PATH" > "$out_goat"
  code_goat=$?

  failed=0

  if [ "$code_ref" -ne "$code_goat" ]; then
    failed=1
    echo "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(EXIT CODE)${CANCEL}: ref=${GREEN}$code_ref${CANCEL}, loosers=${RED}$code_goat${CANCEL}"
  fi

  if ! cmp -s "$out_ref" "$out_goat"; then
    failed=1
    echo "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(OUTPUT)${CANCEL}"
    diff "$out_ref" "$out_goat"
  fi

  if [ "$failed" -eq 0 ]; then
    echo "[${GREEN}OK${CANCEL}] $name"
    passed=$((passed + 1))
  fi
  
  if [ "$code_goat" -eq 124 ]; then #124 = $? of timeout
	  echo "[${BLUE}TIMEOUT${CANCEL}] $name timeout !!!!!!!!!"
  fi
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


# ================= RUN STRING ===================

run_test "LS" "ls"
run_test "Tree -a" "tree -a"

run_test "cat Makefile.am" "cat Makefile.am"


run_test "Cd .. test" "cd .."

run_test "true test" "true"
run_test "false test" "false"

run_test "simple echo" "echo HOMMERR"
run_test "echo -n" "echo -n DONNUUT"
run_test "Echo 2 flags" "echo -n -e La 1ere fois de Baptiste: Le vendredi 9 Janvier au soir;"
run_test "Echo 3 flags" "echo -n -e -E Et oui c etait juste avant un rendu"
run_test "Echo mix" "echo -n -e -n AAAAAAAAAAAh"
run_test "Echo escaped" "echo \" \\n \\t \\\\\""
run_test "Echo bad flags" "echo -nEe Machoire"

run_test "Simple comment" "echo SUCRE # AU SUCRE"
run_test "Comment inside" "echo thibault#bikini"

run_test "Simple list" "echo paul; echo baptiste"
# run_test "List" "echo a;; echo b"
run_test "List newline" "echo a;
echo b"
run_test "if with list" "if echo cond; true; then echo body; fi"


run_test "Simple double echo" "echo 1; echo 2;"
run_test "Harder shell" "if true; then echo coco; echo mangue; else false; echo dragon; fi"
run_test "If inside if" "if true; then if false; then echo no; else echo yes; fi; fi"

run_test "Simple quote test" "echo 'aaa;  simple quote'"
run_test "quote inside" "echo a'b'c"
run_test "triple simple" "echo 'a' 'b' 'c'"
run_test "SQuote + DQuote Concat" "echo 'Simple'\"Double\""
run_test "Empty quotes" "echo '' \"\""
run_test "Empty concat" "echo 'a'\"\"'b'"
run_test "triple quoted mixed" "echo 'a'\"b\"C"


run_test "Expand simple" "A=10; echo \$A"
run_test "Expand Dquote" "A=11; echo \"Res: \$A\""
run_test "Expand SQuote" "A=12; echo 'Res: \$A'"
run_test "Expand concat" "A=12; echo q\$Aw"
run_test "True Exit code" "true; echo \$?"
run_test "False Exit code" "false; echo \$?"

# run_test "Weird ls" ";;;;ls;;;;;;;"
# ================= RUN SCRIPT ===================

run_script "script test 1" "script/script.sh"
run_script "script test 2" "script/script1.sh"

# ================= RUN UNIT =====================

if [ "$COVERAGE" = "yes" ]; then
  run_unit "unit: ast" "./test_ast"
  run_unit "unit: echo" "./test_builtins"
  run_unit "unit: io" "./test_io"
  run_unit "unit: lexer" "./test_lexer"
  
fi

pct=0
if [ "$total" -gt 0 ]; then
  pct=$((passed * 100 / total))
fi

echo "----------------------------------------"
echo "${YELLOW}Result: $passed / $total => $pct% ${CANCEL}"

if [ -n "$OUTPUT_FILE" ]; then
  printf "%d" "$pct" > "$OUTPUT_FILE"
fi

exit 0