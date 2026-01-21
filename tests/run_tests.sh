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
    echo -e "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(EXIT CODE)${CANCEL}: ref=${GREEN}$code_ref${CANCEL}, loosers=${RED}$code_goat${CANCEL}"
  fi

  if ! cmp -s "$out_ref" "$out_goat"; then
    failed=1
    echo -e "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(OUTPUT)${CANCEL}"
    echo -e "${YELLOW}Expected:${CANCEL}"
    sed 's/^/  /' "$out_ref"
    echo -e "${YELLOW}Got:${CANCEL}"
    sed 's/^/  /' "$out_goat"
  fi

  if [ "$failed" -eq 0 ]; then
    echo -e "[${GREEN}OK${CANCEL}] $name"
    passed=$((passed + 1))
  fi

  if [ "$code_goat" -eq 124 ]; then #124 = $? of timeout
	  echo -e "[${BLUE}TIMEOUT${CANCEL}] $name timeout !!!!!!!!!"
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
    echo -e "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(EXIT CODE)${CANCEL}: ref=${GREEN}$code_ref${CANCEL}, loosers=${RED}$code_goat${CANCEL}"
  fi

  if ! cmp -s "$out_ref" "$out_goat"; then
    failed=1
    echo -e "[${RED}FAILED${CANCEL}]: $name ${ORANGE}(OUTPUT)${CANCEL}"
    echo -e "${YELLOW}Expected:${CANCEL}"
    sed 's/^/  /' "$out_ref"
    echo -e "${YELLOW}Got:${CANCEL}"
    sed 's/^/  /' "$out_goat"
  fi

  if [ "$failed" -eq 0 ]; then
    echo -e "[${GREEN}OK${CANCEL}] $name"
    passed=$((passed + 1))
  fi
  
  if [ "$code_goat" -eq 124 ]; then #124 = $? of timeout
	  echo -e "[${BLUE}TIMEOUT${CANCEL}] $name timeout !!!!!!!!!"
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
    echo -e "${GREEN}[OK] $name${CANCEL}"
    passed=$((passed + 1))
  else
    echo -e "${RED}- FAILED: $name${CANCEL}"
  fi
}


# ================= RUN STRING ===================

# Simple commands
run_test "LS" "ls"
run_test "Tree -a" "tree -a"
run_test "cat Makefile.am" "cat Makefile.am"
run_test "Cd .." "cd ..; pwd"
run_test "Cd -" " cd/; cd /tmp; cd -"
run_test "Cd error no exist" "cd /coucou"
run_test "cd too many args" "cd /tmp /var"
run_test "Cd Home diff" "HOME=/tmp cd; pwd"
run_test "Cd home" "cd ; pwd" 


# Builtins
run_test "true test" "true"
run_test "false test" "false"
run_test "simple echo" "echo HOMMERR"
run_test "echo -n" "echo -n DONNUUT"
run_test "Echo 2 flags" "echo -n -e La 1 ere fois de Baptiste: Le vendredi 9 Janvier au soir;"
run_test "Echo 3 flags" "echo -n -e -E Et oui c etait juste avant un rendu"
run_test "Echo mix" "echo -n -e -n AAAAAAAAAAAh"
run_test "Echo escaped" "echo \" \\n \\t \\\\\""
run_test "Echo bad flags" "echo -n -E -e Machoire"
run_test "Simple comment" "echo SUCRE # AU SUCRE"
run_test "Comment inside" "echo thibault#bikini"

# Lists and ifs
run_test "Simple list" "echo paul; echo baptiste"
run_test "List newline" "echo a;
echo b"
run_test "if with list" "if echo cond; true; then echo body; fi"
run_test "Simple double echo" "echo 1; echo 2;"
run_test "Harder shell" "if true; then echo coco; echo mangue; else false; echo dragon; fi"
run_test "If inside if" "if true; then if false; then echo no; else echo yes; fi; fi"

# Quotes
run_test "Simple quote test" "echo 'aaa;  simple quote'"
run_test "quote inside" "echo a'b'c"
run_test "triple simple" "echo 'a' 'b' 'c'"
run_test "SQuote + DQuote Concat" "echo 'Simple'\"Double\""
run_test "Empty quotes" "echo '' \"\""
run_test "Empty concat" "echo 'a'\"\"'b'"
run_test "triple quoted mixed" "echo 'a'\"b\"C"
run_test "Squote inside Dquote" "echo \"'\""
run_test "Quote basckslashed" "echo \"\\\"\""
run_test "Full escaped" "echo \" \\ \\ \\ \\ \\ \\ \\ \\ \""

