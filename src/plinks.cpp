/**
 * @file plinks.cpp
 * @brief Define methods for creating contacts between djinn::Particles that link them together 
 *          (rods, links, cords, etc.)
 * @author Catyre
*/

#include "djinn/plinks.h"

djinn::real djinn::ParticleLink::currentLength() const {
    djinn::Vec3 relativePos = particles[0]->getPosition() -
    particles[1]->getPosition();
    return relativePos.magnitude();
}

unsigned djinn::ParticleCable::addContact(djinn::ParticleContact *contact, unsigned limit) const {
    // Find the length of the cable.
    djinn::real length = currentLength();

    // Check if we’re overextended.
    if (length < maxLength) {
        return 0;
    }

    // Otherwise, return the contact.
    contact->particles[0] = particles[0];
    contact->particles[1] = particles[1];

    // Calculate the normal.
    djinn::Vec3 normal = particles[1]->getPosition() - particles[0]->getPosition();
    normal.normalize();

    contact->contactNormal = normal;
    contact->penetration = length-maxLength;
    contact->restitution = restitution;

    return 1;
}

unsigned djinn::ParticleRod::addContact(djinn::ParticleContact *contact, unsigned limit) const {
    // Find the length of the rod.
    djinn::real currentLen = currentLength();

    // Check if we’re overextended.
    if (currentLen == length) {
        return 0;
    }

    // Otherwise, return the contact.
    contact->particles[0] = particles[0];
    contact->particles[1] = particles[1];

    // Calculate the normal.
    djinn::Vec3 normal = particles[1]->getPosition() - particles[0]->getPosition();
    normal.normalize();

    // The contact normal depends on whether we’re extending or compressing.
    if (currentLen > length) {
        contact->contactNormal = normal;
        contact->penetration = currentLen - length;
    } else {
        contact->contactNormal = normal * -1;
        contact->penetration = length - currentLen;
    }

    // Always use zero restitution (no bounciness).
    contact->restitution = 0;
    return 1;
}