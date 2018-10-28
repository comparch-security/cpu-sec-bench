#!/usr/bin/python3

import sys
import os.path
import subprocess

fname = sys.argv[1]
bname = os.path.basename(fname)

# set argument for special tests
arguments = {
    "cfi-wrong-num-func-vtable": "0"
}

# run the test
try:
    subprocess.check_call(
        [fname, arguments.get(bname, "")],
        stderr=subprocess.STDOUT,
        shell=True
    )
except subprocess.CalledProcessError as e:
    print(bname, "** FAIL! ** return:", e.returncode)
