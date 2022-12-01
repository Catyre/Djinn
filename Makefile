IDIR =./include
CXX=g++
CXXFLAGS=-I$(IDIR) -g -std=c++11
RLFLAGS = -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`

ODIR=obj
LDIR =../lib

SRC = src
DEMOS = $(SRC)/demos

LIBS=-lfmt

# Expand given header and object files to paths 
_DEPS = rlFPCamera.h rlHelper.h djinn/core.h djinn/particle.h djinn/pcontacts.h djinn/pfgen.h djinn/plinks.h djinn/precision.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = particle.o pcontacts.o pfgen.o plinks.o rlFPCamera.o rlHelper.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Link object files to corresponding .cpp files (do it for the demos too)
# $@ is the target, $^ is the dependencies, $< is the first dependency
$(ODIR)/%.o : $(SRC)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(ODIR)/%.o : $(DEMOS)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# Make rules for the demos
bouncyball : $(OBJ) $(DEMOS)/bouncyball.o
	$(CXX) -o $(ODIR)/$@ $^ $(CXXFLAGS) $(RLFLAGS) $(LIBS)

lunarorbit : $(OBJ) $(DEMOS)/lunarorbit.o
	$(CXX) -o $(ODIR)/$@ $^ $(CXXFLAGS) $(RLFLAGS) $(LIBS)

solarsystem : $(OBJ) $(DEMOS)/solarsystem.o
	$(CXX) -o $(ODIR)/$@ $^ $(CXXFLAGS) $(RLFLAGS) $(LIBS)

springmass : $(OBJ) $(DEMOS)/springmass.o
	$(CXX) -o $(ODIR)/$@ $^ $(CXXFLAGS) $(RLFLAGS) $(LIBS)

# Clean up the object files
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
