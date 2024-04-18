SHELL := /bin/bash

# check whether it is a windows env
OSType          ?= $(shell echo %OS%)

ifeq ($(OSType),Windows_NT)
	ARCH          ?=x86_64
else
	ARCH          ?=$(shell arch)
	OSType        :=$(shell uname)
	ifeq ($(ARCH),arm64)
		ARCH        :=aarch64
	endif
endif

# set variables
OPT_LEVEL       ?= O2
SIMPLE_FLAGS    := default
# extra security features (comment them out if not needed)

# common option in Windows and Linux

#enable_stack_nx_protection     = yes
#disable_stack_nx_protection    = yes
#enable_stack_protection        = yes
#disable_stack_protection       = yes
#enable_aslr_protection         = yes
#disable_aslr_protection        = yes
#enable_cet_shadow_stack        = yes

# common option in Linux
#enable_got_protection          = yes
#disable_got_protection         = yes
#enable_vtable_verify           = yes
#disable_vtable_verify          = yes
#enable_control_flow_protection = yes
#disable_control_flow_protection= yes
#enable_stack_clash_protection  = yes
#enable_address_sanitizer       = yes

# common option in Windows, msvc specific safety feature
#enable_extra_stack_protection  = yes
#enable_heap_integrity          = yes

# specific hardware secrutiy features

#enable_riscv64_cheri           = yes
#enable_aarch64_morello         = yes
#enable_aarch64_mte             = yes
#enable_aarch64_pa              = yes
#enable_aarch64_bti             = yes

# define paths and objects
ifeq ($(OSType),Windows_NT)

	# platform
	CPU_INFO      ?= $(shell echo %PROCESSOR_IDENTIFIER%)
	RUN_PREFIX    :=
	test-path     := test
	log-path      := trace-log

	# compiler
	CXX           := cl
	ASM           := ml64
	CLIBAPI       := visualcpp
	OBJDUMP       := dumpbin

	CXXFLAGS_BASE := /std:c11 /nologo /W3 /WX- /Oi /DNDEBUG /D_CONSOLE /D_UNICODE /DUNICODE                 \
									 /EHsc /MD /Gy /Gd /I./lib
	ifdef BUFFER_SIZE
		CXXFLAGS_BASE += /DBUFFER_SIZE=$(BUFFER_SIZE)
	endif
	ifdef BUFFER_KIND
		CXXFLAGS_BASE += /DBUFFER_KIND=$(BUFFER_KIND)
	endif
	ifdef REGION_KIND
		CXXFLAGS_BASE += /DREGION_KIND=$(REGION_KIND)
	endif
	ifneq ($(and $(BUFFER_VAL_UNDERFLOW),$(BUFFER_VAL_MID),$(BUFFER_VAL_OVERFLOW)),)
		CXXFLAGS_BASE += /DBUFFER_VAL_UNDERFLOW=$(BUFFER_VAL_UNDERFLOW) /DBUFFER_VAL_MID=$(BUFFER_VAL_MID) /DBUFFER_VAL_OVERFLOW=$(BUFFER_VAL_OVERFLOW)
	endif
	ifdef TRACE_RUN
		CXXFLAGS_BASE += /DTRACE_RUN=$(TRACE_RUN)
	endif
	SCHEDULER_CXXFLAGS  := /O2 $(CXXFLAGS_BASE) /I. /DRUN_PREFIX="\"$(RUN_PREFIX)\""
	OBJECT_CXXFLAGS     := /$(OPT_LEVEL) /Zi $(CXXFLAGS_BASE)
	CXXFLAGS      := /$(OPT_LEVEL) /Zi $(CXXFLAGS_BASE)
	ASMFLAGS      := /nologo /Zi /c
	# If there is a whitespace between windows msvc's output option and output file,
	# will raise error
	OUTPUT_EXE_OPTION := /Fe
	OUTPUT_LIB_OPTION := /c /Fo
	OUTPUT_DYN_OPTION := /LD /Fe
	MIDFILE_SUFFIX    := .obj
	DLL_SUFFIX        := .dll
	LDFLAGS           := /link /incremental:no /OPT:NOREF /OPT:NOICF
	LIB_LDFLAGS       := /link
	OBJDUMPFLAGS      := /DISASM
	DYNCFI_OPTION     := libcfi.lib
	func-opcode-gen   := .\script\get_x64_func_inst.bat
	dynlibcfi := $(addsuffix $(DLL_SUFFIX), libcfi)
	independent_assembly := lib/x86_64/visualcpp_indepassembly_func.obj

	# define compiling flags
	ifdef enable_stack_nx_protection
		CXXFLAGS += /NXCOMPAT
		LDFLAGS += /NXCOMPAT
		LIB_LDFLAGS += /NXCOMPAT
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-stack_nx
	endif

	ifdef disable_stack_nx_protection
		CXXFLAGS += /NXCOMPAT:NO
		LDFLAGS += /NXCOMPAT:NO
		LIB_LDFLAGS += /NXCOMPAT:NO
	endif

	ifdef enable_stack_protection
		CXXFLAGS += /GS
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-stack_p
	endif

	ifdef disable_stack_protection
		CXXFLAGS += /GS-
	endif

	ifdef enable_aslr_protection
		LDFLAGS  += /DYNAMICBASE /LARGEADDRESSAWARE /HIGHENTROPYVA
		LIB_LDFLAGS += /DYNAMICBASE /LARGEADDRESSAWARE /HIGHENTROPYVA
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-aslr
	endif

	ifdef disable_aslr_protection
		LDFLAGS  += /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO /HIGHENTROPYVA:NO
		LIB_LDFLAGS += /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO /HIGHENTROPYVA:NO
	endif

	ifdef enable_control_flow_protection
		CXXFLAGS += /guard:cf
		LDFLAGS  += /GUARD:CF
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-cf_p
	endif

	ifdef disable_control_flow_protection
		CXXFLAGS += /guard:cf-
		LDFLAGS  += /GUARD:NO
	endif

	ifdef enable_cet_shadow_stack
		LDFLAGS += /CETCOMPAT
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-cet_ss
	endif

	ifdef enable_heap_integrity
		CXXFLAGS += /sdl /GS
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-heap_int
	endif

	ifdef enable_extra_stack_protection
		CXXFLAGS += /RTCs
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-extra_stack_p
	endif

	ifdef enable_address_sanitizer
		CXXFLAGS += /fsanitize=address
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-asan
	endif

