# Define recursive wildcard function
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Define some basic macros
IDIR =./include
CXX=g++
CXXFLAGS=-I$(IDIR) -g -std=c++11
ODIR = obj
SRC = src
DEMODIR = $(SRC)/demos

# Define directories that need to be made on the first make invocation and make those
#	directories if they don't already exist
LOGS = logs
MKDIRS = $(ODIR) $(LOGS)
$(shell mkdir -p $(MKDIRS))

# In case we need it later
#LDIR =-L/opt/vc/lib

# Get a sequence of all demos in the ./src/demos directory and strip the filenames
# 	of path and suffix
_DEMOS := $(wildcard $(DEMODIR)/*.cpp)
DEMOS = $(basename $(notdir $(_DEMOS)))

# Decide what library flags to use based on the current operating system
ifeq ($(shell uname),Linux)
  #Tested on an RPi 4
  LIBS =  -lraylib -lGLESv2 -lEGL -lX11 -lpthread -lrt -lm -ldl -ldrm -lgbm
endif

ifeq ($(shell uname),Darwin)
  LIBS = -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`
endif

# Find all header files starting from Djinn/include
DEPS = $(call rwildcard, $(IDIR), *.h)

# Assign all .cpp files under Djinn/src/ to SRCCODE
SRCCODE = $(shell find ./$(SRC) -type f -wholename "./**/*.cpp")

# Convert those sources into object filenames
OBJ = $(addprefix $(ODIR)/, $(notdir $(SRCCODE:%.cpp=$(ODIR)/%.o)))

# Filter out the demos from the objects (else there will be multiple mains compiled)
OBJ_NO_DEMOS = $(filter-out $(addsuffix .o, $(addprefix $(ODIR)/, $(DEMOS))), $(OBJ))

# Link object files to corresponding .cpp files (do it for the demos too)
# $@ is the target, $^ is the dependencies, $< is the first dependency
$(ODIR)/%.o : $(SRC)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(ODIR)/%.o : $(DEMODIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# Resolve the exact compiler invocation to use dependent on OS (does not include Windows yet).
# The $@ and $^ are valid here because "=" is a recursive call
CMD = $(CXX) $(LDIR) -o $(DEMODIR)/$@ $^ $(CXXFLAGS) $(LIBS)

# make all
all : $(DEMOS)

# Make rules for the demos
$(DEMOS) : % : $(ODIR)/%.o $(OBJ_NO_DEMOS) $(DEPS)
	$(CMD)

# Clean up the object files
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(LOGS)/*.log $(addprefix $(DEMODIR)/, $(DEMOS))

.PHONY: clean all
