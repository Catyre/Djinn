CC = g++
CFLAGS = -g -std=c++11 -O3 -I ./include -I /usr/local/Cellar/fmt/9.1.0/include/ -I /usr/local/Cellar/spdlog/1.10.0_1/include
RLFLAGS = -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`

lunarorbit : src/demos/lunarorbit.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) $(RLFLAGS) -o src/demos/lunarorbit src/demos/lunarorbit.cpp src/particle.cpp src/pfgen.cpp

solarsystem : src/demos/solarsystem.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) $(RLFLAGS) -o src/demos/solarsystem src/demos/solarsystem.cpp src/particle.cpp src/pfgen.cpp

springmass : src/demos/springmass.cpp src/particle.cpp src/pfgen.cpp
	$(CC) $(CFLAGS) -o src/demos/springmass src/demos/springmass.cpp src/particle.cpp src/pfgen.cpp

ballistic : src/demos/ballistic/ballistic.cpp
	$(CC) $(CFLAGS) -o src/demos/ballistic src/demos/ballistic/ballistic.cpp

MCVE : src/demos/MCVE.cpp
	$(CC) $(CFLAGS) -o src/demos/MCVE src/demos/MCVE.cpp



