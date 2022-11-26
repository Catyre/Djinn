/**
 * @file particle.cpp
 * @brief Define methods for the Particle class
 * @author Catyre
 * @date 11-10-2022
*/

#include <assert.h>
#include <sstream>
#include <limits>
#include "engine/particle.h"

using namespace engine;
using namespace std;

string Particle::toString() {
    stringstream ss;

    ss << scientific << "Position [m]:         |<" << this->pos.x << ", " << this->pos.y << ", " << this->pos.z << ">| = " << this->pos.magnitude() << endl <<
                        "Velocity [m/s]:       |<" << this->vel.x << ", " << this->vel.y << ", " << this->vel.z << ">| = " << this->vel.magnitude() << endl <<
                        "Acceleration [m/s^2]: |<" << this->acc.x << ", " << this->acc.y << ", " << this->acc.z << ">| = " << this->acc.magnitude() << endl <<
                        "Net force [N]:        |<" << this->getForceAccum().x << ", " << this->getForceAccum().y << ", " << this->getForceAccum().z << ">| = " << this->acc.magnitude() * this->getMass() << endl <<
                        "Kinetic Energy [J]: " << this->kineticEnergy() << endl;

    return ss.str();
}

void Particle::integrate(real duration) {
    // We won't integrate particles with infinite or negative mass
    if (inverseMass <= 0.0f) return;

    assert(duration > 0.0);

    //Vec3 resultingAcc = acc;
    //resultingAcc.addScaledVector(forceAccum, inverseMass);
    acc.addScaledVector(forceAccum, inverseMass);

    // Impose drag.
    vel *= real_pow(damping, duration);

    // Update linear velocity from the acceleration.
    //vel.addScaledVector(resultingAcc, duration);
    vel.addScaledVector(acc, duration);

    // Update linear position.
    pos.addScaledVector(vel, duration);
    pos.addScaledVector(acc, duration * duration * 0.5);

    // Clear forces
    clearAccumulator();
} // Particle::integrate()

real Particle::kineticEnergy() {
    return .5 * ((real)1.0)/inverseMass * real_pow(vel.magnitude(), 2);
}

// Set the mass (specfically the inverse mass) of the particle
void Particle::setMass(const real mass) {
    assert(mass != 0);
    Particle::inverseMass = ((real)1.0)/mass;
}

// Set the position of the particle to the given vector
void Particle::setPosition(const Vec3& pos) {
    Particle::pos = pos;
}

// Set the position of the particle given three reals representing the XYZ coordinates
void Particle::setPosition(const real x, const real y, const real z) {
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

// Return position of particle
Vec3 Particle::getPosition() const {
    return pos;
}

// Get the particle's velocity and set it equal to the given vector
void Particle::getPosition(Vec3* pos) {
    *pos = Particle::pos;
}

// Set the velocity of the particle to the given vector
void Particle::setVelocity(const Vec3& vel) {
    Particle::vel = vel;
}

// Set the velocity of the particle to the given XYZ values
void Particle::setVelocity(const real x, const real y, const real z) {
    vel.x = x;
    vel.y = y;
    vel.z = z;
}

// Return the velocity of the particle
Vec3 Particle::getVelocity() const {
    return vel;
}

// Get the particle's velocity and set it equal to the given vector
void Particle::getVelocity(Vec3* velocity) {
    *velocity = Particle::vel;
}

// Set the acceleration given a vector
void Particle::setAcceleration(const Vec3 &acc) {
    Particle::acc = acc;
}

void Particle::setAcceleration(const real x, const real y, const real z) {
    acc.x = x;
    acc.y = y;
    acc.z = z;
}

Vec3 Particle::getAcceleration() const {
    return acc;
}

real Particle::getMass() const {
    if (inverseMass == 0) {
        return std::numeric_limits<float>::max();
    } else {
        return ((real)1.0)/inverseMass;
    }
}

void Particle::clearAccumulator() {
    forceAccum.clear();
}

void Particle::addForce(const Vec3& f) {
    forceAccum += f;
}

bool Particle::hasFiniteMass() const {
    return inverseMass > 0.0f;
}

Vec3 Particle::getForceAccum() const {
    return forceAccum;
}