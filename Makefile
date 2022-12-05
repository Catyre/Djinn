# Detect OS
ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
detected_OS := Windows
else
detected_OS := $(shell uname)  # same as "uname -s".  Can be Linux, Darwin (Mac OSX), etc...
endif

IDIR =./include
CXX=g++
CXXFLAGS=-I$(IDIR) -g -std=c++11

ODIR=obj
LDIR =-L~/raylib/src

SRC = src
DEMODIR = $(SRC)/demos

# Get a sequence of all demos in the ./src/demos directory and strip the filenames
# 	of path and suffix
_DEMOS := $(wildcard $(DEMODIR)/*.cpp)
DEMOS = $(basename $(notdir $(_DEMOS)))

# Decide what library flags to use based on the current operating system
ifeq "Linux" "Linux"
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif
ifeq ($(detected_OS),"Darwin")
LIBS = -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`
endif

# Expand given header and object files to paths 
_DEPS = rlFPCamera.h rlHelper.h djinn/core.h djinn/particle.h djinn/pcontacts.h djinn/pfgen.h djinn/plinks.h djinn/precision.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = particle.o pcontacts.o pfgen.o plinks.o rlFPCamera.o rlHelper.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Link object files to corresponding .cpp files (do it for the demos too)
# $@ is the target, $^ is the dependencies, $< is the first dependency
$(ODIR)/%.o : $(SRC)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(ODIR)/%.o : $(DEMODIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# Resolve the exact compiler invocation to use dependent on OS (does not include Windows yet).
# The $@ and $^ are valid here because "=" is a recursive call
CMD=$(CXX) -o $(DEMODIR)/$@ $^ $(CXXFLAGS) $(LIBS)

# For some godforsaken reason, the above if-block is necessary.  If you try simply:
# CMD = $(CXX) -o $(DEMODIR)/$@ $^ $(CXXFLAGS) $(LIBS)
# then g++ does not know how to link Djinn with the demo.  I have no idea why.

# make all
all : $(DEMOS)

# Make rules for the demos
$(DEMOS) : % : $(ODIR)/%.o $(OBJ) $(DEPS)
	$(CMD)

# Clean up the object files
.PHONY: clean all

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~