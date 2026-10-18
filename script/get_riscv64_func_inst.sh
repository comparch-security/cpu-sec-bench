#!/bin/bash
#set -u
#set -e
#set -x
#Usage:   ./get_riscv_func_inst.sh proc_name func_name byte_nums file_name
#Example: ./get_riscv_func_inst.sh ./test/acc-read-func helper 8 func_helper.tmp
byte_nums=$3

reverse_word_order(){
    local result=
    for word in $@; do
        result="$word $result"
    done
    echo "$result" 
}

reverse_str_order(){
    local result=
    for word in $@; do
        local strlen=${#word}
	local tmp=
	    for (( i=$strlen-2; i >=0; i=$i-2)); do
	        tmp=$tmp${word:$i:2}
	    done
            result="$result $tmp"
    done
    echo "$result"

}

get_dw_opcode(){
    local result=
    local count=0
    for word in $@; do
        if [ $count -eq $byte_nums ]; then
            break;
        fi
        result="$result $word"
        count=$[$count+1]
    done
    echo "$result"
}

delete_blank(){
    local result=
    for word in $@; do
        result="$result$word"
    done
    echo "$result"
}

func_addrs="$( objdump -C -t "$1" | awk -v s="$2" '$NF == s {print $1}' )"
func_addr_count=$( printf '%s\n' "$func_addrs" | grep -c . )
if [ "$func_addr_count" -ne 1 ]; then
    echo "get_riscv64_func_inst.sh: expected exactly one symbol named '$2' in '$1', found $func_addr_count" >&2
    exit 1
fi
func_addr="0x""${func_addrs}"
# Select the requested symbol's disassembly by its address span: start at the
# label carrying the matched address and stop at the next label or blank line.
opcode_oneline=$(objdump -C -d --start-address="${func_addr}" "$1" |
                 awk -v a="${func_addrs}" '
                     $1 == a && /^[0-9a-fA-F]+ <.*>:$/ { inside = 1; next }
                     !inside { next }
                     /^$/ { exit }
                     /^[0-9a-fA-F]+ <.*>:$/ { exit }
                     { print }' |
                 grep -E -o ":\s+[a-f0-9]+\s*" | grep -E -o "[a-f0-9]+\s*" | xargs)
# revert twice to revert group(inner ordered)
opcode_revstr_oneline=$(reverse_str_order "$opcode_oneline")
opcode_blank_revstr_oneline=$(echo $opcode_revstr_oneline| sed 's/\([a-f0-9][a-f0-9]\)/\1 /g')
opcode_be_dw=$(get_dw_opcode "$opcode_blank_revstr_oneline")
opcode_le_dw=$(reverse_word_order "$opcode_be_dw")
opcode_unsignedlonglong=$(delete_blank "$opcode_le_dw")
if [ -z "$opcode_unsignedlonglong" ]; then
    echo "get_riscv64_func_inst.sh: extracted no opcode bytes for '$2' from '$1'" >&2
    exit 1
fi
echo $opcode_unsignedlonglong > $4
