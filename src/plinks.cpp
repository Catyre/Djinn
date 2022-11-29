/**
 * @file plinks.cpp
 * @brief Simulate the orbit of the inner planets and jupiter around the sun
 * @author Catyre
 * @date 11-29-2022
*/

#include "djinn/plinks.h"

using namespace djinn;

real ParticleLink::currentLength() const {
    Vec3 relativePos = particle[0]->getPosition() -
    particle[1]->getPosition();
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
    contact->particle[0] = particle[0];
    contact->particle[1] = particle[1];

    // Calculate the normal.
    Vec3 normal = particle[1]->getPosition() - particle[0]->getPosition();
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
    contact->particle[0] = particle[0];
    contact->particle[1] = particle[1];

    // Calculate the normal.
    Vec3 normal = particle[1]->getPosition() - particle[0]->getPosition();
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