#! /bin/sh

# keep log name same with result name
rename_log() {
	if log_name=$(ls -t *.dat); then
		log_name=$(echo "${log_name}" | head -n 1)
		base_name="${log_name%.*}"
		# return base_name
		echo "$base_name"
	else
		exit 1
	fi
}

run_test(){
	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test "$2" >>temp.log 2>&1
	base_name=$(rename_log)
	final_name="$1"_"${base_name}"
	echo "$final_name"
	mv temp.log "$final_name".log
	mv "${base_name}".dat "$final_name".dat

	collect_results ../"$3"
}

collect_results() {

	# Find and copy .dat files
	find . -name "*.dat" -exec cp {} "$1" \;

	# Find and copy .log files
	find . -name "*.log" -exec cp {} "$1" \;
}

export_func(){
	rest_columns="$1"
	# Split rest_columns with comma into an array
	IFS=,
	set -- "$rest_columns"
	columns_array="$@"

	# Iterate over the columns array
	for column in $columns_array; do
		export "$column"
	done

}