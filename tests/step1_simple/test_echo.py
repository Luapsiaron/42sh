from runner import run_shell

def echo_basic():
    run_shell("echo hello")

def echo_args():
    run_shell('echo a b c')

def echo_empty():
    run_shell("echo")

def echo_quotes():
    run_shell("echo 'keep space'")

def echo_comment():
    run_shell('echo test # MAAARGEEEE')