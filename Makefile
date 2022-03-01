SHELL := /bin/bash

# default variables
ARCH          ?= $(shell arch)
GCC_OPT_LEVEL ?= O2
CXX           ?= g++
OBJDUMP       ?= objdump

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
ifeq ($(ARCH), "x86_64")
CXXFLAGS += -mstack-protector-guard=tls
endif
endif

ifdef enable_vtable_verify
CXXFLAGS += -fvtable-verify=std
endif

ifdef enable_control_flow_protection
ifeq ($(ARCH), "x86_64")
CXXFLAGS += -fcf-protection=full -mcet
endif
endif

ifdef enable_stack_clash_protection
CXXFLAGS += -fstack-clash-protection
endif

ifdef enable_address_sanitizer
CXXFLAGS += -fsanitize=address --param=asan-stack=1
LDFLAGS  += -static-libasan
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
extra_objects := $(base)/lib/common/signal.o $(addprefix $(base)/lib/$(ARCH)/, assembly.o)

# compile targets

all: run-test

run-test: $(base)/scheduler/run-test.cpp $(base)/scheduler/json.hpp | $(test-path)
	$(CXX) -O1 -g -I. $< -o $@

rubbish += run-test

$(test-path):
	-mkdir -p $@

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

rubbish += $(test-path)/results.json $(test-path)/results.dat

dump: $(sec-tests-dump)
$(sec-tests-dump): %.dump:%
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

rubbish += $(sec-tests-dump)

prep: $(sec-tests-prep)

rubbish += $(sec-tests-prep)

clean:
	-rm $(rubbish)  > /dev/null 2>&1

.PHONY: clean run dump prep

doc:
	cd doc; pdflatex specification

.PHONY: doc
