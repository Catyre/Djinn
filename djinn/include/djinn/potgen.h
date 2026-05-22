/**
 * @file potgen.h
 * @brief Header file for potential generators
 * @author Catyre
 */

#ifndef POTGEN_H
#define POTGEN_H

#include "core.h"
#include "djinn/particle.h"
#include "djinn/pfgen.h"
#include <vector>

namespace djinn {
    class PotentialGenerator {
        public:
            // "var" is a generic variable representing whatever quantity is
            //      needed to calculate the potential (position/distance, time, etc.)
            virtual void updatePotential(Particle *particle, real var) = 0;
            virtual void updateForce(Particle *particle, Vec3 r_vec, real r_mag, real dvar) = 0;
    }; // class PotentialGenerator

    class PotentialRegistry {
        protected:
            // Keep track of potential generator and the particle that called
            // for it
            struct PotentialRegistration {
                    Particle *particle;
                    PotentialGenerator *pg;

                    bool operator==(const PotentialRegistration &other) const {
                        return particle == other.particle;
                    }
            };

            // Holds list of registrations
            typedef std::vector<PotentialRegistration> Registry;
            Registry registrations;

        public:
            PotentialRegistry() {
                ParticleForceRegistry force_registry;
            }
            // Registers the given force generator to apply to the given
            // particle
            void add(Particle *particle, PotentialGenerator *pg);

            // Integrate all potentials in the registry
            void integrateAll(real duration);

            // Removes given registered pair from registry
            void remove(Particle *particle, PotentialGenerator *pg);

            // Clear all registrations from the registry
            void clear();

            // Calls all the potential generators to update the forces of their
            // corresponding particles
            void updatePotentials(real duration);

    }; // class PotentialRegistry

    class LennardJones : public PotentialGenerator {
            // Lennard-Jones parameter (depth of well), Lennard-Jones parameter (null potential)
            // Originially 0.34e-9, 0.38e-9
            real sig;   // Lennard-Jones parameter
            real eps; // Lennard-Jones parameter

        public:
            LennardJones(real sigma, real epsilon) : sig(sigma), eps(epsilon) {};

            virtual void updatePotential(Particle *particle, real var);
            virtual void updateForce(Particle *particle, Vec3 r_vec, real r_mag, real dvar);

        private:
            real sigma;
            real epsilon;
    }; // class LennardJones
} // namespace djinn

#endif // POTGEN_H
