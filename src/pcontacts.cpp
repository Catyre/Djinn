/**
 * @file pcontacts.cpp
 * @brief .cpp file for the particle contacts class, which handles and resolves collisions between particles
 * @author Catyre
*/

#include "djinn/pcontacts.h"

void djinn::ParticleContact::resolve(djinn::real duration) {
    resolveVelocity(duration);
    resolveInterpenetration(duration);
} // ParticleContact::resolve

djinn::real djinn::ParticleContact::calculateSeparatingVelocity() const {
    djinn::Vec3 relativeVelocity = particles[0]->getVelocity();
    if (particles[1]) relativeVelocity -= particles[1]->getVelocity();
    return relativeVelocity * contactNormal;
} // ParticleContact::calculateSeparatingVelocity

void djinn::ParticleContact::resolveVelocity(djinn::real duration) {
    // Find the velocity in the direction of the contact
    djinn::real separatingVelocity = calculateSeparatingVelocity();

    // Check if it needs to be resolved
    if (separatingVelocity > 0) {
        // The contact is either separating, or stationary - there's
        // no impulse required.
        return;
    }

    // Calculate the new separating velocity
    djinn::real newSepVelocity = -separatingVelocity * restitution;

    // Check the velocity buildup due to acceleration only.
    djinn::Vec3 accCausedVelocity = particles[0]->getAcceleration();
    if (particles[1]) accCausedVelocity -= particles[1]->getAcceleration();
    djinn::real accCausedSepVelocity = accCausedVelocity * contactNormal * duration;

    // If we’ve got a closing velocity due to aceleration buildup,
    // remove it from the new separating velocity.
    if (accCausedSepVelocity < 0) {
        newSepVelocity += restitution * accCausedSepVelocity;
        // Make sure we haven’t removed more than was
        // there to remove.
        if (newSepVelocity < 0) newSepVelocity = 0;
    }

    real deltaVelocity = newSepVelocity - separatingVelocity;

    // We apply the change in velocity to each object in proportion to
    // their inverse mass (i.e. those with lower inverse mass [higher
    // actual mass] get less change in velocity)..
    real totalInverseMass = particles[0]->getInverseMass();
    if (particles[1]) totalInverseMass += particles[1]->getInverseMass();

    // If all particles have infinite mass, then impulses have no effect
    if (totalInverseMass <= 0) return;

    // Calculate the impulse to apply
    djinn::real impulse = deltaVelocity / totalInverseMass;

    // Find the amount of impulse per unit of inverse mass
    djinn::Vec3 impulsePerIMass = contactNormal * impulse;

    // Apply impulses: they are applied in the direction of the contact,
    // and are proportional to the inverse mass.
    particles[0]->setVelocity(particles[0]->getVelocity() + impulsePerIMass * particles[0]->getInverseMass());

    if (particles[1]) {
        // Particle 1 goes in the opposite direction
        particles[1]->setVelocity(particles[1]->getVelocity() + impulsePerIMass * -particles[1]->getInverseMass());
    }
} // void ParticleContact::resolveVelocity

void djinn::ParticleContact::resolveInterpenetration(djinn::real duration) {
    // If we don't have any penetration, skip this step
    if (penetration <= 0) return;

    // The movement of each object is based on their inverse mass, so
    // total that.
    djinn::real totalInverseMass = particles[0]->getInverseMass();
    if (particles[1]) totalInverseMass += particles[1]->getInverseMass();

    // If all particles have infinite mass, then we do nothing
    if (totalInverseMass <= 0) return;

    // Find the amount of penetration resolution per unit of inverse mass
    djinn::Vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

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
} // void ParticleContact::resolveInterpenetration

djinn::ParticleContactResolver::ParticleContactResolver(unsigned iterations) : iterations(iterations)
{
}

void djinn::ParticleContactResolver::setIterations(unsigned iterations) {
    djinn::ParticleContactResolver::iterations = iterations;
} // void ParticleContactResolver::setIterations

void djinn::ParticleContactResolver::resolveContacts(djinn::ParticleContact *particleArray, unsigned numContacts, djinn::real duration) {
    unsigned i;

    iterationsUsed = 0;
    while (iterationsUsed < iterations) {
        // Find the contact with the largest closing velocity;
        djinn::real max = REAL_MAX;
        unsigned maxIndex = numContacts;
        for (i = 0; i < numContacts; i++) {
            djinn::real sepVel = particleArray[i].calculateSeparatingVelocity();
            if (sepVel < max && (sepVel < 0 || particleArray[i].penetration > 0)) {
                max = sepVel;
                maxIndex = i;
            }
        }

        // Do we have anything worth resolving?
        if (maxIndex == numContacts) break;

        // Resolve this contact
        particleArray[maxIndex].resolve(duration);

        // Update the interpenetrations for all particles
        djinn::Vec3 *move = particleArray[maxIndex].particleMovement;
        for (i = 0; i < numContacts; i++) {
            if (particleArray[i].particles[0] == particleArray[maxIndex].particles[0]) {
                particleArray[i].penetration -= move[0] * particleArray[i].contactNormal;
            } else if (particleArray[i].particles[0] == particleArray[maxIndex].particles[1]) {
                particleArray[i].penetration -= move[1] * particleArray[i].contactNormal;
            }

            if (particleArray[i].particles[1]) {
                if (particleArray[i].particles[1] == particleArray[maxIndex].particles[0]) {
                    particleArray[i].penetration += move[0] * particleArray[i].contactNormal;
                } else if (particleArray[i].particles[1] == particleArray[maxIndex].particles[1]) {
                    particleArray[i].penetration += move[1] * particleArray[i].contactNormal;
                }
            }
        }

        iterationsUsed++;
    }
} // void ParticleContactResolver::resolveContacts