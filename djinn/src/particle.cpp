/**
 * @file particle.cpp
 * @brief Define methods for the Particle class
 * @author Catyre
 */

#include "djinn/particle.h"
#include "djinn/numerical.h"
#include "spdlog/spdlog.h"
#include <assert.h>
#include <limits>
#include <sstream>

std::string djinn::Particle::toString() {
    std::stringstream ss;

    if (!this->name.empty()) {
        ss << this->name << ": " << std::endl;
    }

    ss << std::scientific << "Position [m]:         |" << this->pos.toString() << "| = " << this->pos.magnitude() << std::endl
       << "Velocity [m/s]:       |" << this->vel.toString() << "| = " << this->vel.magnitude() << std::endl
       << "Acceleration [m/s^2]: |" << this->acc.toString() << "| = " << this->acc.magnitude() << std::endl
       << "Net force [N]:        |" << this->netForce.toString() << "| = " << this->netForce.magnitude() << std::endl
       << "Kinetic Energy [J]: " << this->kineticEnergy() << std::endl;

    return ss.str();
}

void djinn::Particle::integrate(djinn::real dt) {
    // We won't integrate particles with infinite or negative mass
    if (inverseMass <= 0.0)
        return;

    assert(dt > 0.0);

    djinn::Vec3 pos0 = pos;
    acc.addScaledVector(netForce, inverseMass);

    djinn::verletAlgorithm(pos, vel, acc, dt);

    djinn::Vec3 delta_x = pos - pos0;

    clearNetForce();
    clearNetPotential();
    acc = djinn::Vec3();

    spdlog::info("Particle \"{}\" integrated and forces/acceleration cleared (Δx = {})", this->name, delta_x.toString());
}

djinn::real djinn::Particle::kineticEnergy() {
    return .5 * ((real)1.0) / inverseMass * real_pow(vel.magnitude(), 2);
}

std::string djinn::Particle::getName() const {
    return name;
}

// Set the mass (specfically the inverse mass) of the particle
void djinn::Particle::setMass(const djinn::real mass) {
    assert(mass != 0);
    Particle::inverseMass = ((real)1.0) / mass;
}

// Set the position of the particle to the given vector
void djinn::Particle::setPosition(const djinn::Vec3 &pos) {
    Particle::pos = pos;
}

// Set the position of the particle given three reals representing the XYZ coordinates
void djinn::Particle::setPosition(const djinn::real x, const djinn::real y, const djinn::real z) {
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

// Return position of particle
djinn::Vec3 djinn::Particle::getPosition() const {
    return pos;
}

// Get the particle's position and set it equal to the given vector
void djinn::Particle::getPosition(djinn::Vec3 *pos) {
    *pos = djinn::Particle::pos;
}

// Set the velocity of the particle to the given vector
void djinn::Particle::setVelocity(const djinn::Vec3 &vel) {
    djinn::Particle::vel = vel;
}

// Set the velocity of the particle to the given XYZ values
void djinn::Particle::setVelocity(const djinn::real x, const djinn::real y, const djinn::real z) {
    vel.x = x;
    vel.y = y;
    vel.z = z;
}

// Return the velocity of the particle
djinn::Vec3 djinn::Particle::getVelocity() const {
    return vel;
}

// Get the particle's velocity and set it equal to the given vector
void djinn::Particle::getVelocity(djinn::Vec3 *velocity) {
    *velocity = Particle::vel;
}

// Set the acceleration given a vector
void djinn::Particle::setAcceleration(const djinn::Vec3 &acc) {
    Particle::acc = acc;
}

void djinn::Particle::setAcceleration(const djinn::real x, const djinn::real y, const djinn::real z) {
    acc.x = x;
    acc.y = y;
    acc.z = z;
}

djinn::Vec3 djinn::Particle::getAcceleration() const {
    return acc;
}

djinn::real djinn::Particle::getMass() const {
    if (inverseMass == 0) {
#ifdef DOUBLE_PRECISION
        return std::numeric_limits<double>::max();
#else
        return std::numeric_limits<float>::max();
#endif
    } else {
        return ((real)1.0) / inverseMass;
    }
}

djinn::real djinn::Particle::getInverseMass() const {
    return inverseMass;
}

void djinn::Particle::clearNetForce() {
    netForce.clear();
}

void djinn::Particle::clearNetPotential() {
    netPotential = 0;
}

void djinn::Particle::addForce(const djinn::Vec3 &f) {
    netForce += f;
}

void djinn::Particle::addPotential(const djinn::real potential) {
    netPotential += potential;
}

bool djinn::Particle::hasFiniteMass() const {
    return inverseMass > 0.0;
}

djinn::Vec3 djinn::Particle::getNetForce() const {
    return netForce;
}
