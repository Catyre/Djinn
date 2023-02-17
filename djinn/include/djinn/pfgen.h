/**
 * @file pfgen.h
 * @brief Header file for the particle force generators
 * @author Catyre
*/

#ifndef PFGEN_H
#define PFGEN_H

#include "core.h"
#include "particle.h"
#include <vector>

namespace djinn {
    class ParticleForceGenerator {
        public:
            virtual void updateForce(Particle *particle, real duration) = 0;
    };

    // To be used for forces that apply universally to all particles in the system (gravity or electromagnetism, for example)
    class ParticleUniversalForceRegistry {
        protected:
            struct ParticleUniversalForceRegistration {
                Particle *particle;

                bool operator==(const ParticleUniversalForceRegistration& other) const {
                    return particle == other.particle;
                }
            };

            typedef std::vector<ParticleUniversalForceRegistration> Registry;
            Registry registrations;

        public:
            void add(Particle *particle);

            void add(std::vector<Particle*> particles);

            void applyGravity();

            void clear();

            void integrateAll(real duration);

            void remove(Particle *particle);
    }; // class ParticleUniversalForceRegistry

    class ParticleForceRegistry {
        protected:
            // Keep track of force generator and the particle that called for it
            struct ParticleForceRegistration {
                Particle *particle;
                ParticleForceGenerator *fg;

                bool operator==(const ParticleForceRegistration& other) const{
                    return particle == other.particle;
                }
            };

            // Holds list of registrations
            typedef std::vector<ParticleForceRegistration> Registry;
            Registry registrations;

        public:
            // Registers the given force generator to apply to the given particle
            void add(Particle* particle, ParticleForceGenerator *fg);

            void integrateAll(real duration);

            // Removes given registered pair from registry
            void remove(Particle* particle, ParticleForceGenerator *fg);

            // Clear all registrations from the registry
            void clear();

            // Calls all the force generators to update the forces of their corresponding particles
            void updateForces(real duration);
    }; // class ParticleForceRegistry

    // A force generator that applies a gravitational force.  One instance can be used for
    //      multiple particles
    class ParticleEarthGravity : public ParticleForceGenerator {
        Vec3 gravity;

        public:
            // Create generator with given acceleration
            ParticleEarthGravity(const Vec3& gravity);

            // Applies the gravitational force to the given particle
            virtual void updateForce(Particle* particle, real duration);
    }; // class ParticleEarthGravity

    class ParticlePointGravity : public ParticleForceGenerator {
        Vec3 origin;
        real mass;

        public:
            ParticlePointGravity(const Vec3& origin, const real mass);

            void setOrigin(const Vec3& origin);

            void applyGravBodies();

            virtual void updateForce(Particle* particle, real duration);
    }; // class ParticlePointGravity

    // Force generator to apply drag force
    class ParticleDrag : public ParticleForceGenerator {
        // Hold velocity drag coefficient
        real k1;

        // Hold velocity squared drag coefficient
        real k2;

        public:
            ParticleDrag(real k1, real k2);

            virtual void updateForce(Particle* particle, real duration);
    }; // class ParticleDrag

    class ParticleUplift : public ParticleForceGenerator {
        Vec3 origin;
        real radius;

        public:
            ParticleUplift(Vec3 origin, real radius);

            virtual void updateForce(Particle* particle, real duration);
    }; // class ParticleUplift

    class ParticleSpring : public ParticleForceGenerator {
        // Holds the other end of the spring
        Particle *other;

        // Holds the spring constant
        real springConstant;

        // Holds the rest length of the spring
        real restLength;

        public:
            // Creates a new spring with the given parameters
            ParticleSpring(Particle *other, real springConstant, real restLength);

            virtual void updateForce(Particle* particle, real duration);

            real calcCritDamping(real mass);
    }; // class ParticleSpring

    class ParticleAnchoredSpring : public ParticleForceGenerator {
        // Holds the location of the anchored end of the spring
        Vec3 *anchor;

        // Holds the spring constant
        real springConstant;

        // Holds the rest length of the spring
        real restLength;

        // Holds the limit of elasticity
        real elasticLimit;

        public:
            // Creates a new spring with the given parameters
            ParticleAnchoredSpring(Vec3 *anchor, real springConstant, real restLength, real elasticLimit);

            virtual void updateForce(Particle* particle, real duration);
    }; // class ParticleAnchoredSpring

    class ParticleBungee : public ParticleForceGenerator {
        // Holds the other end of the bungee
        Particle *other;
        
        // Holds how springy the bungee is
        real springConstant;

        // Keep track of rest length
        real restLength;

        public:
            ParticleBungee(Particle *other, real springConstant, real restLength);

            virtual void updateForce(Particle *particle, real duration);
    }; // class ParticleBungee

    class ParticleFakeSpring : public ParticleForceGenerator {
        // The location of the anchored end of the spring
        Vec3 *anchor;

        // Holds the spring constant
        real springConstant;

        // Holds the damping of the oscillations of the spring
        real damping;

        public:
            ParticleFakeSpring(Vec3 *anchor, real springConstant, real damping);

            virtual void updateForce(Particle *particle, real duration);
    }; // class ParticleFakeSpring
}; // namespace djinn

#endif // PFGEN_H
