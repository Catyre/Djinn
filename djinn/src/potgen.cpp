/**
 * @file potgen.cpp
 * @brief Define methods for molecular dynamics
 * @author Catyre
 */

#include "djinn/potgen.h"
#include "djinn/numerical.h"
#include "spdlog/spdlog.h"
#include <cmath>

void djinn::PotentialRegistry::add(djinn::Particle *particle, djinn::PotentialGenerator *pg) {
    PotentialRegistration registration;
    registration.particle = particle;
    registration.pg = pg;

    // Don't add duplicates
    if (find(begin(registrations), end(registrations), registration) == end(registrations)) {
        registrations.push_back(registration);

        // Log registration
        spdlog::info("Added particle \"{}\" to potential registry", particle->getName());
    } else {
        // Log discard
        spdlog::info("Particle \"{}\" already in potential registry...discarding", particle->getName());
    }
}

void djinn::PotentialRegistry::remove(Particle *particle, PotentialGenerator *pg) {
    Registry::iterator i = registrations.begin();

    for (; i != registrations.end(); i++) {
        if (i->particle == particle && i->pg == pg) {
            registrations.erase(i);
            return;
        }
    }
}

void djinn::PotentialRegistry::integrateAll(djinn::real duration) {
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++) {
        i->particle->integrate(duration);

        // Log integration
        spdlog::info("Integrated particle in potential registry");
    }
}

void djinn::PotentialRegistry::clear() {
    registrations.clear();
}

djinn::LennardJones::LennardJones(djinn::real sigma, djinn::real epsilon)
    : sigma(sigma), epsilon(epsilon) {
}

void djinn::LennardJones::updatePotential(djinn::Particle *particle, djinn::real var) {
    djinn::real sr6 = real_pow(sigma / var, 6);
    particle->addPotential(4 * epsilon * (sr6 * sr6 - sr6));
}

// F = -grad(U)
void djinn::LennardJones::updateForce(djinn::Particle *particle, djinn::real var, djinn::real dvar) {
    djinn::Vec3 pos = particle->getPosition();

    djinn::Vec3 force = djinn::rungeKutta4([=](djinn::Vec3 r, djinn::real dr) {
        djinn::real sr6 = real_pow(sigma / r.magnitude(), 6);
        return 4 * epsilon * (sr6 * sr6 - sr6);
    },
                                           pos, var, dvar);

    particle->addForce(force);
}
