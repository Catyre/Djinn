/**
 * @file pcontacts.h
 * @brief Header file for the particle collision resolution
 * @author Catyre
*/

#ifndef PCONTACTS_H
#define PCONTACTS_H

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
        // The contact resolver object needs access into the contacts to set and effect the contact
        friend class ParticleContactResolver;

        public:
            /**
            * Holds the particles that are involved in the contact. The
            * second of these can be NULL for contacts with the scenery
            */
            Particle* particles[2];

            // Holds the normal restitution coefficient at the contact
            real restitution;

            // Holds the direction of the contact in world coordinates
            Vec3 contactNormal;

            // Holds the depth of penetration at the contact point
            real penetration;

            // Holds the amount each particle is moved by during interpenetration resolution
            Vec3 particleMovement[2];

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
    }; // class ParticleContact

    class ParticleContactResolver {
        protected:
            // Holds the number of iterations allowed
            unsigned iterations;

            // This is a performance tracking value - we keep a record
            // of the actual number of iterations used.
            unsigned iterationsUsed;

        public:
            // Creates a new contact resolver
            ParticleContactResolver(unsigned iterations);

            // Sets the number of iterations that can be used
            void setIterations(unsigned iterations);

            // Resolves a set of particle contacts for both penetration and velocity
            void resolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration);
    }; // class ParticleContactResolver

    /**
     * This is the basic polymorphic interface for contact generators
     * applying to particles.
     */
    class ParticleContactGenerator {
        public:
            /**
             * Fills the given contact structure with the generated
             * contact. The contact pointer should point to the first
             * available contact in a contact array, where limit is the
             * maximum number of contacts in the array that can be written
             * to. The method returns the number of contacts that have
             * been written.
             */
            virtual unsigned addContact(ParticleContact *contact, unsigned limit) const = 0;
    }; // class ParticleContactGenerator
};

#endif // PCONTACTS_H