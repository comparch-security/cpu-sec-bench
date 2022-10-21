#!/bin/bash
set -u
set -e
set -x
#Usage:   ./get_aarch64_func_inst.sh proc_name func_name file_name
#Example: ./get_aarch64_func_inst.sh ./test/acc-read-func helper func_helper.tmp
func_addr="$( objdump -C -t "$1" | grep "$2" | grep -o -E "^[a-f0-9]+" )"
func_addr="0x""${func_addr}"
objdump -C -d --start-address="${func_addr}" "$1" | sed -n '/.*<helper(.*)>:/,/^$/p' | sed '1d' | grep -E -o "\s+([a-f0-9][a-f0-9] )+\s*" > "$3"