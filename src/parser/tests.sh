#!/usr/bin/env bash
set -euo pipefail

BIN="${1:-./main}"

run_ok() {
  local name="$1"
  local script="$2"
  echo "== OK: $name"
  "$BIN" -c "$script" >/dev/null
}

run_ko() {
  local name="$1"
  local script="$2"
  echo "== KO: $name"
  if "$BIN" -c "$script" >/dev/null; then
    echo "  ❌ expected failure but succeeded"
    exit 1
  else
    echo "  ✅ failed as expected"
  fi
}

# ----- VALID -----
run_ok "single" "echo a"
run_ok "semicolon" "echo a; echo b"
run_ok "newline" $'echo a\necho b'
run_ok "trailing semicolon" "echo a;"
run_ok "multiple newlines" $'echo a\n\n\necho b'

run_ok "if simple" "if false; then echo ok; fi"
run_ok "if condition multi ;" "if false; true; then echo ok; fi"
run_ok "if condition multi \\n" $'if false\ntrue\nthen echo ok\nfi'
run_ok "if then multi" $'if false; then\necho a\necho b; echo c;\nfi'
run_ok "if else multi" $'if false; then echo a; else\necho b\necho c; echo d;\nfi'
run_ok "elif" "if false; then echo a; elif true; then echo b; fi"
run_ok "elif+else" "if false; then echo a; elif true; then echo b; else echo c; fi"
run_ok "elif chain" "if false; then echo a; elif false; then echo b; elif true; then echo c; fi"
run_ok "newlines after keywords" $'if false\nthen\necho a\nfi'

# ----- INVALID -----
run_ko "double semicolon" "echo a;; echo b"
run_ko "semicolon-space-semicolon" "echo a; ; echo b"

run_ko "if empty condition" "if then echo a; fi"
run_ko "if empty then" "if false; then fi"
run_ko "if empty else" "if false; then echo a; else fi"
run_ko "elif empty condition" "if false; then echo a; elif then echo b; fi"
run_ko "elif missing then" "if false; then echo a; elif true; echo b; fi"
run_ko "missing then" "if false; echo a; fi"
run_ko "missing fi" "if false; then echo a;"
run_ko "wrong order else" "if false; else echo a; fi"

# strictness tests (should fail if you require separators before THEN)
run_ko "no separator before then" "if false then echo a; fi"
run_ko "no separator in condition list" "if false true; then echo a; fi"

echo "All tests passed."

