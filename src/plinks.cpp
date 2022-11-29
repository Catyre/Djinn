/**
 * @file plinks.cpp
 * @brief Simulate the orbit of the inner planets and jupiter around the sun
 * @author Catyre
*/

#include "djinn/plinks.h"

using namespace djinn;

real ParticleLink::currentLength() const {
    Vec3 relativePos = particles[0]->getPosition() -
    particles[1]->getPosition();
    return relativePos.magnitude();
}

unsigned ParticleCable::addContact(ParticleContact *contact, unsigned limit) const {
    // Find the length of the cable.
    real length = currentLength();

    // Check if we’re overextended.
    if (length < maxLength) {
        return 0;
    }

    // Otherwise, return the contact.
    contact->particles[0] = particles[0];
    contact->particles[1] = particles[1];

    // Calculate the normal.
    Vec3 normal = particles[1]->getPosition() - particles[0]->getPosition();
    normal.normalize();

    contact->contactNormal = normal;
    contact->penetration = length-maxLength;
    contact->restitution = restitution;

    return 1;
}

unsigned ParticleRod::addContact(ParticleContact *contact, unsigned limit) const {
    // Find the length of the rod.
    real currentLen = currentLength();

    // Check if we’re overextended.
    if (currentLen == length) {
        return 0;
    }

    // Otherwise, return the contact.
    contact->particles[0] = particles[0];
    contact->particles[1] = particles[1];

    // Calculate the normal.
    Vec3 normal = particles[1]->getPosition() - particles[0]->getPosition();
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