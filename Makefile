SHELL := /bin/bash

# default variables
ARCH          ?= $(shell arch)
OSType        ?= $(shell uname)
GCC_OPT_LEVEL ?= O2
CXX           ?= g++
OBJDUMP       ?= objdump

# Apple's Darwin OS on M1 list Arm core differently
ifeq ($(ARCH), arm64)
  ARCH := aarch64
endif

ifeq ($(OSType), Darwin)
  CPU_INFO := $(shell sysctl -n machdep.cpu.brand_string)
else
  CPU_INFO := $(shell grep -m 1 "model name" /proc/cpuinfo)
endif

# extra security features (comment them out if not needed)
#disable_stack_nx_protection    = yes
#disable_stack_protection       = yes
#enable_aslr_protection         = yes
#enable_got_protection          = yes
#enable_stack_protection        = yes
#enable_vtable_verify           = yes
#enable_control_flow_protection = yes
#enable_stack_clash_protection  = yes
#enable_address_sanitizer       = yes

# define paths and objects
base = .

test-path = $(base)/test
LD_LIBRARY_PATH=$(test-path)

# define compiling flags
CXXFLAGS := -I./lib -$(GCC_OPT_LEVEL) -std=c++11 -Wall
LDFLAGS  :=
OBJDUMPFLAGS := -D -l -S

ifdef disable_stack_nx_protection
  CXXFLAGS += -z execstack
endif

ifdef disable_stack_protection
  CXXFLAGS += -fno-stack-protector
endif

ifdef enable_aslr_protection
  CXXFLAGS += -pie -fPIE
  LDFLAGS  += -Wl,-pie
endif

ifdef enable_got_protection
  LDFLAGS  += -Wl,-z,relro,-z,now
endif

ifdef enable_stack_protection
  CXXFLAGS += -Wstack-protector -fstack-protector-all
ifeq ($(ARCH), x86_64)
  CXXFLAGS += -mstack-protector-guard=tls
endif
endif

ifdef enable_vtable_verify
  CXXFLAGS += -fvtable-verify=std
endif

ifdef enable_control_flow_protection
ifeq ($(ARCH), x86_64)
  CXXFLAGS += -fcf-protection=full -mcet
endif
endif

ifdef enable_stack_clash_protection
  CXXFLAGS += -fstack-clash-protection
endif

ifdef enable_address_sanitizer
  CXXFLAGS += -fsanitize=address
ifeq ($(CXX),clang++)
  LDFLAGS  += -static-libsan
else ifeq ($(CXX),c++)
  LDFLAGS  += -static-libsan
else
  LDFLAGS  += -static-libasan
  CXXFLAGS += --param=asan-stack=1
endif
endif

