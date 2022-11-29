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
}

real ParticleContact::calculateSeparatingVelocity() const {
    Vector3 relativeVelocity = particle[0]->getVelocity();
    if (particle[1]) relativeVelocity -= particle[1]->getVelocity();
    return relativeVelocity * contactNormal;
}

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
    real totalInverseMass = particle[0]->getInverseMass();
    if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

    // If all particles have infinite mass, then impulses have no effect
    if (totalInverseMass <= 0) return;

    // Calculate the impulse to apply
    real impulse = deltaVelocity / totalInverseMass;

    // Find the amount of impulse per unit of inverse mass
    Vector3 impulsePerIMass = contactNormal * impulse;

    // Apply impulses: they are applied in the direction of the contact,
    // and are proportional to the inverse mass.
    particle[0]->setVelocity(particle[0]->getVelocity() + impulsePerIMass * particle[0]->getInverseMass());

    if (particle[1]) {
        // Particle 1 goes in the opposite direction
        particle[1]->setVelocity(particle[1]->getVelocity() + impulsePerIMass * -particle[1]->getInverseMass());
    }
}