else

	# platform
	ifeq ($(OSType),Darwin)
		CPU_INFO    = $(shell sysctl -n machdep.cpu.brand_string)
	else
		CPU_INFO    = $(shell grep -m 1 "model name" /proc/cpuinfo)
	endif
	RUN_PREFIX    :=
	test-path     := test
	log-path      := trace-log

	#compiler
	ifeq ($(OSType),Darwin)
		CXX         := clang++
	else
		CXX         := g++
	endif
	ASM           := as
	CLIBAPI       := posix
	OBJDUMP       := objdump

	CXXFLAGS_BASE = ${CXXFLAGS} -I./lib -std=c++11 -Wall
	ifdef BUFFER_SIZE
		CXXFLAGS_BASE += -DBUFFER_SIZE=$(BUFFER_SIZE)
	endif
	ifdef BUFFER_KIND
		CXXFLAGS_BASE += -DBUFFER_KIND=$(BUFFER_KIND)
	endif
	ifdef REGION_KIND
		CXXFLAGS_BASE += -DREGION_KIND=$(REGION_KIND)
	endif
	ifneq ($(and $(BUFFER_VAL_UNDERFLOW),$(BUFFER_VAL_MID),$(BUFFER_VAL_OVERFLOW)),)
		CXXFLAGS_BASE += -DBUFFER_VAL_UNDERFLOW=$(BUFFER_VAL_UNDERFLOW) -DBUFFER_VAL_MID=$(BUFFER_VAL_MID) -DBUFFER_VAL_OVERFLOW=$(BUFFER_VAL_OVERFLOW)
	endif
	ifdef TRACE_RUN
		CXXFLAGS_BASE += -DTRACE_RUN=$(TRACE_RUN)
	endif
	SCHEDULER_CXXFLAGS  := -O2 $(CXXFLAGS_BASE) -I. -DRUN_PREFIX="\"$(RUN_PREFIX)\""
	OBJECT_CXXFLAGS     := -$(OPT_LEVEL) $(CXXFLAGS_BASE)
	CXXFLAGS      := $(CXXFLAGS_BASE)
	ASMFLAGS      :=
	OUTPUT_EXE_OPTION := -o 
	OUTPUT_LIB_OPTION := -c -o 
	OUTPUT_DYN_OPTION := -shared -fPIC -o 
	MIDFILE_SUFFIX    := .o
	DLL_SUFFIX        := .so
	LDFLAGS           :=
	LIB_LDFLAGS       :=
	OBJDUMPFLAGS      := -D -l -S
	DYNCFI_OPTION     := -Llib/common/ -Wl,-rpath,lib/common/ -lcfi
	func-opcode-gen   := ./script/get_x64_func_inst.sh
	ifeq ($(ARCH), aarch64)
		func-opcode-gen := ./script/get_aarch64_func_inst.sh
	else ifeq ($(ARCH), riscv64)
		func-opcode-gen := ./script/get_riscv64_func_inst.sh
	endif
	dynlibcfi := $(addsuffix $(DLL_SUFFIX), lib/common/libcfi)
	independent_assembly :=

	ifeq ($(CXX),$(filter $(CXX),clang++ c++))
		ifneq ($(OSType),Darwin)
			CXXFLAGS += -fuse-ld=lld
		endif
	endif

	# define compiling flags
	ifdef disable_stack_nx_protection
		CXXFLAGS += -z execstack
	endif

	ifdef disable_stack_nx_protection
		CXXFLAGS += -z noexecstack
	endif

	ifdef enable_stack_protection
		CXXFLAGS += -Wstack-protector -fstack-protector-all
	ifeq ($(ARCH),x86_64)
		CXXFLAGS += -mstack-protector-guard=guard
	endif
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-stack_p
	endif

	ifdef disable_stack_protection
		CXXFLAGS += -fno-stack-protector
	endif

	ifdef enable_aslr_protection
		CXXFLAGS += pie -fPIE
		LDFLAGS  += -Wl, pie
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-aslr
	endif

	ifdef disable_aslr_protection
		CXXFLAGS += -no-pie
		LDFLAGS  += -Wl,-no-pie
	endif

	ifdef enable_got_protection
		LDFLAGS  += -Wl,-z,relro,-z,now
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-got
	endif

	ifdef disable_got_protection
		LDFLAGS  += -Wl,-z, norelro,-z,lazy
	endif

	ifdef enable_vtable_verify
		CXXFLAGS += -fvtable-verify=std
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-vtable_v
	endif

	ifdef disable_vtable_verify
		CXXFLAGS += -fvtable-verify=none
	endif

	ifdef enable_control_flow_protection
	ifeq ($(ARCH),x86_64)
		CXXFLAGS += -fcf-protection=full
	endif
	SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-cf_p
	endif

	ifdef enable_cet_shadow_stack
		CXXFLAGS += -fcf-protection=return
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-cet_ss
	endif

	ifdef disable_control_flow_protection
	ifeq ($(ARCH),x86_64)
		CXXFLAGS += -fcf-protection=none
	endif
	endif

	ifdef enable_stack_clash_protection
		CXXFLAGS += -fstack-clash-protection
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-stack_p
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
		SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-asan
	endif
