#!/bin/sh
#set -u
#set -e
#set -x
#Usage:   ./get_riscv_func_inst.sh proc_name func_name byte_nums file_name
#Example: ./get_riscv_func_inst.sh ./test/acc-read-func helper 8 func_helper.tmp
byte_nums=$3

reverse_word_order(){
    result=
    for word in $@; do
        result="$word $result"
    done
    echo "$result" 
}

reverse_str_order(){
    result=
    for word in $@; do
        strlen=${#word}
	tmp=
        i=$((strlen-2))
        while [ $i -ge 0 ]; do
            tmp=$(echo "$word" | cut -c "$i-$((i+1))")
            i=$((i-2))
        done

            result="$result $tmp"
    done
    echo "$result"

}

get_dw_opcode(){
    result=
    count=0
    for word in $@; do
        if [ $count -eq $byte_nums ]; then
            break;
        fi
        result="$result $word"
        count=$((count + 1))
    done
    echo "$result"
}

delete_blank(){
    result=
    for word in $@; do
        result="$result$word"
    done
    echo "$result"
}

func_addr="$( objdump -C -t "$1" | grep "$2" | grep -o -E "^[a-f0-9]+" )"
func_addr="0x""${func_addr}"
opcode_oneline=$(objdump -C -d --start-address="${func_addr}" "$1" | sed -n '/.*<helper(*.*)*>:/,/^$/p' | sed '1d' | grep -E -o ":\s+[a-f0-9]+\s*" | grep -E -o "[a-f0-9]+\s*" | xargs)
# revert twice to revert group(inner ordered)
opcode_revstr_oneline=$(reverse_str_order "$opcode_oneline")
opcode_blank_revstr_oneline=$(echo $opcode_revstr_oneline| sed 's/\([a-f0-9][a-f0-9]\)/\1 /g')
opcode_be_dw=$(get_dw_opcode "$opcode_blank_revstr_oneline")
opcode_le_dw=$(reverse_word_order "$opcode_be_dw")
opcode_unsignedlonglong=$(delete_blank "$opcode_le_dw")
echo $opcode_unsignedlonglong > $4