# define cases
mss-path  = $(base)/mss
mss-cpps  = $(wildcard $(mss-path)/*.cpp)
mss-tests = $(addprefix $(test-path)/mss-, $(basename $(notdir $(mss-cpps))))
mss-cpps-prep = $(addsuffix .prep, $(mss-cpps))

mts-path  = $(base)/mts
mts-cpps  = $(wildcard $(mts-path)/*.cpp)
mts-tests = $(addprefix $(test-path)/mts-, $(basename $(notdir $(mts-cpps))))
mts-cpps-prep = $(addsuffix .prep, $(mts-cpps))

acc-path  = $(base)/acc
acc-cpps  = $(wildcard $(acc-path)/*.cpp)
acc-tests = $(addprefix $(test-path)/acc-, $(basename $(notdir $(acc-cpps))))
acc-cpps-prep = $(addsuffix .prep, $(acc-cpps))

cpi-path  = $(base)/cpi
cpi-cpps  = $(wildcard $(cpi-path)/*.cpp)
cpi-tests = $(addprefix $(test-path)/cpi-, $(basename $(notdir $(cpi-cpps))))
cpi-cpps-prep = $(addsuffix .prep, $(cpi-cpps))

cfi-path  = $(base)/cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/cfi-, $(basename $(notdir $(cfi-cpps))))
cfi-cpps-prep = $(addsuffix .prep, $(cfi-cpps))

sec-tests := $(mss-tests) $(mts-tests) $(acc-tests) $(cpi-tests) $(cfi-tests)
sec-tests-dump = $(addsuffix .dump, $(sec-tests))
sec-tests-prep := $(mss-cpps-prep) $(mts-cpps-prep) $(acc-cpps-prep) $(cpi-cpps-prep) $(cfi-cpps-prep)

headers := $(wildcard $(base)/lib/include/*.hpp) $(wildcard $(base)/lib/$(ARCH)/*.hpp)
extra_objects := $(base)/lib/common/global_var.o $(base)/lib/common/signal.o $(base)/lib/common/temp_file.o $(addprefix $(base)/lib/$(ARCH)/, assembly.o)

func-opcode-gen := ./script/get_x86_func_inst.sh
ifeq ($(ARCH), aarch64)
  func-opcode-gen := ./script/get_aarch64_func_inst.sh
else ifeq ($(ARCH), riscv64)
  func-opcode-gen := ./script/get_riscv64_func_inst.sh
endif

# compile targets

all: run-test
.PHONY: all

# json.hpp needs C++11, which might be problematic on some systems
run-test: $(base)/scheduler/run-test.cpp $(base)/lib/common/temp_file.cpp $(base)/lib/include/temp_file.hpp $(base)/scheduler/json.hpp $(test-path)/sys_info.txt
	$(CXX) -O2 --std=c++11 -I. -I./lib  $< $(base)/lib/common/temp_file.cpp -o $@

rubbish += run-test

$(test-path)/sys_info.txt:
	-mkdir -p $(test-path)
	echo "CPU: $(CPU_INFO)" > $(test-path)/sys_info.txt
	echo "System : " >> $(test-path)/sys_info.txt
	uname -srp >> $(test-path)/sys_info.txt
	echo "Compiler : " >> $(test-path)/sys_info.txt
	$(CXX) --version >> $(test-path)/sys_info.txt
	echo "LIBC : " >> $(test-path)/sys_info.txt
	$(OBJDUMP) --version >> $(test-path)/sys_info.txt
	echo "Flags : " >> $(test-path)/sys_info.txt
	echo "CXXFLAGS = " $(CXXFLAGS) >> $(test-path)/sys_info.txt
	echo "LDFLAGS = "$(LDFLAGS) >> $(test-path)/sys_info.txt

rubbish += $(test-path)/sys_info.txt

libcfi.so: $(base)/lib/common/cfi.cpp  $(base)/lib/include/cfi.hpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

rubbish += $(test-path)/libcfi.so

$(extra_objects): %.o : %.cpp $(headers)
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(extra_objects)

$(base)/lib/common/mss.o: %.o : %.cpp $(base)/lib/include/mss.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(base)/lib/common/mss.o

$(mss-tests): $(test-path)/mss-%:$(mss-path)/%.cpp $(extra_objects) $(headers) $(base)/lib/common/mss.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(base)/lib/common/mss.o -o $@ $(LDFLAGS)

rubbish += $(mss-tests)

$(mss-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(mts-tests): $(test-path)/mts-%:$(mts-path)/%.cpp $(extra_objects) $(base)/lib/common/mss.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(base)/lib/common/mss.o -o $@ $(LDFLAGS)

rubbish += $(mts-tests)

$(mts-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(acc-tests): $(test-path)/acc-%:$(acc-path)/%.cpp $(extra_objects)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) -o $@ $(LDFLAGS)

$(test-path)/acc-read-func-func-opcode.tmp: $(func-opcode-gen) $(test-path)/acc-read-func
	$^ helper 8 $@

$(test-path)/acc-read-func.gen: %.gen:% $(test-path)/acc-read-func-func-opcode.tmp
	cp $< $@

rubbish += $(acc-tests)

$(acc-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(cpi-tests): $(test-path)/cpi-%:$(cpi-path)/%.cpp $(extra_objects) libcfi.so $(headers)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) -L. -Wl,-rpath,. -o $@ -lcfi $(LDFLAGS)

rubbish += $(cpi-tests)

$(cpi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(cfi-tests): $(test-path)/cfi-%:$(cfi-path)/%.cpp $(extra_objects) libcfi.so $(headers)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) -L. -Wl,-rpath,. -o $@ -lcfi $(LDFLAGS)

rubbish += $(cfi-tests)

$(cfi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

rubbish += results.json results.json results.dat variables.json

dump: $(sec-tests-dump)
$(sec-tests-dump): %.dump:%
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

rubbish += $(sec-tests-dump)

prep: $(sec-tests-prep)

rubbish += $(sec-tests-prep)

clean:
	-rm $(rubbish) *.tmp $(test-path)/*.tmp $(test-path)/*.gen > /dev/null 2>&1

.PHONY: clean run dump prep

doc:
	cd doc; pdflatex specification

.PHONY: doc