endif

ifdef enable_riscv64_cheri
	ARCH :=cheri_riscv64
	CXXFLAGS += -mno-relax -march=rv64gcxcheri -mabi=l64pc128d -cheri-bounds=very-aggressive
	SCHEDULER_CXXFLAGS += -mno-relax
	OBJECT_CXXFLAGS += -mno-relax -march=rv64gcxcheri -mabi=l64pc128d -cheri-bounds=very-aggressive
	SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-cheri
endif

ifdef enable_aarch64_morello
	ARCH        :=aarch64
	CXXFLAGS += -march=morello -mabi=purecap -cheri-bounds=very-aggressive
	OBJECT_CXXFLAGS += -march=morello -mabi=purecap -cheri-bounds=very-aggressive
	SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-morello
endif

ifdef enable_aarch64_mte
	CXXFLAGS := $(CXXFLAGS) -march=armv8.5-a+memtag -fsanitize=hwaddress
	OBJECT_CXXFLAGS += -march=armv8.5-a+memtag -fsanitize=hwaddress
	SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-mte
endif

ifdef enable_aarch64_pa
	CXXFLAGS := $(CXXFLAGS) -march=armv8.3-a+pauth -mbranch-protection=pac-ret
	OBJECT_CXXFLAGS += -march=armv8.3-a+pauth -mbranch-protection=pac-ret
	SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-pa
