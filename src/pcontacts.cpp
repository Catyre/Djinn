/**
 * @file pcontacts.cpp
 * @brief Header file for the Particle class
 * @author Catyre
*/

#include "pcontacts.h"

using namespace djinn;

void ParticleContact::resolve(real duration) {
    resolveVelocity(duration);
    resolveInterpenetration(duration);
} // ParticleContact::resolve

real ParticleContact::calculateSeparatingVelocity() const {
    Vector3 relativeVelocity = particles[0]->getVelocity();
    if (particles[1]) relativeVelocity -= particles[1]->getVelocity();
    return relativeVelocity * contactNormal;
} // ParticleContact::calculateSeparatingVelocity

void ParticleContact::resolveVelocity(real duration) {
    // Find the velocity in the direction of the contact
    real separatingVelocity = calculateSeparatingVelocity();

    // Check if it needs to be resolved
    if (separatingVelocity > 0) {
        // The contact is either separating, or stationary - there's
        // no impulse required.
        return;
    }

    // Calculate the new separating velocity
    real newSepVelocity = -separatingVelocity * restitution;
    real deltaVelocity = newSepVelocity - separatingVelocity;

    // We apply the change in velocity to each object in proportion to
    // their inverse mass (i.e. those with lower inverse mass [higher
    // actual mass] get less change in velocity)..
    real totalInverseMass = particles[0]->getInverseMass();
    if (particles[1]) totalInverseMass += particles[1]->getInverseMass();

    // If all particles have infinite mass, then impulses have no effect
    if (totalInverseMass <= 0) return;

    // Calculate the impulse to apply
    real impulse = deltaVelocity / totalInverseMass;

    // Find the amount of impulse per unit of inverse mass
    Vector3 impulsePerIMass = contactNormal * impulse;

    // Apply impulses: they are applied in the direction of the contact,
    // and are proportional to the inverse mass.
    particles[0]->setVelocity(particles[0]->getVelocity() + impulsePerIMass * particles[0]->getInverseMass());

    if (particles[1]) {
        // Particle 1 goes in the opposite direction
        particles[1]->setVelocity(particles[1]->getVelocity() + impulsePerIMass * -particles[1]->getInverseMass());
    }
} // void ParticleContact::resolveVelocity

void ParticleContact::resolveInterpenetration(real duration) {
    // If we don't have any penetration, skip this step
    if (penetration <= 0) return;

    // The movement of each object is based on their inverse mass, so
    // total that.
    real totalInverseMass = particles[0]->getInverseMass();
    if (particles[1]) totalInverseMass += particles[1]->getInverseMass();

    // If all particles have infinite mass, then we do nothing
    if (totalInverseMass <= 0) return;

    // Find the amount of penetration resolution per unit of inverse mass
    Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);

    // Calculate the the movement amounts
    particleMovement[0] = movePerIMass * particles[0]->getInverseMass();

    if (particles[1]) {
        particleMovement[1] = movePerIMass * -particles[1]->getInverseMass();
    } else {
        particleMovement[1].clear();
    }

    // Apply the penetration resolution
    particles[0]->setPosition(particles[0]->getPosition() + particleMovement[0]);
    if (particles[1]) {
        particles[1]->setPosition(particles[1]->getPosition() + particleMovement[1]);
    }
}