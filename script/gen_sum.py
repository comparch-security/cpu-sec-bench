#!/usr/bin/python3

import re
import sys
from operator import add
from functools import reduce

db = {
    "mss": [0, 0],
    "mts": [0, 0],
    "acc": [0, 0],
    "cpi": [0, 0],
    "cfi-b": [0, 0],
    "cfi-f": [0, 0],
    "total": [0, 0]
}

for entry in sys.stdin:
    result = entry.split(' ')
    case_name = result.pop(0)
    if case_name in [
            "cfi-call-wrong-type-arg-vtable",
            "cfi-call-wrong-type-arg-vtable-heap",
            "cfi-call-wrong-type-arg-vtable-with-data-modification",
            "cfi-call-wrong-vtable-heap",
            "cfi-call-wrong-num-arg-vtable-heap",
            "cfi-call-wrong-num-func-vtable"]:
        continue
    case_name = case_name.split('-')
    if case_name[0] == "cfi":
        if case_name[1] == "return":
            case_name[0] = "cfi-b"
        else:
            case_name[0] = "cfi-f"
    db["total"][1] += 1
    db[case_name[0]][1] += 1
    if int(result[0]) == 0:
        db["total"][0] += 1
        db[case_name[0]][0] += 1
        
print("mss   " + str(db["mss"]))
print("mts   " + str(db["mts"]))
print("acc   " + str(db["acc"]))
print("cpi   " + str(db["cpi"]))
print("cfi-b " + str(db["cfi-b"]))
print("cfi-f " + str(db["cfi-f"]))
print("total " + str(db["total"]))
