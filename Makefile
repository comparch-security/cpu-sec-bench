SHELL := /bin/bash

# default variables
TEST-ARCH     ?= ARCH_X86_64
GCC-OPT-LEVEL ?= O2

# define paths and objects
base = .

test-path = $(base)/test

cfi-path  = $(base)/cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/, $(basename $(notdir $(cfi-cpps))))


sec-tests := $(cfi-tests)

headers := $(wildcard $(base)/include/*.hpp)

# conditional variables
ifeq ($(TEST-ARCH), ARCH_X86_64)
  ifeq ($(GCC-OPT-LEVEL), g)
    STACK-STRUCT := STACK_FP_RET
  else
    STACK-STRUCT := STACK_RET
  endif
  headers += $(wildcard $(base)/lib/x86_64/*.hpp)
endif

CXX := g++
CXXFLAGS := -I. -D$(TEST-ARCH) -D$(STACK-STRUCT) -$(GCC-OPT-LEVEL)

# compile targets

all: $(test-path) $(sec-tests)

$(test-path):
	-mkdir -p $@

$(cfi-tests): $(test-path)/%:$(cfi-path)/%.cpp $(headers)
	$(CXX) $(CXXFLAGS) $< -o $@

run: $(sec-tests)
	@(for t in $^; do $$t || echo $$t failed; done)

clean:
	-rm $(sec-tests)

.PHONY: clean run

