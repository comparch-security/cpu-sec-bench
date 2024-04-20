#!/bin/bash

rename_log() {
	if log_name=$(ls -t *.dat); then
		log_name=$(echo "${log_name}" | head -n 1)
		base_name="${log_name%.*}"
		echo "$base_name"
	else
		exit 1
	fi
}

if [ $# -eq 0 ]; then
    echo "run multiple cheri virtual envs, and copy cpu-sec-bench to them"


elif [ $# -eq 1 ]; then

while [ "$i" -le 11 ]; do
	# Create a new directory name
	new_dir="cpu-sec-bench-$i"

	rm -rf "$new_dir"
	cp -r "cpu-sec-bench" "$new_dir"
	# Increment i by 1 in each loop
	i=$((i + 1))
done

prefix=CHERI-default 
(
	echo "${prefix}"

	cd ..
	cd cpu-sec-bench || (
		echo "no cpu-sec-bench"
		exit
	)
	unset CXX
	export CXX=clang++
    export enable_riscv64_cheri_default="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	echo "$base_name"
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &

prefix=CHERI-strong
(
	echo "${prefix}"

	cd ..
	cd cpu-sec-bench || (
		echo "no cpu-sec-bench"
		exit
	)
	unset CXX
	export CXX=clang++
    export enable_riscv64_cheri_everywhere_unsafe="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	echo "$base_name"
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &

wait

fi

# Define variables
USER="user"
HOST="host"
SOURCE_DIRECTORY="/path/to/source_directory"
DESTINATION_DIRECTORY="/path/to/destination_directory"
BASH_SCRIPT="bash_script.sh"

# Copy directory to remote server
scp -r $SOURCE_DIRECTORY $USER@$HOST:$DESTINATION_DIRECTORY

# Execute bash script in the copied directory on the remote server
ssh $USER@$HOST "bash $DESTINATION_DIRECTORY/$BASH_SCRIPT 1"
