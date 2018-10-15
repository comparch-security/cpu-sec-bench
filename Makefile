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

sec-tests := $(cfi-tests)
sec-tests-dump = $(addsuffix .dump, $(sec-tests))
sec-tests-prep := $(cfi-cpps-prep)

headers := $(wildcard $(base)/lib/include/*.hpp)

# conditional variables
ifeq ($(TEST_ARCH), ARCH_X86_64)
  ifeq ($(GCC_OPT_LEVEL), g)
    STACK_STRUCT := STACK_FP_RET
  else
    STACK_STRUCT := STACK_RET
  endif
  headers += $(wildcard $(base)/lib/x86_64/*.hpp)
endif

CXX := g++
CXXFLAGS := -I./lib -D$(TEST_ARCH) -D$(STACK_STRUCT) -$(GCC_OPT_LEVEL) -Wall
OBJDUMP := objdump
OBJDUMPFLAGS := -D -l -S
RUN_SCRIPT := $(base)/script/run-test.py

# compile targets

all: $(test-path) $(sec-tests)

$(test-path):
	-mkdir -p $@

$(test-path)/libcfi.so: $(base)/lib/common/cfi.cpp  $(base)/lib/include/cfi.hpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

$(cfi-tests): $(test-path)/cfi-%:$(cfi-path)/%.cpp $(test-path)/libcfi.so $(headers)
	$(CXX) $(CXXFLAGS) $< -L$(test-path) -lcfi -Wl,-rpath,$(test-path) -o $@

$(cfi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

run: $(sec-tests)
	@echo ===============================
	@echo Run all tests:
	@echo -------------------------------
	@for t in $^; do $(RUN_SCRIPT) $$t; done

dump: $(sec-tests-dump)
$(sec-tests-dump): %.dump:%
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

prep: $(sec-tests-prep)

clean:
	-rm $(sec-tests) > /dev/null 2>&1
	-rm $(sec-tests-dump) > /dev/null 2>&1
	-rm $(sec-tests-prep) > /dev/null 2>&1

.PHONY: clean run dump prep

doc:
	cd doc; pdflatex specification

.PHONY: doc

