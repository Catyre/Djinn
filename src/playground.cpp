/**
 * @file playground.cpp
 * @brief Main entry for the engine, runs whatever I need it to run to test the engine
 * @author Caleb Remocaldo
 * @date 11-10-2022
*/

#include "engine/particle.h"
#include "engine/pfgen.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#define LINECOUNT 5 // How many lines are printed out from toString()

using namespace engine;
using namespace std;

int main() {
    Vec3 x_i = Vec3(3.844e6, 0, 0); // [m]
    Vec3 v_i = Vec3(0, 3.7e6, 0); // [m/s]
    Vec3 a_i = Vec3(0, 0, 0); // [m/s^2]
    Vec3 springAnchor = Vec3(0, 100, 0);
    Vec3 pointGravityOrigin = Vec3(0, 0, 0);
    real pointGravityMass = 6e24;
    Particle *p1 = new Particle(x_i, v_i, a_i, .75, 1.3609751e-23);

    bool first = true;

    // Time resolution
    real dt = 1e-6; // [s]

    // Define force registry
    ParticleForceRegistry registry;

    // Define force generators to be applied to the particle
    ParticleGravity* gravity = new ParticleGravity(Vec3(0, -9.81, 0)); // Args: (Vec3) acceleration
    ParticlePointGravity* pointGravity = new ParticlePointGravity(pointGravityOrigin, pointGravityMass); // Args: (Vec3) origin, (real) mass
    ParticleAnchoredSpring* spring = new ParticleAnchoredSpring(&springAnchor, 25, 10); // Args: anchor point, spring constant, rest length
    ParticleUplift* uplift = new ParticleUplift(Vec3(), 1);

    // Register force generators with the particle
    //registry.add(p1, gravity);
    registry.add(p1, pointGravity);
    //registry.add(p1, spring);
    //registry.add(p1, uplift);
   
    //while (p1->getPosition().magnitude() < 2 * pointGravityOrigin.magnitude()) {
    while (true) {
        // This mess puts the cursor back to the top of the output after erasing everything line by line
        if (!first) {
            cout << "\x1b[" << LINECOUNT << "F";
            //for (int i = 0; i < LINECOUNT; i++) {cout << string("\33[2K\x1b[F");}
        }
        first = false;
        // End mess

        // Update forces for this time step
        registry.updateForces(dt);

        // Update particle
        p1->integrate(dt);

        // Print particle data
        cout << p1->toString();
    }

    delete gravity;
    delete spring;
    delete uplift;
    delete p1;

    return 0;
}