/**
 * @file pfgen.cpp
 * @brief Source file to define behavior for particle force generators
 * @author Catyre
 */

/* FORCES TO IMPLEMENT:
 *  Bouyancy
 *  Friction
 *  Electricity
 *  Magnetism
 */

/* FORCES IMPLEMENTED:
 *  Drag
 *  Gravity
 *  Uplift
 */

#define G 6.67408e-11 // [m^3 kg^-1 s^-2]

#include "djinn/pfgen.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <iostream>

void djinn::ParticleUniversalForceRegistry::add(djinn::Particle *particle) {
    ParticleUniversalForceRegistration registration;
    registration.particle = particle;

    // Don't add duplicates
    if (find(begin(registrations), end(registrations), registration) == end(registrations)) {
        registrations.push_back(registration);

        // Log registration
        spdlog::info("Added particle \"{}\" to universal force registry", particle->getName());
    } else {
        // Log discard
        spdlog::info("Particle \"{}\" already in universal force registry...discarding", particle->getName());
    }
}

void djinn::ParticleUniversalForceRegistry::add(std::vector<djinn::Particle *> particles) {
    for (std::vector<djinn::Particle *>::iterator particle = particles.begin(); particle != particles.end(); particle++) {
        ParticleUniversalForceRegistration registration;
        registration.particle = *particle;

        // Don't add duplicates
        if (find(begin(registrations), end(registrations), registration) == end(registrations)) {
            registrations.push_back(registration);

            // Log registration
            spdlog::info("Added particle \"{}\" to universal force registry", (*particle)->getName());
        } else {
            // Log discard
            spdlog::info("Particle \"{}\" already in universal force registry...discarding", (*particle)->getName());
        }
    }
}

void djinn::ParticleUniversalForceRegistry::remove(djinn::Particle *particle) {
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++) {
        if (i->particle == particle) {
            registrations.erase(i);

            // Log removal
            spdlog::info("Removed particle \"{}\" from universal force registry", particle->getName());
            return;
        }
    }
}

void djinn::ParticleUniversalForceRegistry::applyGravity() {
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++) {
        for (Registry::iterator j = registrations.begin(); j != registrations.end(); j++) {
            if (i->particle != j->particle) {
                djinn::Vec3 r = i->particle->getPosition() - j->particle->getPosition();
                djinn::real rMagSquared = r.squareMagnitude();
                r.normalize();
                djinn::Vec3 force = r * (-G * i->particle->getMass() * j->particle->getMass()) / (rMagSquared);
                i->particle->addForce(force);

                // Log force application
                spdlog::info("Applied gravitational force from \"{}\" on \"{}\" ({} N)", j->particle->getName(), i->particle->getName(), force.toString());
            }
        }
    }
}

void djinn::ParticleUniversalForceRegistry::integrateAll(djinn::real duration) {
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++) {
        i->particle->integrate(duration);

        // Log integration
        spdlog::info("Integrated particle in universal force registry");
    }
}

void djinn::ParticleForceRegistry::updateForces(djinn::real duration) {
    Registry::iterator i = registrations.begin();

    for (; i != registrations.end(); i++) {
        i->fg->updateForce(i->particle, duration);
    }
}

void djinn::ParticleForceRegistry::add(djinn::Particle *particle, djinn::ParticleForceGenerator *fg) {
    ParticleForceRegistration registration;
    registration.particle = particle;
    registration.fg = fg;

    // Add to the list of registrations only if it's not already in it
    if (find(begin(registrations), end(registrations), registration) == end(registrations)) {
        registrations.push_back(registration);

        // Log registration
        spdlog::info("Added particle \"{}\" to force registry", particle->getName());
    } else {
        // Log discard
        spdlog::info("djinn::Particle \"{}\" already in force registry...discarding", particle->getName());
    }
}

void djinn::ParticleForceRegistry::integrateAll(djinn::real duration) {
    Registry::iterator i = registrations.begin();

    for (; i != registrations.end(); i++) {
        i->particle->integrate(duration);
    }
}

