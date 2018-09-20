SHELL := /bin/bash

# default variables
TEST_ARCH     ?= ARCH_X86_64
GCC_OPT_LEVEL ?= O2

# define paths and objects
base = .

test-path = $(base)/test

cfi-path  = $(base)/cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/, $(basename $(notdir $(cfi-cpps))))
cfi-cpps-prep = $(addsuffix .prep, $(cfi-cpps))

sec-tests := $(cfi-tests)
sec-tests-dump = $(addsuffix .dump, $(sec-tests))
sec-tests-prep := $(cfi-cpps-prep)

headers := $(wildcard $(base)/include/*.hpp)

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
CXXFLAGS := -I. -D$(TEST_ARCH) -D$(STACK_STRUCT) -$(GCC_OPT_LEVEL) -Wall
OBJDUMP := objdump
OBJDUMPFLAGS := -D

# compile targets

all: $(test-path) $(sec-tests)

$(test-path):
	-mkdir -p $@

$(cfi-tests): $(test-path)/%:$(cfi-path)/%.cpp $(headers)
	$(CXX) $(CXXFLAGS) $< -o $@

$(cfi-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

run: $(sec-tests)
	@(for t in $^; do $$t || echo $$t failed; done)

dump: $(sec-tests-dump)
$(sec-tests-dump): %.dump:%
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

prep: $(sec-tests-prep)

clean:
	-rm $(sec-tests)
	-rm $(sec-tests-dump)
	-rm $(sec-tests-prep)

.PHONY: clean run dump prep

