import pytest
import os
import subprocess
import tempfile

REF_SHELL = "bash"
#GOAT_SHELL = os.path.abspath("./42sh") #OUR SHELL
GOAT_SHELL = os.environ.get("BINPATH", "bash")

def pretty_format(expected_stdout, got_stdout, expected_exit = None, got_exit = None):
    errors = []
   
    if isinstance(expected_stdout, int):
        errors.append(f"EXIT CODE DIFF: expected={expected_stdout} got={got_stdout}")
        return "\n".join(errors)

    if expected_stdout.strip() != got_stdout.strip(): # strip spaces, can delete it if bad
        errors.append("STDOUT DIFF:")
        errors.append("EXPECTED:")
        errors.append(expected_stdout)
        errors.append("GOT:")
        errors.append(got_stdout)
    
    if expected_exit is not None and expected_exit != got_exit:
        errors.append(f"EXIT CODE DIFF: expected={expected_exit} got={got_exit}")
    
    return "\n".join(errors)



def run_shell(script, mode = "string", args=[], stdin_data="", expected_exit = None):
    __tracebackhide__ = True # hide helper in tests

    temp_path = None

    if mode == "file":
        with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix=".sh") as tmp: #create a temp shell file for file mode testing
            tmp.write(script)
            temp_path= tmp.name
        
        ref_cmd = [REF_SHELL, "--posix", temp_path] + args
        goat_cmd = [GOAT_SHELL, temp_path] + args

    else: # default mode/ string mode with -c, mise en liste car subprocess faix execve
        ref_cmd = [REF_SHELL, "--posix", "-c", script, "REFsh"] + args
        goat_cmd = [GOAT_SHELL,"-c", script, "42sh"] + args
    
    try:
        ref_process = subprocess.run(ref_cmd, input=stdin_data, capture_output=True, text=True)
        goat_process = subprocess.run(goat_cmd, input=stdin_data, capture_output=True, text=True, timeout=0.5)

        assert goat_process.stdout == ref_process.stdout, pretty_format(ref_process.stdout, goat_process.stdout)

        expected = ref_process.returncode if expected_exit is None else expected_exit
        
        assert goat_process.returncode == expected, pretty_format(expected, goat_process.returncode)

    finally:
        if temp_path and os.path.exists(temp_path):
            os.remove(temp_path)

