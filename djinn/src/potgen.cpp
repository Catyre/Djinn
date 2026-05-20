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
        //spdlog::info("Integrated particle in potential registry");
    }
}

void djinn::PotentialRegistry::clear() {
    registrations.clear();
}


void djinn::LennardJones::updatePotential(djinn::Particle *particle, djinn::real var) {
    djinn::real sr6 = real_pow(sigma / var, 6);
    particle->addPotential(4 * epsilon * (sr6 * sr6 - sr6));
}

// F = -grad(U)
void djinn::LennardJones::updateForce(djinn::Particle *particle, djinn::Vec3 r_vec, djinn::real r_mag, djinn::real dvar) {
    djinn::real r2 = r_mag * r_mag;
    djinn::Vec3 r_hat = r_vec / r_mag;

    djinn::real sigmaSq = sigma * sigma;
    djinn::real u = sigmaSq / r2;
    djinn::real u3 = u * u * u;
    djinn::real u6 = u3 * u3;
    
    djinn::real forceMag = (24.0f * epsilon / r2) * (2.0f * u6 - u3);
    djinn::Vec3 force = r_hat * forceMag;

    particle->addForce(force);
}
