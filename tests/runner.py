import pytest
import os
import subprocess
import tempfile

REF_SHELL = "bash"
GOAT_SHELL = os.path.abspath("./42sh")

def pretty_format(expected, got):

def run_shell(script, mode = "string", args=[], stdin_data="", expected_exit = None):
    
    temp_path = None

    if mode == "file":
        with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix=".sh") as tmp:
            tmp.write(script)
            temp_path= tmp.name
        
        ref_cmd = [REF_SHELL, "--posix", temp_path] + args
        goat_cmd = [GOAT_SHELL, temp_path] + args

    else:
        ref_cmd = [REF_SHELL, "--posix", "-c", script, "sh_ref"] + args
        goat_cmd = [GOAT_SHELL,"-c", script, "sh_test"] + args
    
    try:
        ref_process = subprocess.run(ref_cmd, input=stdin_data, capture_output=True, text=True)
        goat_process = subprocess.run(ref_cmd, input=stdin_data, capture_output=True, text=True, timeout=0.5)

        assert goat_process.stdout == ref_process.stdout, pretty_format(ref_process.stdout, goat_process.stdout)

        expected = expected_exit if expected_exit else ref_process.returncode
        
        assert goat_process.returncode == expected, pretty_format(expected, goat_process.returncode)

    finally:
        if temp_path and os.path.exists(temp_path):
            os.remove(temp_path)

