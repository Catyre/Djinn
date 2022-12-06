/**
 * @file springmass.cpp
 * @brief Demonstration of the spring force generator in a gravitational field
 * @author Catyre
 * @date 11-10-2022
*/

#include "djinn/particle.h"
#include "djinn/pfgen.h"

#define LINECOUNT 5

int main() {
    // Create the necessary initial conditions:
    djinn::Vec3 x_i = djinn::Vec3(0, 100, 0); // [m]
    djinn::Vec3 v_i = djinn::Vec3(0, 0, 0); // [m/s]
    djinn::Vec3 a_i = djinn::Vec3(0, 0, 0); // [m/s^2]
    djinn::Vec3 springAnchor = djinn::Vec3(0, 100, 0); // [m]
    bool first = true;
    djinn::real dt = 1e-3; // [s]
    djinn::real mass = 1; // [kg]
    djinn::real damping = 1;
    djinn::real springConstant = 1;
    djinn::real restLength = 1; // [m]
    djinn::real elasticLimit = 1e1; // [m]

    djinn::Particle* p1 = new djinn::Particle(x_i, v_i, a_i, damping, 1/mass);

    djinn::ParticleForceRegistry registry;
    djinn::ParticleAnchoredSpring* spring = new djinn::ParticleAnchoredSpring(&springAnchor, springConstant, restLength, elasticLimit);
    djinn::ParticleEarthGravity* gravity = new djinn::ParticleEarthGravity(djinn::Vec3(0, -9.81, 0));

    registry.add(p1, spring);
    registry.add(p1, gravity);

    //while((p1->getAcceleration() != djinn::Vec3(0, 0, 0) || p1->getVelocity() != djinn::Vec3(0, 0, 0)) || first) {
    while(!(p1->getAcceleration().isZero() && p1->getVelocity().isZero()) || first) {
    //while (true) {
        // This mess puts the cursor back to the top of the output after erasing everything line by line
        if (!first) std::cout << "\x1b[" << LINECOUNT << "F";
        first = false;
        // End mess

        registry.updateForces(dt);
        p1->integrate(dt);
        std::cout << p1->toString();
    }

    return 0;
} // int main()