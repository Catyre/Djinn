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
            virtual void updateForce(Particle *particle, real var, real dvar) = 0;
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
            real sigma, epsilon;

        public:
            LennardJones(real sigma, real epsilon);

            virtual void updatePotential(Particle *particle, real var);
            virtual void updateForce(Particle *particle, real var, real dvar);
    }; // class LennardJones
} // namespace djinn

#endif // POTGEN_H
