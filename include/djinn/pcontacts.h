/**
 * @file pcontacts.h
 * @brief Header file for the particle collision resolution
 * @author Catyre
*/

#ifndef PCONTACTS_H
#define PCONTACTS_H

#include "core.h"
#include "particle.h"

namespace djinn {
    /**
    * A contact represents two objects in contact (in this case
    * ParticleContact representing two particles). Resolving a
    * contact removes their interpenetration, and applies sufficient
    * impulse to keep them apart. Colliding bodies may also rebound.
    *
    * The contact has no callable functions, it just holds the
    * contact details. To resolve a set of contacts, use the particle
    * contact resolver class.
    */
    class ParticleContact {
        public:
            /**
            * Holds the particles that are involved in the contact. The
            * second of these can be NULL for contacts with the scenery
            */
            Particle* particles[2];

            // Holds the normal restitution coefficient at the contact
            real restitution;

            // Holds the direction of the contact in world coordinates
            Vector3 contactNormal;

            // Holds the depth of penetration at the contact point
            real penetration;

        protected:
            // Resolves this contact for both velocity and interpenetration
            void resolve(real duration);

            // Calculates the separating velocity at this contact
            real calculateSeparatingVelocity() const;

        private:
            // Handles the impulse calculations for this collision
            void resolveVelocity(real duration);

            // Handles the interpenetration resolution for this contact
            void resolveInterpenetration(real duration);
    };
};

#endif