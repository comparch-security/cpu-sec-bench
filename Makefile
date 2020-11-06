SHELL := /bin/bash

# default variables
ARCH ?= $(shell arch)
GCC_OPT_LEVEL ?= O2

ifeq ($(ARCH), x86_64)
  # host compilation
  CXX ?= g++
  OBJDUMP ?= objdump
endif

ifeq ($(ARCH), riscv64)
  # cross-compilation
  CXX ?= $(RISCV)/bin/riscv64-unknown-linux-gnu-g++
  OBJDUMP ?= $(RISCV)/bin/riscv64-unknown-linux-gnu-objdump
endif


# define paths and objects
base = .

test-path = $(base)/test-$(ARCH)
LD_LIBRARY_PATH=$(test-path)

bof-path  = $(base)/bof
bof-cpps  = $(wildcard $(bof-path)/*.cpp)
bof-tests = $(addprefix $(test-path)/bof-, $(basename $(notdir $(bof-cpps))))
bof-cpps-prep = $(addsuffix .prep, $(bof-cpps))

ptt-path  = $(base)/ptt
ptt-cpps  = $(wildcard $(ptt-path)/*.cpp)
ptt-tests = $(addprefix $(test-path)/ptt-, $(basename $(notdir $(ptt-cpps))))
ptt-cpps-prep = $(addsuffix .prep, $(ptt-cpps))

cpi-path  = $(base)/cpi
cpi-cpps  = $(wildcard $(cpi-path)/*.cpp)
cpi-tests = $(addprefix $(test-path)/cpi-, $(basename $(notdir $(cpi-cpps))))
cpi-cpps-prep = $(addsuffix .prep, $(cpi-cpps))

cfi-path  = $(base)/cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/cfi-, $(basename $(notdir $(cfi-cpps))))
cfi-cpps-prep = $(addsuffix .prep, $(cfi-cpps))

sec-tests := $(bof-tests) $(ptt-tests) $(cpi-tests) $(cfi-tests)
sec-tests-dump = $(addsuffix .dump, $(sec-tests))
sec-tests-prep := $(bof-cpps-prep) $(ptt-cpps-prep) $(cpi-cpps-prep) $(cfi-cpps-prep)

headers := $(wildcard $(base)/lib/include/*.hpp) $(wildcard $(base)/lib/$(ARCH)/*.hpp)
extra_objects := $(base)/lib/common/signal.o $(addprefix $(base)/lib/$(ARCH)/, assembly.o)

CXXFLAGS := -I./lib -$(GCC_OPT_LEVEL) -Wall
# -fno-omit-frame-pointer
OBJDUMPFLAGS := -D -l -S
RUN_SCRIPT := ./run-test.py

# compile targets

all: $(test-path) $(sec-tests)

$(test-path):
	-mkdir -p $@

$(test-path)/libcfi.so: $(base)/lib/common/cfi.cpp  $(base)/lib/include/cfi.hpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

rubbish += $(test-path)/libcfi.so

$(extra_objects): %.o : %.cpp $(headers)
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(extra_objects)

$(base)/lib/common/bof.o: %.o : %.cpp $(base)/lib/include/bof.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(base)/lib/common/bof.o

$(bof-tests): $(test-path)/bof-%:$(bof-path)/%.cpp $(extra_objects) $(headers) $(base)/lib/common/bof.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(base)/lib/common/bof.o -o $@

rubbish += $(bof-tests)

$(bof-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(base)/lib/common/ptt.o: %.o : %.cpp $(base)/lib/include/ptt.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(base)/lib/common/ptt.o

$(ptt-tests): $(test-path)/ptt-%:$(ptt-path)/%.cpp $(extra_objects) $(headers) $(base)/lib/common/ptt.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(base)/lib/common/ptt.o -o $@

rubbish += $(ptt-tests)

$(ptt-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(cpi-tests): $(test-path)/cpi-%:$(cpi-path)/%.cpp $(extra_objects) $(test-path)/libcfi.so $(headers)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) -L$(test-path) -Wl,-rpath,. -o $@ -lcfi

rubbish += $(cpi-tests)

$(cpi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(cfi-tests): $(test-path)/cfi-%:$(cfi-path)/%.cpp $(extra_objects) $(test-path)/libcfi.so $(headers)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) -L$(test-path) -Wl,-rpath,. -o $@ -lcfi

rubbish += $(cfi-tests)

$(cfi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

run: $(sec-tests) $(test-path)/$(RUN_SCRIPT)
	cd $(test-path); $(RUN_SCRIPT)

dump: $(sec-tests-dump)
$(sec-tests-dump): %.dump:%
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

rubbish += $(sec-tests-dump)

prep: $(sec-tests-prep)

rubbish += $(sec-tests-prep)

clean:
	-rm $(rubbish) $(test-path)/results.json > /dev/null 2>&1

.PHONY: clean run dump prep

doc:
	cd doc; pdflatex specification

.PHONY: doc
