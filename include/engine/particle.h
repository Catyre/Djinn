/**
 * @file particle.h
 * @brief Header file for the Particle class
 * @author Catyre
*/

#ifndef PARTICLE_H
#define PARTICLE_H

#include "core.h"
#include <iostream>

namespace engine{
    class Particle {
        protected:
            // In some systems it is useful to name the particles
            std::string name;

            // Keep track of position and its time derivatives
            Vec3 pos;
            Vec3 vel;
            Vec3 acc;

            // Keep track of damping of linear motion
            real damping;

            /**
             * Holds the inverse of the mass of the particle. It
             * is more useful to hold the inverse mass because
             * integration is simpler, and because in real-time
             * simulation it is more useful to have objects with
             * infinite mass (immovable) than zero mass
             * (completely unstable in numerical simulation).
            */
            real inverseMass;

            Vec3 netForce;

        public:
            Particle() : pos(0, 0, 0), vel(0, 0, 0), acc(0, 0, 0), damping((real)1.0), inverseMass(0), name("") {};

            Particle(const Vec3 pos, 
                     const Vec3 vel, 
                     const Vec3 acc, 
                     const real damping, 
                     const real inverseMass,
                     const std::string name = "") : pos(pos), vel(vel), acc(acc), damping(damping), inverseMass(inverseMass), name(name) {};

            std::string toString();

            void integrate(real duration);

            real kineticEnergy();

            // Getters and setters for various physical quantities

            std::string getName() const;
            
            void setMass(const real mass);

            real getMass() const;

            void setPosition(const Vec3& pos);

            void setPosition(const real x, const real y, const real z);

            Vec3 getPosition() const;
            
            void getPosition(Vec3* pos);

            void setVelocity(const Vec3& vel);

            void setVelocity(const real x, const real y, const real z);

            Vec3 getVelocity() const;

            void getVelocity(Vec3* velocity);

            void setAcceleration(const Vec3& acc);

            void setAcceleration(const real x, const real y, const real z);

            Vec3 getAcceleration() const;

            void clearAccumulator();

            void addForce(const Vec3& f);

            Vec3 getNetForce() const;

            bool hasFiniteMass() const;

            // If the two particles have the saem time derivatives, damping factor, and mass, we assume them to be the same particle
            bool operator==(const Particle& p) const {
                return (pos == p.pos) && (vel == p.vel) && (acc == p.acc) && (real_abs(damping - p.damping) < EPSILON) && (real_abs(inverseMass - p.inverseMass) < EPSILON);
            }

            bool operator != (const Particle& p) {
                return !(*this == p);
            }
    }; // class Particle
}; // namespace engine

#endif // PARTICLE_H