void djinn::ParticleForceRegistry::remove(djinn::Particle *particle, djinn::ParticleForceGenerator *fg) {
    Registry::iterator i = registrations.begin();

    for (; i != registrations.end(); i++) {
        if (i->particle == particle && i->fg == fg) {
            registrations.erase(i);
            return;
        }
    }
}

void djinn::ParticleForceRegistry::clear() {
    registrations.clear();
}

djinn::ParticleEarthGravity::ParticleEarthGravity(const djinn::Vec3 &gravity)
    : gravity(gravity) {
}

void djinn::ParticleEarthGravity::updateForce(djinn::Particle *particle, djinn::real duration) {
    // Check if particle has a finite mass
    if (!particle->hasFiniteMass())
        return;

    // Calculate force
    djinn::Vec3 force = gravity * particle->getMass();

    // Apply force to the particle
    particle->addForce(force);
    // Log force application
    spdlog::info("Applied Earth gravity to particle \"{}\" ({})", particle->getName(), force.toString());
}

djinn::ParticlePointGravity::ParticlePointGravity(const djinn::Vec3 &origin, const djinn::real mass)
    : origin(origin), mass(mass) {
}

void djinn::ParticlePointGravity::setOrigin(const djinn::Vec3 &origin) {
    this->origin = origin;
}

void djinn::ParticlePointGravity::updateForce(djinn::Particle *particle, djinn::real duration) {
    // Check if particle has a finite mass
    if (!particle->hasFiniteMass())
        return;

    djinn::Vec3 force;

    // Calculate the direction of the force
    djinn::Vec3 direction = particle->getPosition() - origin;
    djinn::real distance = direction.magnitude();
    direction.normalize();

    // Calculate the magnitude of the force
    djinn::real forceMagnitude = -G * particle->getMass() * mass / (distance * distance);
    force = direction * forceMagnitude;

    // Apply the force
    particle->addForce(force);

    // Log force application
    spdlog::info("Applied fixed-point gravitational force to particle \"{}\" ({})", particle->getName(), force.toString());
} // void djinn::ParticlePointGravity::updateForce

void djinn::ParticleDrag::updateForce(djinn::Particle *particle, djinn::real duration) {
    djinn::Vec3 force;
    particle->getVelocity(&force);

    // Calculate total drag coefficient
    djinn::real dragCoeff = force.magnitude();
    dragCoeff = k1 * dragCoeff + k2 * real_pow(dragCoeff, 2);

    force.normalize();
    force *= -dragCoeff;
    particle->addForce(force);

    // Log force application
    spdlog::info("Applied drag force to particle \"{}\" ({})", particle->getName(), force.toString());
} // void djinn::ParticleDrag::updateForce

djinn::ParticleUplift::ParticleUplift(djinn::Vec3 origin, djinn::real radius)
    : origin(origin), radius(radius) {
}

void djinn::ParticleUplift::updateForce(djinn::Particle *particle, djinn::real duration) {
    djinn::Vec3 force;

    djinn::real x = particle->getPosition().x;
    djinn::real z = particle->getPosition().z;

    if (real_pow((x - origin.x), 2) + real_pow((z - origin.z), 2) < real_pow(radius, 2)) {
        force.y = 1.0; // Apply constant force in y-direction
        particle->addForce(force);

        // Log force application
        spdlog::info("Applied uplift force to particle \"{}\" ({})", particle->getName(), force.toString());
    }
} // void djinn::ParticleUplift::updateForce

djinn::ParticleSpring::ParticleSpring(djinn::Particle *other, djinn::real springConstant, djinn::real restLength)
    : other(other), springConstant(springConstant), restLength(restLength) {
}

void djinn::ParticleSpring::updateForce(djinn::Particle *particle, djinn::real duration) {
    // Calculate the vector of the spring
    djinn::Vec3 force = particle->getPosition();
    force -= other->getPosition();

    // Calculate the magnitude of the force
    djinn::real magnitude = force.magnitude();
    magnitude = real_abs(magnitude - restLength);
    magnitude *= springConstant;

    // Calculate the final force and apply it
    force.normalize();
    force *= -magnitude;
    particle->addForce(force);

    // Log force application
    spdlog::info("Applied spring force to particle \"{}\" ({})", particle->getName(), force.toString());
} // void djinn::ParticleSpring::updateForce

