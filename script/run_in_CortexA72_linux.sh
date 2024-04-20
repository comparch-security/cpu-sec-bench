#! /bin/sh
set +x
# echo $PWD

# keep log name same with result name
rename_log() {
	if log_name=$(ls -t *.dat); then
		log_name=$(echo "${log_name}" | head -n 1)
		base_name="${log_name%.*}"
		echo "$base_name"
	else
		exit 1
	fi
}

cd ../
i=1
while [ "$i" -le 1 ]; do
	# Create a new directory name
	new_dir="cpu-sec-bench-$i"
	# Use cp to copy the current directory into the new directory
	rm -rf "$new_dir"
	cp -r "cpu-sec-bench" "$new_dir"
	# Increment i by 1 in each loop
	i=$((i + 1))
done
cd - || (
	echo "cd - failed"
	exit
)

prefix=Armv8.0-Lin-G
(
	echo "${prefix}"

	cd ..
	cd cpu-sec-bench || (
		echo "no cpu-sec-bench"
		exit
	)
	unset CXX
	export CXX=g++
	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	echo "$base_name"
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &

ind=1
prefix=Armv8.0-Lin-L
(
	echo "${prefix}"

	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=clang++

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &

wait