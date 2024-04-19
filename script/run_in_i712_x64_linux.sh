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
while [ "$i" -le 12 ]; do
	# Create a new directory name
	new_dir="cpu-sec-bench-$i"
	# Check if the directory already exists
	# if [ -d "$new_dir" ]; then
	# 	echo "Directory $new_dir already exists, skipping..."
	# 	i=$((i + 1))
	# 	continue
	# fi
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

prefix=x86-64-Lin-G
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
prefix=x86-64-Lin-L
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

ind=2
prefix=GCC-ASan-address
(
	echo "${prefix}"

	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=g++
	export enable_default_address_sanitizer="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=3
prefix=LLVM-ASan-address
(
	echo "${prefix}"

	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=clang++
	export enable_default_address_sanitizer="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=4
prefix=GCC-ASan-undefined
(
	echo "${prefix}"

	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=g++
	export enable_undefined_sanitizer="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=5
prefix=LLVM-ASan-undefined
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=clang++
	export enable_undefined_sanitizer="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=6
prefix=GCC-ASan-full
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=g++
	export enable_default_address_sanitizer="yes"
	export enable_undefined_sanitizer="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=7
prefix=LLVM-ASan-full
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=clang++
	export enable_default_address_sanitizer="yes"
	export enable_undefined_sanitizer="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=8
prefix=G12-GCC-stack
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=g++
	export enable_stack_protection="yes"
	export enable_stack_clash_protection="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=9
prefix=G12-GCC-CET
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=g++
	export enable_cet_shadow_stack="yes"
	export GLIBC_TUNABLES=glibc.cpu.hwcaps=SHSTK

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=10
prefix=G12-GCC-FULL
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=g++
	export enable_stack_protection="yes"
	export enable_stack_clash_protection="yes"
	export enable_cet_shadow_stack="yes"
	export GLIBC_TUNABLES=glibc.cpu.hwcaps=SHSTK
	export enable_vtable_verify="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=11
prefix=G12-LLVM-full
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	unset CXX
	export CXX=clang++
	export enable_stack_protection="yes"
	export enable_stack_clash_protection="yes"
	export enable_cet_shadow_stack="yes"
	export GLIBC_TUNABLES=glibc.cpu.hwcaps=SHSTK

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &
ind=12
prefix=G12-GCC-vtv
(
	echo "${prefix}"
	cd ..
	cd cpu-sec-bench-"${ind}" || (
		echo "no cpu-sec-bench-${ind}"
		exit
	)
	export enable_vtable_verify="yes"

	make cleanall >temp.log 2>&1
	make -e >>temp.log 2>&1
	./run-test exhausted-run >>temp.log 2>&1
	base_name=$(rename_log)
	mv temp.log "${prefix}"_"${base_name}".log
	mv "${base_name}".dat "${prefix}"_"${base_name}".dat
) &


wait

#-fsanitize-address-use-after-scope
#-fsanitize=undefined has many suboptions
