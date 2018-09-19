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


sec-tests := $(cfi-tests)

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

