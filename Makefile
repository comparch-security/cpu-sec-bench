
base = .

test-path = $(base)/test

cfi-path  = $(base)/cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/, $(basename $(notdir $(cfi-cpps))))


sec-tests := $(cfi-tests)

headers := $(wildcard $(base)/include/*.hpp)

# conditional variables
TEST-ARCH ?= ARCH_X86_64
#STACK-STRUCT ?= STACK_FP_RET
STACK-STRUCT ?= STACK_RET

CXX := g++
CXXFLAGS := -I. -D$(TEST-ARCH) -D$(STACK-STRUCT) -O2

headers += $(wildcard $(base)/lib/x86_64/*.hpp)

# compile targets

all: $(test-path) $(sec-tests)

$(test-path):
	-mkdir -p $@

$(cfi-tests): $(test-path)/%:$(cfi-path)/%.cpp $(headers)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	-rm $(sec-tests)

.PHONY: clean

