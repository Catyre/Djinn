## Welcome to the Djinn Engine!
Currently, this engine is in its very very early form.  In fact, at the moment, it can only support gravitational forces, spring forces, a generic "uplift" force, and some others with demos only built for gravity (solarsystem and lunarorbit) and a brief demonstration of collisions (bouncyball).

This repository was made as a means of quickly sharing my codebase to people so they can help me troubleshoot problems/offer help in the design of the engine.

### About Djinn

https://user-images.githubusercontent.com/7707850/226810901-94b4dbc5-546b-474b-8d47-bb78ae93647a.mp4

This is a video demonstrating the `solarsystem` demo, which simulates the orbits of the inner planets and Jupiter.  The orbits, speeds, and relative sizes are to scale, however orbital distances are not to scale with the radii of the planets.

https://user-images.githubusercontent.com/7707850/232650381-0e58d14a-2ceb-4027-9c30-19031f3857c7.mov

This is another example of a demo in Djinn named `thermo`.  It simulates N-particles in a box subject to a Lennard-Jones potential.

There is also `bouncyball`, which is simply a ball bouncing against a flat surface, and `lunarorbit`, which is another orbit demo except just the Moon-Earth system.

Djinn is heavily (but not entirely) based on the paradigms introduced in "Game Physics Engine Development" by Ian Millington.  His GitHub -- and the Cyclone engine upon which Djinn is based -- can be found [here](https://github.com/idmillington).

Current To-do list:
- Implement electromagnetism
- Finish collision resolution
- Make a graphical demo for the spring-mass system depicted in src/demos/springmass.cpp
- Build a GUI so that simulations can be built outside of a literal program
- Come up with more ideas for demos

### Installing Djinn
Djinn relies on two other libraries: [spdlog](https://github.com/gabime/spdlog) for logging data about simulations, and [raylib](https://github.com/raysan5/raylib) to create the graphics that bring the simulated data to life.  These dependencies are handled in the cloning process of this repository:

    git clone --recurse-submodules https://github.com/Catyre/Djinn

To build Djinn, [cmake](https://cmake.org/download/) is required.  

NOTE: At the moment, Djinn's build process has only been demonstrated to work on Apple silicon and some Linux distros.  There is more to be done on my part to be able to build the demos on Windows.  

After downloading and installing cmake, the full download and build process is as simple as:

    git clone --recurse-submodules https://github.com/Catyre/Djinn
    cd Djinn
    mkdir build && cd build
    cmake ..
    make

At this point, CMake should have output a `Djinn/bin/` directory that contains the executable demos.  Currently, the executables are:
- bouncyball
- solarsystem
- lunarorbit
- springmass
