
base = .

test-path = $(base)/test

cfi-path  = $(base)/cfi
cfi-cpps  = $(wildcard $(cfi-path)/*.cpp)
cfi-tests = $(addprefix $(test-path)/, $(basename $(notdir $(cfi-cpps))))


sec-tests := $(cfi-tests)

# conditional variables
TEST-ARCH ?= ARCH_X86_64
CXX := g++
CXXFLAGS := -I. -D$(TEST-ARCH) -O2

# compile targets

all: $(test-path) $(sec-tests)

$(test-path):
	-mkdir -p $@

$(cfi-tests): $(test-path)/%:$(cfi-path)/%.cpp $(headers)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	-rm $(sec-tests)

.PHONY: clean

