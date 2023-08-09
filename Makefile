# Compiler flags
AR := ar rcs

LIBFLAGS := -g -O2 -std=c++11 -Wall -Werror -Wextra -Wconversion -Wpedantic
CXXFLAGS := -g -std=c++11
INCLUDES := -Iinclude

# Library variables
SRCDIR := ./src
BINDIR := ./bin
SRCBINDIR := $(SRCDIR)/bin

SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp, $(SRCBINDIR)/%.o, $(SRC))
LIBRARY := $(BINDIR)/libmathex++.a

# Testing variables
TESTDIR := ./test
TESTBINDIR := $(TESTDIR)/bin

TESTSRC := $(wildcard $(TESTDIR)/*.cpp)
TESTBIN := $(patsubst $(TESTDIR)/%.cpp, $(TESTBINDIR)/%, $(TESTSRC))

# Sample variables
SAMPLEDIR := ./sample
SAMPLEBINDIR := $(SAMPLEDIR)/bin

# Phonies
build: $(LIBRARY)

test: $(TESTBIN)
	CODE=0; for test in $(TESTBIN); do $$test || CODE=$$?; done; exit $$CODE

clean:
	$(RM) $(BINDIR)/* $(SRCBINDIR)/* $(TESTBINDIR)/* $(SAMPLEBINDIR)/*

# Library
$(LIBRARY): $(OBJ) | $(BINDIR)
	$(AR) $(LIBRARY) $(OBJ)

$(SRCBINDIR)/%.o: $(SRCDIR)/%.cpp | $(SRCBINDIR)
	$(CXX) $(LIBFLAGS) $(INCLUDES) -c $< -o $@

# Testing
$(TESTBINDIR)/%: $(TESTDIR)/%.cpp $(LIBRARY) | $(TESTBINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathex++ -lcriterion

# Samples
$(SAMPLEBINDIR)/%: $(SAMPLEDIR)/%.cpp $(LIBRARY) | $(SAMPLEBINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathex++

# Directories
$(BINDIR):
	mkdir -p $@

$(SRCBINDIR):
	mkdir -p $@

$(TESTBINDIR):
	mkdir -p $@

$(SAMPLEBINDIR):
	mkdir -p $@
