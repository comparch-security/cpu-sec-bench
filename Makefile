SHELL := /bin/bash

# default variables
ARCH          ?= $(shell arch)
GCC_OPT_LEVEL ?= O2
CXX           ?= g++
OBJDUMP       ?= objdump

# define paths and objects
base = .

test-path = $(base)/test-$(ARCH)
LD_LIBRARY_PATH=$(test-path)

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

CXXFLAGS := -I./lib -$(GCC_OPT_LEVEL) -Wall
# -fno-omit-frame-pointer
OBJDUMPFLAGS := -D -l -S
RUN_SCRIPT := ../run-test.py

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

$(base)/lib/common/mss.o: %.o : %.cpp $(base)/lib/include/mss.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rubbish += $(base)/lib/common/mss.o

$(mss-tests): $(test-path)/mss-%:$(mss-path)/%.cpp $(extra_objects) $(headers) $(base)/lib/common/mss.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(base)/lib/common/mss.o -o $@

rubbish += $(mss-tests)

$(mss-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(mts-tests): $(test-path)/mts-%:$(mts-path)/%.cpp $(extra_objects) $(base)/lib/common/mss.o
	$(CXX) $(CXXFLAGS) $< $(extra_objects) $(base)/lib/common/mss.o -o $@

rubbish += $(mts-tests)

$(mts-cpps-prep): %.prep:%
	$(CXX) -E $(CXXFLAGS) $< > $@

$(acc-tests): $(test-path)/acc-%:$(acc-path)/%.cpp $(extra_objects)
	$(CXX) $(CXXFLAGS) $< $(extra_objects) -o $@

rubbish += $(acc-tests)

$(acc-cpps-prep): %.prep:%
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
