SHELL := /bin/bash

# check whether it is a windows env
OSType          ?= $(shell echo %OS%)

ifeq ($(OSType),Windows_NT)
  ARCH          ?= x86_64
else
  ARCH          ?= $(shell arch)
  OSType        := $(shell uname)
  ifeq ($(ARCH),arm64)
    ARCH        := aarch64
  endif
endif

# set variables
OPT_LEVEL       ?= O2

ifeq ($(OSType),Windows_NT)

  # platform
  CPU_INFO      ?= $(shell echo %PROCESSOR_IDENTIFIER%)
  RUN_PREFIX    :=
  test-path     := test

  # compiler
  CXX           := cl
  CLIBAPI       := visualcpp
  OBJDUMP       := dumpbin

  CXXFLAGS_BASE := /nologo /W3 /WX- /sdl /Oi /DNDEBUG /D_CONSOLE /D_UNICODE /DUNICODE \
                   /Gm- /EHsc /MD /GS /Gy /Gd /I./lib
  CXXFLAGS_RUN  := /O2 $(CXXFLAGS_BASE) /I. /DRUN_PREFIX="\"$(RUN_PREFIX)\""
  CXXFLAGS      := /$(OPT_LEVEL) $(CXXFLAGS_BASE)
  LDFLAGS       :=
  OBJDUMPFLAGS  :=

else

  # platform
  ifeq ($(OSType),Darwin)
    CPU_INFO    = $(shell sysctl -n machdep.cpu.brand_string)
  else
    CPU_INFO    = $(shell grep -m 1 "model name" /proc/cpuinfo)
  endif
  RUN_PREFIX    :=
  test-path     := test

  #compiler
  ifeq ($(OSType),Darwin)
    CXX         := clang++
  else
    CXX         := g++
  endif
  CLIBAPI       := posix
  OBJDUMP       := objdump

  CXXFLAGS_BASE := -I./lib -std=c++11 -Wall
  CXXFLAGS_RUN  := -O2 $(CXXFLAGS_BASE) -I. -DRUN_PREFIX="\"$(RUN_PREFIX)\""
  CXXFLAGS      := -$(OPT_LEVEL) $(CXXFLAGS_BASE) 
  LDFLAGS       :=
  LD_LIBRARY_PATH := $(test-path)
  OBJDUMPFLAGS  := -D -l -S

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

# define compiling flags

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
ifeq ($(ARCH),x86_64)
  CXXFLAGS += -mstack-protector-guard=tls
endif
endif

ifdef enable_vtable_verify
  CXXFLAGS += -fvtable-verify=std
endif

ifdef enable_control_flow_protection
ifeq ($(ARCH),x86_64)
  CXXFLAGS += -fcf-protection=full -mcet
endif
endif

ifdef enable_stack_clash_protection
  CXXFLAGS += -fstack-clash-protection
endif

ifdef enable_address_sanitizer
  CXXFLAGS += -fsanitize=address
  RUN_PREFIX += ASAN_OPTIONS=detect_leaks=0
ifeq ($(CXX),$(filter $(CXX),clang++ c++))
  LDFLAGS  += -static-libsan
else
  LDFLAGS  += -static-libasan
  CXXFLAGS += --param=asan-stack=1
endif
endif

