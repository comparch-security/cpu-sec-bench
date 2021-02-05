#!/usr/bin/python2

import sys
import os.path
import subprocess
from functools import reduce

sys.path.append(os.path.relpath("../script"))
import database

# check the dependence of a tests
def check_dependence(test):
    depend_tests = database.cfg_get_depends(test)
    if len(depend_tests) == 0:
        return 0
    if reduce((lambda x, y: x or y), map(lambda x: database.result_get_result(x) == -1, depend_tests)):
        return -1
    if reduce((lambda x, y: x and y), map(lambda x: database.result_get_result(x) == 0, depend_tests)):
        return 0
    else:
        return 1

# process when the some depended tests are not tested yet
def proc_when_untested(test, dep):
    tests.append(test)

# process when the dependence checked out failed
def proc_when_fail(test, dep):
    database.result_record_result(test, 1024)

# process when dependence checked out ok
def proc_when_ok(test, dep):
    test_prog = database.cfg_get_prog(test)
    argument = database.cfg_get_arguments(test)
    expected_results = database.cfg_get_expected_results(test)
    try:
        print("./" + test_prog + " " + argument)
        subprocess.check_call(
            "./" + test_prog + " " + argument,
            stderr=subprocess.STDOUT,
            shell=True
        )
        database.result_record_result(test, 0)
    except subprocess.CalledProcessError as e:
        if not str(e.returncode) in expected_results:
            print(test, "** FAIL! ** return:", e.returncode)
        database.result_record_result(test, e.returncode)

# process when dependence check returns a wrong result
def proc_when_unknown(test, dep):
    print(test, "** FAIL! ** dependence check returns ", dep)

# dependence dispatcher
dependence_dispatcher = {
    -1: proc_when_untested,
    0:  proc_when_ok,
    1:  proc_when_fail
}

# read in the configuration database
database.open_cfg_db()

tests = list(database.cfg_get_tests())

while (len(tests) > 0):
    test = tests.pop(0)
    dep = check_dependence(test)
    func = dependence_dispatcher.get(dep, proc_when_unknown)
    func(test, dep)
    continue

# write out the result
database.write_result_db()
