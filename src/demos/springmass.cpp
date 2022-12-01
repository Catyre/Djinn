/**
 * @file springmass.cpp
 * @brief Demonstration of the spring force generator in a gravitational field
 * @author Catyre
 * @date 11-10-2022
*/

#include "djinn/particle.h"
#include "djinn/pfgen.h"

#define LINECOUNT 5

using namespace djinn;
using namespace std;

int main() {
    // Create the necessary initial conditions:
    Vec3 x_i = Vec3(0, 100, 0); // [m]
    Vec3 v_i = Vec3(0, 0, 0); // [m/s]
    Vec3 a_i = Vec3(0, 0, 0); // [m/s^2]
    Vec3 springAnchor = Vec3(0, 100, 0); // [m]
    bool first = true;
    real dt = 1e-3; // [s]
    real mass = 1; // [kg]
    real damping = 1;
    real springConstant = 1;
    real restLength = 1; // [m]
    real elasticLimit = 1e1; // [m]

    Particle* p1 = new Particle(x_i, v_i, a_i, damping, 1/mass);

    ParticleForceRegistry registry;
    ParticleAnchoredSpring* spring = new ParticleAnchoredSpring(&springAnchor, springConstant, restLength, elasticLimit);
    ParticleEarthGravity* gravity = new ParticleEarthGravity(Vec3(0, -9.81, 0));

    registry.add(p1, spring);
    registry.add(p1, gravity);

    //while((p1->getAcceleration() != Vec3(0, 0, 0) || p1->getVelocity() != Vec3(0, 0, 0)) || first) {
    while(!(p1->getAcceleration().isZero() && p1->getVelocity().isZero()) || first) {
    //while (true) {
        // This mess puts the cursor back to the top of the output after erasing everything line by line
        if (!first) cout << "\x1b[" << LINECOUNT << "F";
        first = false;
        // End mess

        registry.updateForces(dt);
        p1->integrate(dt);
        cout << p1->toString();
    }

    return 0;
} // int main()