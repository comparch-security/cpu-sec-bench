#!/bin/sh
set +x
#ensure the input csv has one blank line in the last, read command need linke break, else would read the last line
# Check if the correct number of arguments is provided
if [ $# -ne 2 ]; then
    echo "Usage: $0 <csv_file> [0:parrlel 1:sequential]"
    exit 1
fi

# Read the CSV file specified as a command-line argument
csv_file="$1"
run_method="$2"
. ./script/subscript-function.sh

collect_dir="results"
(
cd ..
rm -rf "$collect_dir"
mkdir -p "$collect_dir"
)

one_run_ind=0
# Read the CSV file
while IFS=, read -r prefix mode rest_columns; do
    # copy test bench directory and enter work directory
    
    if [ "$run_method" -eq 0 ]; then
        (
            if [ "$one_run_ind" -ne 0 ]; then
                new_dir="cpu-sec-bench-$one_run_ind"
                cd ..
                rm -rf "$new_dir"
                cp -r cpu-sec-bench "$new_dir"
                cd cpu-sec-bench-$one_run_ind || (
                    echo "$new_dir"
                    exit 1
                )
            fi

            export_func "$rest_columns"
            run_test "$prefix" "$mode" "$collect_dir"
        )&
    else
        (
            export_func "$rest_columns"
            run_test "$prefix" "$mode" "$collect_dir"
        )
    fi

    one_run_ind=$((one_run_ind + 1))
done < "$csv_file"

wait
