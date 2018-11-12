#!/usr/bin/python3

import sys
import os.path
import subprocess
import json
from functools import reduce

fname = sys.argv[1]
bname = os.path.basename(fname)

config_file = "configure.json"
result_file = "results.json"

# read configuration
with open(config_file, "r") as read_file:
    configDB = json.loads(read_file.read())

# read existing results
resultDB = {}
if os.path.isfile(result_file):
    with open(result_file, "r") as read_file:
        resultDB = json.loads(read_file.read())

arguments = configDB[bname]["arguments"]
depend_on_tests = configDB[bname]["depends-on"]
resultDB[bname] = {}

# check dependency

if len(depend_on_tests) != 0:
    depends_ok = reduce((lambda x, y: x and y), map(lambda x: resultDB[x]["result"] == 0, depend_on_tests))
else:
    depends_ok = True

# run the test
if depends_ok:
    try:
        subprocess.check_call(
            [fname, arguments],
            stderr=subprocess.STDOUT,
            shell=True
        )
        resultDB[bname]["result"] = 0
    except subprocess.CalledProcessError as e:
        print(bname, "** FAIL! ** return:", e.returncode)
        resultDB[bname]["result"] = e.returncode
else:
    resultDB[bname]["result"] = 1024

with open(result_file, "w") as write_file:
    json.dump(resultDB, write_file, indent=2, separators=(',', ': '), sort_keys=True)

