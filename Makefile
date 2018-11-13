SHELL := /bin/bash

# default variables
TEST_ARCH     ?= ARCH_X86_64
GCC_OPT_LEVEL ?= O2

# define paths and objects
base = .

test-path = $(base)/test
LD_LIBRARY_PATH=$(test-path)

cfi-path  = $(base)/cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/cfi-, $(basename $(notdir $(cfi-cpps))))
cfi-cpps-prep = $(addsuffix .prep, $(cfi-cpps))

bof-path  = $(base)/bof
bof-cpps  = $(wildcard $(bof-path)/*.cpp)
bof-tests = $(addprefix $(test-path)/bof-, $(basename $(notdir $(bof-cpps))))
bof-cpps-prep = $(addsuffix .prep, $(bof-cpps))

sec-tests := $(cfi-tests) $(bof-tests)
sec-tests-dump = $(addsuffix .dump, $(sec-tests))
sec-tests-prep := $(cfi-cpps-prep) $(bof-cpps-prep)

headers := $(wildcard $(base)/lib/include/*.hpp)

# conditional variables
ifeq ($(TEST_ARCH), ARCH_X86_64)
  headers += $(wildcard $(base)/lib/x86_64/*.hpp)
  arch_targets = $(addprefix $(base)/lib/x86_64/, assembly.o)
endif

CXX := g++
CXXFLAGS := -I./lib -D$(TEST_ARCH) -$(GCC_OPT_LEVEL) -Wall
OBJDUMP := objdump
OBJDUMPFLAGS := -D -l -S
RUN_SCRIPT := $(base)/script/run-test.py

# compile targets

all: $(test-path) $(sec-tests)

$(test-path):
	-mkdir -p $@

$(test-path)/libcfi.so: $(base)/lib/common/cfi.cpp  $(base)/lib/include/cfi.hpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

rubbish += $(test-path)/libcfi.so

$(arch_targets): %.o : %.cpp $(headers)
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(arch_targets)

$(cfi-tests): $(test-path)/cfi-%:$(cfi-path)/%.cpp $(arch_targets) $(test-path)/libcfi.so $(headers)
	$(CXX) $(CXXFLAGS) $< $(arch_targets) -L$(test-path) -Wl,-rpath,$(test-path) -o $@ -lcfi

rubbish += $(cfi-tests)

$(cfi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(bof-tests): $(test-path)/bof-%:$(bof-path)/%.cpp $(arch_targets) $(headers)
	$(CXX) $(CXXFLAGS) $< $(arch_targets) -o $@

rubbish += $(bof-tests)

$(bof-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

run: $(sec-tests)
	@echo ===============================
	@echo Run all tests:
	@echo -------------------------------
	@for t in $^; do $(RUN_SCRIPT) $$t; done

dump: $(sec-tests-dump)
$(sec-tests-dump): %.dump:%
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

rubbish += $(sec-tests-dump)

prep: $(sec-tests-prep)

rubbish += $(sec-tests-prep)

clean:
	-rm $(rubbish) > /dev/null 2>&1

.PHONY: clean run dump prep

doc:
	cd doc; pdflatex specification

.PHONY: doc