# define cases
mss-path  = mss
mss-cpps  = $(wildcard $(mss-path)/*.cpp)
mss-tests = $(addprefix $(test-path)/mss-, $(basename $(notdir $(mss-cpps))))
mss-cpps-prep = $(addsuffix .prep, $(mss-cpps))

mts-path  = mts
mts-cpps  = $(wildcard $(mts-path)/*.cpp)
mts-tests = $(addprefix $(test-path)/mts-, $(basename $(notdir $(mts-cpps))))
mts-cpps-prep = $(addsuffix .prep, $(mts-cpps))

acc-path  = acc
acc-cpps  = $(wildcard $(acc-path)/*.cpp)
acc-tests = $(addprefix $(test-path)/acc-, $(basename $(notdir $(acc-cpps))))
acc-cpps-prep = $(addsuffix .prep, $(acc-cpps))

cpi-path  = cpi
cpi-cpps  = $(wildcard $(cpi-path)/*.cpp)
cpi-tests = $(addprefix $(test-path)/cpi-, $(basename $(notdir $(cpi-cpps))))
cpi-cpps-prep = $(addsuffix .prep, $(cpi-cpps))

cfi-path  = cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/cfi-, $(basename $(notdir $(cfi-cpps))))
cfi-cpps-prep = $(addsuffix .prep, $(cfi-cpps))

sec-tests := $(mss-tests) $(mts-tests) $(acc-tests) $(cpi-tests) $(cfi-tests)
sec-tests-dump = $(addsuffix .dump, $(sec-tests))
sec-tests-prep := $(mss-cpps-prep) $(mts-cpps-prep) $(acc-cpps-prep) $(cpi-cpps-prep) $(cfi-cpps-prep)

headers := $(wildcard lib/include/*.hpp) $(wildcard lib/$(ARCH)/*.hpp) $(wildcard lib/$(CLIBAPI)/*.hpp)
extra_objects := lib/common/global_var.o lib/common/signal.o lib/common/temp_file.o $(addprefix lib/$(ARCH)/, assembly.o)

func-opcode-gen := ./script/get_x86_func_inst.sh
ifeq ($(ARCH), aarch64)
  func-opcode-gen := ./script/get_aarch64_func_inst.sh
else ifeq ($(ARCH), riscv64)
  func-opcode-gen := ./script/get_riscv64_func_inst.sh
endif

all: run-test
.PHONY: all

run-test: scheduler/run-test.cpp lib/common/temp_file.cpp lib/include/temp_file.hpp scheduler/json.hpp $(test-path)/sys_info.txt
	$(CXX) $(CXXFLAGS_RUN) $< lib/common/temp_file.cpp -o $@

ifeq ($(OSType),Windows_NT)

$(test-path)/sys_info.txt:
	-mkdir $(test-path)
	echo "CPU: & System : " > $(test-path)/sys_info.txt
	systeminfo | findstr /C:"Windows" /C:"Intel" >> $(test-path)/sys_info.txt
	echo "Compiler : " >> $(test-path)/sys_info.txt
	echo "VSCMD_VER=" %VSCMD_VER% " UCRTVersion=" %UCRTVersion% " VCToolsVersion=" %VCToolsVersion% >> $(test-path)/sys_info.txt
	echo "Flags : " >> $(test-path)/sys_info.txt
	echo "CXXFLAGS = " $(CXXFLAGS) >> $(test-path)/sys_info.txt
	echo "LDFLAGS = " $(LDFLAGS) >> $(test-path)/sys_info.txt

rubbish += run-test.* temp_file.obj $(test-path)/sys_info.txt

else

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
	echo "LDFLAGS = " $(LDFLAGS) >> $(test-path)/sys_info.txt

rubbish += run-test $(test-path)/sys_info.txt

endif


libcfi.so: lib/common/cfi.cpp  lib/include/cfi.hpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

rubbish += libcfi.so

$(extra_objects): %.o : %.cpp $(headers)
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(extra_objects)

lib/common/mss.o: %.o : %.cpp lib/include/mss.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += lib/common/mss.o

$(mss-tests): $(test-path)/mss-%:$(mss-path)/%.cpp $(extra_objects) lib/common/mss.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) lib/common/mss.o -o $@ $(LDFLAGS)

rubbish += $(mss-tests)

$(mss-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(mts-tests): $(test-path)/mts-%:$(mts-path)/%.cpp $(extra_objects) lib/common/mss.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) lib/common/mss.o -o $@ $(LDFLAGS)

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

$(cpi-tests): $(test-path)/cpi-%:$(cpi-path)/%.cpp $(extra_objects) libcfi.so
	$(CXX) $(CXXFLAGS) $< $(extra_objects) -L. -Wl,-rpath,. -o $@ -lcfi $(LDFLAGS)

rubbish += $(cpi-tests)

$(cpi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(cfi-tests): $(test-path)/cfi-%:$(cfi-path)/%.cpp $(extra_objects) libcfi.so
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

ifeq ($(OSType),Windows_NT)

# cmd can not identify "/", so use powershell remove-item
# powershell remove-item separates diff items by comma not whitespace

rubbish:= $(subst /,\,$(rubbish))
clean:
	-del /Q $(rubbish),$(test-path) *.tmp *.ilk *.pdb *.obj *.exe

else

clean:
	-rm $(rubbish) *.tmp $(test-path)/*.tmp $(test-path)/*.gen > /dev/null 2>&1

endif

.PHONY: clean run dump prep

doc:
	cd doc; pdflatex specification

.PHONY: doc
