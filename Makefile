# Compiler flags
AR := ar rcs

LIBFLAGS := -g -std=c++11 -Wall -Werror -Wextra -Wconversion -Wpedantic
CXXFLAGS := -g -std=c++11
INCLUDES := -Iinclude

# Library variables
SRCDIR := ./src
BINDIR := ./bin
SRCBINDIR := $(SRCDIR)/bin

SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp, $(SRCBINDIR)/%.o, $(SRC))
LIBRARY := $(BINDIR)/libmathex++.a

# Sample variables
SAMPLEDIR := ./sample
SAMPLEBINDIR := $(SAMPLEDIR)/bin

# Phonies
build: $(LIBRARY)

clean:
	$(RM) $(BINDIR)/* $(SRCBINDIR)/* $(SAMPLEBINDIR)/*

# Library
$(LIBRARY): $(OBJ) | $(BINDIR)
	$(AR) $(LIBRARY) $(OBJ)

$(SRCBINDIR)/%.o: $(SRCDIR)/%.cpp | $(SRCBINDIR)
	$(CXX) $(LIBFLAGS) $(INCLUDES) -c $< -o $@

# Samples
$(SAMPLEBINDIR)/%: $(SAMPLEDIR)/%.cpp $(LIBRARY) | $(SAMPLEBINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ -L$(BINDIR) -lmathex++

# Directories
$(BINDIR):
	mkdir -p $@

$(SRCBINDIR):
	mkdir -p $@

$(SAMPLEBINDIR):
	mkdir -p $@
