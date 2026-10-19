#!/bin/bash
#set -u
#set -e
#set -x
#Usage:   ./get_x64_func_inst.sh proc_name func_name byte_nums file_name
#Example: ./get_x64_func_inst.sh ./test/acc-read-func helper 8 func_helper.tmp
byte_nums=$3

# The extraction honors the bench's published OBJDUMP knob: the Makefile passes
# the platform's objdump in, so no tool is resolved through PATH anymore (a bare
# `objdump` once picked the x86_64 one on the aarch64 lanes).
objdump_bin="${OBJDUMP:-objdump}"
if ! command -v "$objdump_bin" > /dev/null 2>&1; then
    echo "get_x64_func_inst.sh: objdump '$objdump_bin' (OBJDUMP='${OBJDUMP:-<unset>}') not found" >&2
    exit 1
fi
# Bound every objdump invocation: an externally supplied tool that hangs would
# otherwise stall the build and the scheduler indefinitely (run-test.cpp's
# run_cmd has no timeout).  `timeout` is coreutils and is absent on a stock
# macOS, so fall back to an unbounded call there.
if command -v timeout > /dev/null 2>&1; then
    objdump_run(){ timeout 30 "$@"; }
else
    objdump_run(){ "$@"; }
fi
# Audit line: name the resolved tool and its version, so a build log shows which
# objdump an extraction actually used.
objdump_version=$(objdump_run "$objdump_bin" --version 2>/dev/null | head -n 2 | tr '\n' ' ' | sed 's/ *$//')
echo "get_x64_func_inst.sh: using objdump '$(command -v "$objdump_bin")' version '${objdump_version}'"

reverse_word_order(){
    local result=
    for word in $@; do
        result="$word $result"
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

func_table=$( objdump_run "$objdump_bin" -C -t "$1" ) || {
    echo "get_x64_func_inst.sh: objdump '$objdump_bin' failed to read the symbol table of '$1' (exit $?)" >&2
    exit 1
}
func_addrs=$( printf '%s\n' "$func_table" | awk -v s="$2" '$NF == s {print $1}' )
func_addr_count=$( printf '%s\n' "$func_addrs" | grep -c . )
if [ "$func_addr_count" -ne 1 ]; then
    echo "get_x64_func_inst.sh: expected exactly one symbol named '$2' in '$1', found $func_addr_count (objdump '$objdump_bin')" >&2
    exit 1
fi
func_addr="0x""${func_addrs}"
# Select the requested symbol's disassembly by its address span: start at the
# label carrying the matched address and stop at the next label or blank line.
# A tool that cannot decode the binary must fail the extraction rather than feed
# a partial listing into the byte parse.
func_dump=$( objdump_run "$objdump_bin" -C -d --start-address="${func_addr}" "$1" ) || {
    echo "get_x64_func_inst.sh: objdump '$objdump_bin' failed to disassemble '$1' (exit $?)" >&2
    exit 1
}
opcode_oneline=$(printf '%s\n' "$func_dump" |
                 awk -v a="${func_addrs}" '
                     $1 == a && /^[0-9a-fA-F]+ <.*>:$/ { inside = 1; next }
                     !inside { next }
                     /^$/ { exit }
                     /^[0-9a-fA-F]+ <.*>:$/ { exit }
                     { print }' |
                 grep -E -o "\s+([a-f0-9][a-f0-9] )+\s*" | xargs)
opcode_be_dw=$(get_dw_opcode "$opcode_oneline")
opcode_le_dw=$(reverse_word_order "$opcode_be_dw")
opcode_unsignedlonglong=$(delete_blank "$opcode_le_dw")
if [ -z "$opcode_unsignedlonglong" ]; then
    echo "get_x64_func_inst.sh: extracted no opcode bytes for '$2' from '$1' (objdump '$objdump_bin')" >&2
    exit 1
fi
# The consumer parses the side file as a hex number of ANY width, so a partial
# listing from a tool that exits 0 would otherwise be accepted silently and
# yield a wrong-but-well-formed word.  Assert the exact width and hex shape
# here, and name the tool in each message so a bad tool is identifiable from
# the log.
case "$opcode_unsignedlonglong" in
    *[!0-9a-f]*)
        echo "get_x64_func_inst.sh: extracted non-hex opcode '$opcode_unsignedlonglong' for '$2' from '$1' (objdump '$objdump_bin')" >&2
        exit 1
        ;;
esac
if [ "${#opcode_unsignedlonglong}" -ne $((2 * byte_nums)) ]; then
    echo "get_x64_func_inst.sh: extracted ${#opcode_unsignedlonglong} hex digits for '$2' from '$1', expected $((2 * byte_nums)) (objdump '$objdump_bin')" >&2
    exit 1
fi
echo $opcode_unsignedlonglong > $4



