/**
 * @file pfgen.cpp
 * @brief Source file to define behavior for particle force generators
 * @author Catyre
 * @date 11-10-2022
*/

/* FORCES TO IMPLMENT
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
#include <iostream>
#include "engine/pfgen.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <algorithm>

using namespace engine;
using namespace std;

void ParticleUniversalForceRegistry::add(Particle* particle) {
    ParticleUniversalForceRegistration registration;
    registration.particle = particle;

    // Don't add duplicates
    if (find(begin(registrations), end(registrations), registration) == end(registrations)) registrations.push_back(registration);
}

void ParticleUniversalForceRegistry::add(vector<Particle*> particles) {
    for(vector<Particle*>::iterator particle = particles.begin(); particle != particles.end(); particle++) {
        ParticleUniversalForceRegistration registration;
        registration.particle = *particle;

        // Don't add duplicates
        if (find(begin(registrations), end(registrations), registration) == end(registrations)) registrations.push_back(registration);
    }
}

void ParticleUniversalForceRegistry::remove(Particle* particle) {
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++) {
        if (i->particle == particle) {
            registrations.erase(i);
            return;
        }
    }
}

void ParticleUniversalForceRegistry::applyGravity() {
    for(Registry::iterator i = registrations.begin(); i != registrations.end(); i++) {
        for(Registry::iterator j = registrations.begin(); j != registrations.end(); j++) {
            if (i->particle != j->particle) {
                Vec3 r = i->particle->getPosition() - j->particle->getPosition();
                real rMag = r.magnitude();
                r.normalize();
                Vec3 force = r * (-G * i->particle->getMass() * j->particle->getMass()) / (rMag * rMag);
                i->particle->addForce(force);
            }
        }
    }
}

void ParticleUniversalForceRegistry::integrateAll(real duration) {
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++) {
        i->particle->integrate(duration);
    }
}

void ParticleForceRegistry::updateForces(real duration) {
    Registry::iterator i = registrations.begin();

    for (; i != registrations.end(); i++) {
        i->fg->updateForce(i->particle, duration);
    }
}

void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* fg) {
    ParticleForceRegistration registration;
    registration.particle = particle;
    registration.fg = fg;

    // Add to the list of registrations only if it's not already in it
    if (find(begin(registrations), end(registrations), registration) == end(registrations)) registrations.push_back(registration);
}

void ParticleForceRegistry::integrateAll(real duration) {
    Registry::iterator i = registrations.begin();

    for (; i != registrations.end(); i++) {
        i->particle->integrate(duration);
    }
}

void ParticleForceRegistry::remove(Particle* particle, ParticleForceGenerator* fg) {
    Registry::iterator i = registrations.begin();

    for (; i != registrations.end(); i++) {
        if (i->particle == particle && i->fg == fg) {
            registrations.erase(i);
            return;
        }
    }
}

void ParticlePointGravity::applyGravBodies() {

}

void ParticleForceRegistry::clear() {
    registrations.clear();
}

ParticleGravity::ParticleGravity(const Vec3& gravity)
: gravity(gravity) 
{
}

void ParticleGravity::updateForce(Particle* particle, real duration) {
    // Check if particle has a finite mass
    if (!particle->hasFiniteMass()) return;

    // Apply force to the particle
    particle->addForce(gravity * particle->getMass());
}

ParticlePointGravity::ParticlePointGravity(const Vec3& origin, const real mass)
: origin(origin), mass(mass) 
{
}

void ParticlePointGravity::setOrigin(const Vec3& origin) {
    this->origin = origin;
}

void ParticlePointGravity::updateForce(Particle* particle, real duration) {
    // Check if particle has a finite mass
    if (!particle->hasFiniteMass()) return;

    Vec3 force;

    // Calculate the direction of the force
    Vec3 direction = particle->getPosition() - origin;
    real distance = direction.magnitude();
    direction.normalize();

    // Calculate the magnitude of the force
    real forceMagnitude = -G * particle->getMass() * mass / (distance * distance);
    force = direction * forceMagnitude;
    
    // Apply the force
    particle->addForce(force);
} // void ParticlePointGravity::updateForce

void ParticleDrag::updateForce(Particle* particle, real duration) {
    Vec3 force;
    particle->getVelocity(&force);

    // Calculate total drag coefficient
    real dragCoeff = force.magnitude();
    dragCoeff = k1 * dragCoeff + k2 * real_pow(dragCoeff, 2);

    force.normalize();
    force *= -dragCoeff;
    particle->addForce(force);
} // void ParticleDrag::updateForce

ParticleUplift::ParticleUplift(Vec3 origin, real radius)
: origin(origin), radius(radius)
{
}

void ParticleUplift::updateForce(Particle* particle, real duration) {
    Vec3 force;

    real x = particle->getPosition().x;
    real z = particle->getPosition().z;

    if (real_pow((x - origin.x), 2) + real_pow((z - origin.z), 2) < real_pow(radius, 2)) {
        force.y = 1.0f; // Apply constant force in y-direction
        particle->addForce(force);
    }
} // void ParticleUplift::updateForce

ParticleSpring::ParticleSpring(Particle* other, real springConstant, real restLength)
: other(other), springConstant(springConstant), restLength(restLength) 
{
}

void ParticleSpring::updateForce(Particle* particle, real duration) {
    // Calculate the vector of the spring
    Vec3 force = particle->getPosition();
    force -= other->getPosition();

    // Calculate the magnitude of the force
    real magnitude = force.magnitude();
    magnitude = real_abs(magnitude - restLength);
    magnitude *= springConstant;

    // Calculate the final force and apply it
    force.normalize();
    force *= -magnitude;
    particle->addForce(force);
} // void ParticleSpring::updateForce

real ParticleSpring::calcCritDamping(real mass) {
    return 2 * real_sqrt(mass * springConstant);
}

ParticleAnchoredSpring::ParticleAnchoredSpring(Vec3* anchor, real springConstant, real restLength, real elasticLimit)
: anchor(anchor), springConstant(springConstant), restLength(restLength), elasticLimit(elasticLimit)
{
}

void ParticleAnchoredSpring::updateForce(Particle* particle, real duration) {
    // Calculate the vector of the spring
    Vec3 force = particle->getPosition();
    force -= *anchor;
    real stretchedLength = force.magnitude();

    // Calculate the magnitude of the force
    real magnitude = real_abs(stretchedLength - restLength) * springConstant;

    // Calculate the final force and apply it
    force.normalize();
    force *= -magnitude;

    // If the spring is stretched too far, reduce its spring constant (and therefore the force) to a quarter
    if(stretchedLength >= elasticLimit) force *= .25;

    particle->addForce(force);
} // void ParticleAnchoredSpring::updateForce

ParticleBungee::ParticleBungee(Particle *other, real springConstant, real restLength) 
: other(other), springConstant(springConstant), restLength(restLength)
{
}

void ParticleBungee::updateForce(Particle *particle, real duration) {
    // Calculate the vector of the spring.
    Vec3 force;
    particle->getPosition(&force);
    force -= other->getPosition();

    // Check if the bungee is compressed.
    real magnitude = force.magnitude();
    if (magnitude <= restLength) return;

    // Calculate the magnitude of the force.
    magnitude = springConstant * (restLength - magnitude);

    // Calculate the final force and apply it.
    force.normalize();
    force *= -magnitude;
    particle->addForce(force);
} // ParticleBungee::updateForce

ParticleFakeSpring::ParticleFakeSpring(Vec3 *anchor, real springConstant, real damping)
: anchor(anchor), springConstant(springConstant), damping(damping)
{
}

void ParticleFakeSpring::updateForce(Particle *particle, real duration) {
    // Check for finite non-zero mass
    if(!particle->hasFiniteMass()) return;

    Vec3 vel = particle->getVelocity();

    // Calculate the position of the particle relative to the anchor
    Vec3 pos;
    particle->getPosition(&pos);
    pos -= *anchor;

    // Calculate constants and check that they are in bounds
    real gamma = 0.5f * real_sqrt(4 * springConstant - damping * damping);
    if(gamma == 0.0f) return;
    Vec3 c = pos * (damping / (2.0f * gamma)) + vel * (1.0f/gamma);

    // Calculate the target position
    Vec3 target = pos * real_cos(gamma * duration) + c * real_sin(gamma * duration);
    target *= real_exp(-0.5f * damping * duration);

    // Calculate the resulting acceleration (and therefore the force)
    Vec3 acc = (target - pos) * (1.0f / (duration * duration)) - vel * duration;
    particle->addForce(acc * particle->getMass());
} // void ParticleFakeSpring