/**
 * @file numerical.h
 * @brief Header file for numerical methods
 * @author Catyre
 */

#ifndef NUMERICAL_H
#define NUMERICAL_H

#include "core.h"

namespace djinn {
    // Type aliases for readability
    using ODE = std::function<Vec3(Vec3, real)>;

    // Function declarations
    Vec3 rungeKutta4(const ODE &func, Vec3 &initial, const real t, const real dt);

    // Loup Verlet algorithm
    void verletAlgorithm(Vec3 &x, Vec3 &v, Vec3 a, real dt);
} // namespace djinn

#endif // NUMERICAL_H
