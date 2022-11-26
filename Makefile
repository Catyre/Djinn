CC = g++
CFLAGS = -g -std=c++11 -O3 -I ./include 
RLFLAGS = -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`

playground : src/playground.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) -o playground src/playground.cpp src/particle.cpp src/pfgen.cpp

lunarorbit : src/demos/lunarorbit.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) $(RLFLAGS) -o src/demos/lunarorbitNew src/demos/lunarorbit.cpp src/particle.cpp src/pfgen.cpp

solarsystem : src/demos/solarsystem.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) $(RLFLAGS) -o src/demos/solarsystem src/demos/solarsystem.cpp src/particle.cpp src/pfgen.cpp

graphics : src/demos/graphicstest.cpp 
	$(CC) $(CFLAGS) $(RLFLAGS) -o src/demos/graphicstest src/demos/graphicstest.cpp

MCVE : MCVE.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) $(RLFLAGS) -o MCVE MCVE.cpp src/particle.cpp src/pfgen.cpp

springmass : src/demos/springmass.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) -o src/demos/springmass src/demos/springmass.cpp src/particle.cpp src/pfgen.cpp

ballistic : src/demos/ballistic/ballistic.cpp
	$(CC) $(CFLAGS) -o src/demos/ballistic src/demos/ballistic/ballistic.cpp



