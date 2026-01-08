from runner import run_shell

def test_echo_basic():
    run_shell("echo hello")

def test_echo_args():
    run_shell('echo a b c')

def test_echo_empty():
    run_shell("echo")

def test_echo_quotes():
    run_shell("echo 'keep space'")

def test_echo_double_quotes():
    run_shell("echo \"keep space\"")

def test_echo_comment():
    run_shell('echo test # MAAARGEEEE')

#def test_fail():
#  run_shell("echo hello", expected_exit=42)