# Expansions
run_test "Expand simple" "A=10; echo \$A"
run_test "Expand Dquote" "A=11; echo \"Res: \$A\""
run_test "Expand SQuote" "A=12; echo 'Res: \$A'"
run_test "Expand concat" "A=12; echo q\$Aw"
run_test "True Exit code" "true; echo \$?"
run_test "False Exit code" "false; echo \$?"
run_test "Var in quotes" "Thibaut=Audrey; echo \"\$Thibaut OUI\""
run_test "Empty var in quotes" "A=; echo \"\$A\""
run_test "Var stuck in quotes" "A=1; echo \"\$A\"'test'"
run_test "Variable not defined" "echo [\$PASLA]"
run_test "PWD" "echo \$PWD"
run_test "IFS" "echo \$IFS"
run_test "Exit code" "true; echo \$?; false; echo \$?"
run_test "Arg numb" "echo \$#"
run_test "PID" "echo \$$" 


# Pipelines
run_test "Simple pipe" "echo Hello World | cat -e"
run_test "Pipe with spaces" "   echo    Hello    World    |    cat -e"
run_test "Pipe multiple" "echo line1 line2 line3 | grep line2 | wc -l"
run_test "Pipe with builtin" "! echo line1 line2 line3 | grep line2 | echo final"
run_test "Pipe with exit code" "echo line1 line2 line3 | grep line2 | false; echo \$?"

# Redirections
run_test "Simple output redir" "echo Hello > /tmp/test_output_redir.txt; cat /tmp/test_output_redir.txt"
run_test "Output redir append" "echo Line1 > /tmp/test_output_redir_append.txt; echo Line2 >> /tmp/test_output_redir_append.txt; cat /tmp/test_output_redir_append.txt"
run_test "Input redir" "echo Line1 > /tmp/test_input_redir.txt; echo Line2 >> /tmp/test_input_redir.txt; cat < /tmp/test_input_redir.txt"
run_test "Input redir with pipe" "echo Line1 > /tmp/test_input_redir_pipe.txt; echo Line2 >> /tmp/test_input_redir_pipe.txt; cat < /tmp/test_input_redir_pipe.txt | grep Line2"

# Negation
run_test "Negation of true" " ! true;"
run_test "Negation of false" " ! false;"
run_test "Negation with if" " if ! false; then echo yes; else echo no; fi"
run_test "Double negation" "! ! false"

# And
run_test "AND true true" "true && true; echo after_and"
run_test "AND true false" "true && false; echo after_and"
run_test "AND false true" "false && true; echo after_and"
run_test "AND false false" "false && false; echo after_and"

# Or
run_test "OR true true" "true || true; echo after_or"
run_test "OR true false" "true || false; echo after_or"
run_test "OR false true" "false || true; echo after_or"
run_test "OR false false" "false || false; echo after_or"

# While loop
run_test "Simple while loop" "A=0; while \$A == 3 ; do echo \$A; A=\$((A + 1)); done"
run_test "While loop never true" "A=0; while \$A == 5 ; do echo \$A; A=\$((A + 1)); done; echo end"

# For loop
run_test "Simple for loop" "for I in 1 2 3 ; do echo \$I; done"
run_test "For loop no iterations" "for I in 1; do echo \$I; done;"

# ================= RUN SCRIPT ===================

run_script "script test basic if" "script/script_basic_if.sh"
run_script "script test multiple if" "script/script_mul_if.sh"

run_script "script test error nl_semi_nl" "script/script_nl_semi_nl.sh"
run_script "script test fill_nl" "script/script_fill_nl.sh"

run_script "script test while" "script/script_while.sh"
run_script "script test for 2 forms" "script/script_2_for.sh"

run_script "script test if in if and elif" "script/if_in_if_elif.sh"

run_script "script test var @ complex" "script/echo_var@_cplx.sh"
run_script "script test var other" "script/echo_var_others.sh" 1 2 3
run_script "script test var environment" "script/echo_var_env.sh"

run_script "script test Piscine - tower" "script/tower.sh"
run_script "script test Piscine - ascii house" "script/ascii_house.sh"

# ================= RUN UNIT =====================

if [ "$COVERAGE" = "yes" ]; then
  run_unit "unit: ast" "./test_ast"
  run_unit "unit: echo" "./test_builtins"
  run_unit "unit: io" "./test_io"
  run_unit "unit: lexer" "./test_lexer"
  run_unit "unit: parser" "./test_parser"
  run_unit "unit: execution" "./test_execution"
fi

pct=0
if [ "$total" -gt 0 ]; then
  pct=$((passed * 100 / total))
fi

echo "----------------------------------------"
echo -e "${YELLOW}Result: $passed / $total => $pct% ${CANCEL}"

if [ -n "$OUTPUT_FILE" ]; then
  printf "%d" "$pct" > "$OUTPUT_FILE"
fi

exit 0