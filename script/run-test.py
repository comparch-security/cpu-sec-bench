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
return_value = subprocess.call([fname, arguments.get(bname, "")])
if return_value != 0:
    print(bname, "** FAIL! ** return:", return_value)
