/**
 * @file plinks.h
 * @brief Header file for the particle links .cpp, which handles collisions between linked particles
 * @author Catyre
 * @date 11-29-2022
*/

#ifndef PLINKS_H
#define PLINKS_H

#include "core.h"
#include "particle.h"

namespace djinn {
    /**
    * Links connect two particles together, generating a contact if
    * they violate the constraints of their link. It is used as a
    * base class for cables and rods, and could be used as a base
    * class for springs with a limit to their extension.
    */
    class ParticleLink : public ParticleContactGenerator {
        public:
            // Holds the pair of particles that are connected by this link
            Particle* particle[2];

        protected:
            // Returns the current length of the link
            real currentLength() const;

        public:
            /*
            * Generates the contacts to keep this link from being
            * violated. This class can only ever generate a single
            * contact, so the pointer can be a pointer to a single
            * element, the limit parameter is assumed to be at least 1
            * (0 isn’t valid), and the return value is 0 if the
            * cable wasn’t overextended, or 1 if a contact was needed.
            *
            * NB: This method is declared in the same way (as pure
            * virtual) in the parent class, but is replicated here for
            * documentation purposes.
            */

            virtual unsigned addContact(ParticleContact *contact, unsigned limit) const = 0;
    }; // class ParticleLink

    /**
    * Rods link a pair of particles, generating a contact if they
    * stray too far apart or too close.
    */
    class ParticleRod : public ParticleLink {
        public:
            /**
            * Holds the length of the rod.
            */
            real length;

        public:
            /**
            * Fills the given contact structure with the contact needed
            * to keep the rod from extending or compressing.
            */
           virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;
    }; // class ParticleRod
};

#endif