djinn::real djinn::ParticleSpring::calcCritDamping(djinn::real mass) {
    return 2 * real_sqrt(mass * springConstant);
}

djinn::ParticleAnchoredSpring::ParticleAnchoredSpring(djinn::Vec3 *anchor, djinn::real springConstant, djinn::real restLength, djinn::real elasticLimit)
    : anchor(anchor), springConstant(springConstant), restLength(restLength), elasticLimit(elasticLimit) {
}

void djinn::ParticleAnchoredSpring::updateForce(djinn::Particle *particle, djinn::real duration) {
    // Calculate the vector of the spring
    djinn::Vec3 force = particle->getPosition();
    force -= *anchor;
    djinn::real stretchedLength = force.magnitude();

    // Calculate the magnitude of the force
    djinn::real magnitude = real_abs(stretchedLength - restLength) * springConstant;

    // Calculate the final force and apply it
    force.normalize();
    force *= -magnitude;

    // If the spring is stretched too far, reduce its spring constant (and therefore the force) to a quarter
    if (stretchedLength >= elasticLimit)
        force *= .25;

    particle->addForce(force);

    // Log force application
    spdlog::info("Applied anchored spring force to particle \"{}\" ({})", particle->getName(), force.toString());
} // void djinn::ParticleAnchoredSpring::updateForce

djinn::ParticleBungee::ParticleBungee(djinn::Particle *other, djinn::real springConstant, djinn::real restLength)
    : other(other), springConstant(springConstant), restLength(restLength) {
}

void djinn::ParticleBungee::updateForce(djinn::Particle *particle, djinn::real duration) {
    // Calculate the vector of the spring.
    djinn::Vec3 force;
    particle->getPosition(&force);
    force -= other->getPosition();

    // Check if the bungee is compressed.
    djinn::real magnitude = force.magnitude();
    if (magnitude <= restLength)
        return;

    // Calculate the magnitude of the force.
    magnitude = springConstant * (restLength - magnitude);

    // Calculate the final force and apply it.
    force.normalize();
    force *= -magnitude;
    particle->addForce(force);

    // Log force application
    spdlog::info("Applied bungee spring force to particle \"{}\" ({})", particle->getName(), force.toString());
} // djinn::ParticleBungee::updateForce

djinn::ParticleFakeSpring::ParticleFakeSpring(djinn::Vec3 *anchor, djinn::real springConstant, djinn::real damping)
    : anchor(anchor), springConstant(springConstant), damping(damping) {
}

void djinn::ParticleFakeSpring::updateForce(djinn::Particle *particle, djinn::real duration) {
    // Check for finite non-zero mass
    if (!particle->hasFiniteMass())
        return;

    djinn::Vec3 vel = particle->getVelocity();

    // Calculate the position of the particle relative to the anchor
    djinn::Vec3 pos;
    particle->getPosition(&pos);
    pos -= *anchor;

    // Calculate constants and check that they are in bounds
    djinn::real gamma = 0.5 * real_sqrt(4 * springConstant - damping * damping);
    if (gamma == 0.0)
        return;
    djinn::Vec3 c = pos * (damping / (2.0 * gamma)) + vel * (1.0 / gamma);

    // Calculate the target position
    djinn::Vec3 target = pos * real_cos(gamma * duration) + c * real_sin(gamma * duration);
    target *= real_exp(-0.5 * damping * duration);

    // Calculate the resulting acceleration (and therefore the force)
    djinn::Vec3 acc = (target - pos) * (1.0 / (duration * duration)) - vel * duration;
    djinn::Vec3 force = acc * particle->getMass();
    particle->addForce(force);

    // Log force application
    spdlog::info("Applied fake spring force to particle \"{}\" ({})", particle->getName(), force.toString());
} // void djinn::ParticleFakeSpring