endif

ifdef enable_aarch64_bti
	CXXFLAGS := $(CXXFLAGS) -march=armv8.5-a -mbranch-protection=bti
	OBJECT_CXXFLAGS += -march=armv8.5-a -mbranch-protection=bti
	SIMPLE_FLAGS :=$(SIMPLE_FLAGS)-bti
endif

# define cases
mss-path  = mss
mss-cpps  = $(wildcard $(mss-path)/*.cpp)
mss-obj   = $(addsuffix $(MIDFILE_SUFFIX),$(addprefix $(test-path)/mss-, $(basename $(notdir $(mss-cpps)))))
mss-tests = $(addprefix $(test-path)/mss-, $(basename $(notdir $(mss-cpps))))
mss-cpps-prep = $(addsuffix .prep, $(mss-cpps))

mts-path  = mts
mts-cpps  = $(wildcard $(mts-path)/*.cpp)
mts-obj   = $(addsuffix $(MIDFILE_SUFFIX), $(addprefix $(test-path)/mts-, $(basename $(notdir $(mts-cpps)))))
mts-tests = $(addprefix $(test-path)/mts-, $(basename $(notdir $(mts-cpps))))
mts-cpps-prep = $(addsuffix .prep, $(mts-cpps))

acc-path  = acc
acc-cpps  = $(wildcard $(acc-path)/*.cpp)
acc-obj   = $(addsuffix $(MIDFILE_SUFFIX), $(addprefix $(test-path)/acc-, $(basename $(notdir $(acc-cpps)))))
acc-tests = $(addprefix $(test-path)/acc-, $(basename $(notdir $(acc-cpps))))
acc-cpps-prep = $(addsuffix .prep, $(acc-cpps))

cpi-path  = cpi
cpi-cpps  = $(wildcard $(cpi-path)/*.cpp)
cpi-obj   = $(addsuffix $(MIDFILE_SUFFIX), $(addprefix $(test-path)/cpi-, $(basename $(notdir $(cpi-cpps)))))
cpi-tests = $(addprefix $(test-path)/cpi-, $(basename $(notdir $(cpi-cpps))))
cpi-cpps-prep = $(addsuffix .prep, $(cpi-cpps))

cfi-path  = cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-obj   = $(addsuffix $(MIDFILE_SUFFIX), $(addprefix $(test-path)/cfi-, $(basename $(notdir $(cfi-cpps)))))
cfi-tests = $(addprefix $(test-path)/cfi-, $(basename $(notdir $(cfi-cpps))))
cfi-cpps-prep = $(addsuffix .prep, $(cfi-cpps))

sec-tests := $(mss-tests) $(mts-tests) $(acc-tests) $(cpi-tests) $(cfi-tests)
sec-tests-dump = $(addsuffix .dump, $(sec-tests))
sec-tests-prep := $(mss-cpps-prep) $(mts-cpps-prep) $(acc-cpps-prep) $(cpi-cpps-prep) $(cfi-cpps-prep)

headers := $(wildcard lib/include/*.hpp) $(wildcard lib/$(ARCH)/*.hpp) $(wildcard lib/$(CLIBAPI)/*.hpp)

extra_objects := lib/common/global_var lib/common/temp_file $(addprefix lib/$(ARCH)/, assembly) $(addprefix lib/$(CLIBAPI)/, signal)
extra_objects := $(addsuffix $(MIDFILE_SUFFIX), $(extra_objects))

libmss := $(addsuffix $(MIDFILE_SUFFIX), lib/common/mss)

all: run-test
.PHONY: all

run-test: scheduler/run-test.cpp lib/common/temp_file.cpp lib/include/temp_file.hpp scheduler/json.hpp $(test-path)/sys_info.txt
	$(CXX) $(SCHEDULER_CXXFLAGS) $< lib/common/temp_file.cpp  $(OUTPUT_EXE_OPTION)$@

ifeq ($(OSType),Windows_NT)

$(test-path)/sys_info.txt:
	-mkdir $(test-path)
	-mkdir $(log-path)
	echo "OVERVIEW:"$(ARCH)"-"$(CXX)"-"$(SIMPLE_FLAGS) > $(test-path)/sys_info.txt
	echo "CPU: " >> $(test-path)/sys_info.txt
	systeminfo | findstr /C:"Intel" >> $(test-path)/sys_info.txt
	echo "System:" >> $(test-path)/sys_info.txt
	systeminfo | findstr /C:"Windows" >> $(test-path)/sys_info.txt
	echo "Compiler : " >> $(test-path)/sys_info.txt
	echo "VSCMD_VER=" %VSCMD_VER% " UCRTVersion=" %UCRTVersion% " VCToolsVersion=" %VCToolsVersion% >> $(test-path)/sys_info.txt
	Reg Query "HKLM\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" >> $(test-path)/sys_info.txt
	echo "Flags : " >> $(test-path)/sys_info.txt
	echo "OBJECT_CXXFLAGS = " $(OBJECT_CXXFLAGS) >> $(test-path)/sys_info.txt
	echo "CXXFLAGS = " $(CXXFLAGS) >> $(test-path)/sys_info.txt
	echo "LDFLAGS = " $(LDFLAGS) >> $(test-path)/sys_info.txt

rubbish += run-test.exe

else

$(test-path)/sys_info.txt:
	-mkdir -p $(test-path)
	-mkdir -p $(log-path)
	echo "OVERVIEW: $(ARCH)-$(CXX)-$(SIMPLE_FLAGS)" > $(test-path)/sys_info.txt
	echo "CPU:" >> $(test-path)/sys_info.txt
	echo "$(CPU_INFO)" >> $(test-path)/sys_info.txt
	echo "System: " >> $(test-path)/sys_info.txt
	uname -srp >> $(test-path)/sys_info.txt
	echo "Compiler : " >> $(test-path)/sys_info.txt
	$(CXX) --version >> $(test-path)/sys_info.txt
	echo "LIBC : " >> $(test-path)/sys_info.txt
	$(OBJDUMP) --version >> $(test-path)/sys_info.txt
	echo "Flags : " >> $(test-path)/sys_info.txt
	echo "OBJECT_CXXFLAGS = " $(OBJECT_CXXFLAGS) >> $(test-path)/sys_info.txt
	echo "CXXFLAGS = " $(CXXFLAGS) >> $(test-path)/sys_info.txt
	echo "LDFLAGS = " $(LDFLAGS) >> $(test-path)/sys_info.txt

rubbish += run-test

endif


$(dynlibcfi): lib/common/cfi.cpp  lib/include/cfi.hpp
	$(CXX) $(OBJECT_CXXFLAGS) $< $(OUTPUT_DYN_OPTION)$@ $(LIB_LDFLAGS)

rubbish += $(dynlibcfi)

cfi_base := $(basename $(dynlibcfi))
rubbish += $(cfi_base).so $(cfi_base).dll $(cfi_base).pdb $(cfi_base).obj $(cfi_base).lib $(cfi_base).ilk $(cfi_base).exp lib/common/cfi.obj

$(extra_objects): %$(MIDFILE_SUFFIX) : %.cpp $(headers)
	$(CXX) $(OBJECT_CXXFLAGS) $< $(OUTPUT_LIB_OPTION)$@ $(LIB_LDFLAGS)

rubbish += $(extra_objects)

$(independent_assembly): %$(MIDFILE_SUFFIX) : %.asm
	$(ASM) $(OUTPUT_LIB_OPTION)$@ $(ASMFLAGS) $^ $(LIB_LDFLAGS)

rubbish += $(independent_assembly)

$(libmss): %$(MIDFILE_SUFFIX) : %.cpp lib/include/mss.hpp
	$(CXX) $(OBJECT_CXXFLAGS) -c $< $(OUTPUT_LIB_OPTION)$@ $(LIB_LDFLAGS)

rubbish += $(libmss)

$(mss-obj): $(test-path)/mss-%$(MIDFILE_SUFFIX):$(mss-path)/%.cpp
	$(CXX) $(OBJECT_CXXFLAGS) $< $(OUTPUT_LIB_OPTION)$@ $(LIB_LDFLAGS)

$(mss-tests): %:%$(MIDFILE_SUFFIX) $(extra_objects) $(libmss) $(independent_assembly)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(independent_assembly) $(libmss) $(OUTPUT_EXE_OPTION)$@ $(LDFLAGS)

$(mss-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(mts-obj): $(test-path)/mts-%$(MIDFILE_SUFFIX):$(mts-path)/%.cpp
	$(CXX) $(OBJECT_CXXFLAGS) $< $(OUTPUT_LIB_OPTION)$@ $(LIB_LDFLAGS)

$(mts-tests): %:%$(MIDFILE_SUFFIX) $(extra_objects) $(libmss) $(independent_assembly)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(independent_assembly) $(libmss) $(OUTPUT_EXE_OPTION)$@ $(LDFLAGS)

$(mts-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(acc-obj): $(test-path)/acc-%$(MIDFILE_SUFFIX):$(acc-path)/%.cpp
	$(CXX) $(OBJECT_CXXFLAGS) $< $(OUTPUT_LIB_OPTION)$@ $(LIB_LDFLAGS)

$(acc-tests): %:%$(MIDFILE_SUFFIX) $(extra_objects) $(independent_assembly)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(independent_assembly) $(OUTPUT_EXE_OPTION)$@ $(LDFLAGS)

$(test-path)/acc-read-func-func-opcode.tmp: $(func-opcode-gen) $(test-path)/acc-read-func
ifeq ($(OSType),Windows_NT)
	$< $(test-path)\acc-read-func.exe helper 8 $(test-path)\acc-read-func-func-opcode.tmp
else
	$^ helper 8 $@
endif

$(test-path)/acc-read-func.gen: %.gen:% $(test-path)/acc-read-func-func-opcode.tmp
ifeq ($(OSType), Windows_NT)
	copy /Y $(test-path)\acc-read-func.exe $(test-path)\acc-read-func.gen
else
	cp $< $@
endif

$(acc-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(cpi-obj): $(test-path)/cpi-%$(MIDFILE_SUFFIX):$(cpi-path)/%.cpp
	$(CXX) $(OBJECT_CXXFLAGS) $< $(OUTPUT_LIB_OPTION)$@ $(LIB_LDFLAGS)

$(cpi-tests): %:%$(MIDFILE_SUFFIX) $(extra_objects) $(dynlibcfi) $(independent_assembly)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(independent_assembly) $(DYNCFI_OPTION) $(OUTPUT_EXE_OPTION)$@  $(LDFLAGS)

$(cpi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(cfi-obj): $(test-path)/cfi-%$(MIDFILE_SUFFIX):$(cfi-path)/%.cpp
	$(CXX) $(OBJECT_CXXFLAGS) $< $(OUTPUT_LIB_OPTION)$@ $(LIB_LDFLAGS)

$(cfi-tests): %:%$(MIDFILE_SUFFIX) $(extra_objects) $(dynlibcfi) $(independent_assembly)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(independent_assembly) $(DYNCFI_OPTION) $(OUTPUT_EXE_OPTION)$@ $(LDFLAGS)

$(cfi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

dump: $(sec-tests-dump)
$(sec-tests-dump): %.dump:%
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

prep: $(sec-tests-prep)

res_and_rubbish:= $(rubbish) results.json results.dat variables.json *.dat *.log

ifeq ($(OSType),Windows_NT)

# cmd can not identify "/", so use powershell remove-item
# powershell remove-item separates diff items by comma not whitespace

rubbish:= $(subst /,\,$(rubbish))

clean:
	-del /Q $(rubbish) $(test-path) $(log-path) *.tmp *.ilk *.pdb *.obj *.exe *.dump *.dll *.lib *.exp

cleanall:
	-del /Q $(res_and_rubbish) $(test-path) $(log-path) *.tmp *.ilk *.pdb *.obj *.exe *.dump *.dll *.lib *.exp

else

clean:
	-rm -rf $(rubbish) $(test-path) $(log-path) *.tmp > /dev/null 2>&1

cleanall:
	-rm -rf $(res_and_rubbish) $(test-path) $(log-path) *.tmp > /dev/null 2>&1

endif

.PHONY: clean run dump prep

doc:
	cd doc; pdflatex specification

.PHONY: doc
