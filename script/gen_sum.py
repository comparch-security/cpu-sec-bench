#!/usr/bin/python3

import re
import os
import sys
import csv

directory_path = os.path.abspath(sys.argv[1])

for filename in os.listdir(directory_path):
    print(filename)
    if "resu" in filename:
        file_path = os.path.join(directory_path, filename)
        db = {
            "acc": [0, 0],
            "mss": [0, 0],
            "mts": [0, 0],
            "cpi": [0, 0],
            "cfi-b": [0, 0],
            "cfi-f": [0, 0],
            "total": [0, 0]
        }
        try:
            with open(file_path, 'r') as file:
                # Process the file content here (e.g., read lines, analyze data, etc.)
                print(f"Processing {filename}...")

                for entry in file:
                    result = entry.split()
                    case_name = result[0]
                    if case_name[0] == "#" or case_name == "Compilation" or case_name == "Run":
                        continue
                    result_val = result[2]
                    #print("case_name is {}, result is {}".format(case_name, result_val))
                    # if case_name in [
                    #         "cfi-call-wrong-type-arg-vtable",
                    #         "cfi-call-wrong-type-arg-vtable-heap",
                    #         "cfi-call-wrong-type-arg-vtable-with-data-modification",
                    #         "cfi-call-wrong-vtable-heap",
                    #         "cfi-call-wrong-num-arg-vtable-heap",
                    #         "cfi-call-wrong-num-func-vtable"]:
                    #     continue
                    case_name = case_name.split('-')
                    if case_name[0] == "cfi":
                        if case_name[1] == "return":
                            case_name[0] = "cfi-b"
                        else:
                            case_name[0] = "cfi-f"
                    db["total"][1] += 1
                    db[case_name[0]][1] += 1
                    if int(result_val) == 0:
                        db["total"][0] += 1
                        db[case_name[0]][0] += 1

                print("acc   " + str(db["acc"]))
                print("mss   " + str(db["mss"]))
                print("mts   " + str(db["mts"]))
                print("cpi   " + str(db["cpi"]))
                print("cfi-b " + str(db["cfi-b"]))
                print("cfi-f " + str(db["cfi-f"]))
                print("total " + str(db["total"]))

                # Open the file in write mode (w)
                with open('./results.csv', 'a', newline='') as f:
                    # Create the CSV writer
                    writer = csv.writer(f)
                    l = [filename]
                    for (k,n) in db.items():
                        l.append(n[0])
                    writer.writerow(l) 

        except FileNotFoundError:
            print(f"File {filename} not found.")
    else:
        continue
