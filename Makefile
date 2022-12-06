# Define some basic macros
IDIR =./include
CXX=g++
CXXFLAGS=-I$(IDIR) -g -std=c++11
ODIR = obj
SRC = src
DEMODIR = $(SRC)/demos

# Define directories that need to be made on the first make invocation and make those
#	directories if they don't already exist
MKDIRS = $(ODIR) logs
$(shell mkdir -p $(MKDIRS))

LDIR =-L/home/pi/raylib/src -L/opt/vc/lib

# Get a sequence of all demos in the ./src/demos directory and strip the filenames
# 	of path and suffix
_DEMOS := $(wildcard $(DEMODIR)/*.cpp)
DEMOS = $(basename $(notdir $(_DEMOS)))

# Get a sequence of all demo object files in the obj/ directory
DEMOS_O = $(addprefix $(OBJ)/, $(addsuffix .o, $(DEMOS)))

# Decide what library flags to use based on the current operating system
ifeq ($(shell uname),Linux)
#Tested on an RPi 4
LIBS =  -lraylib -lGLESv2 -lEGL -lX11 -lpthread -lrt -lm -lbcm_host -ldl -ldrm -lgbm
endif
ifeq ($(shell uname),Darwin)
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
#	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# Resolve the exact compiler invocation to use dependent on OS (does not include Windows yet).
# The $@ and $^ are valid here because "=" is a recursive call
CMD = $(CXX) $(LDIR) -o $(DEMODIR)/$@ $^ $(CXXFLAGS) $(LIBS)

# make all
all : $(DEMOS)

# Make rules for the demos
$(DEMOS) : % : $(ODIR)/%.o $(OBJ) $(DEPS)
	$(CMD)

# Clean up the object files
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

.PHONY: clean all
