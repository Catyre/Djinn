/**
 * @file numerical.cpp
 * @brief Define numerical methods for first order ODEs
 * @author Catyre
 */

#include "djinn/numerical.h"
#include <array>
#include <iostream>

// Reminder: "using ODE = std::function<Vec3(Vec3, real)>;" - djinn/numerical.h
djinn::Vec3 djinn::rungeKutta4(const djinn::ODE &func, djinn::Vec3 &initial, const djinn::real t, const djinn::real dt) {
    // Array to store the k-values (k[1-4])
    djinn::Vec3 k[4];

    // Update k1
    k[0] = func(initial, t) * dt;

    // Update k[2-4]
    for (int i = 1; i < std::end(k) - std::begin(k); i++)
        k[i] = func(initial + k[i - 1] * 0.5, t + 0.5 * dt) * dt;

    // Return the k values weighted according to RK4
    return initial + (k[0] + k[1] * 2 + k[2] * 2 + k[3]) * (1.0 / 6.0);
}

/*
 * Example RK4 implementation
 * --------------------------
 * #include "djinn/numerical.h"
 * #include "math.h"
 * #include <iostream>
 *
 * // Generic function (f') to integrate via RK4
 * djinn::Vec3 func(djinn::Vec3 pos, djinn::real t) {
 *     pos.x = exp(t);
 *     pos.y = exp(t);
 *     pos.z = exp(t);
 *
 *     return pos;
 * }
 *
 * int main() {
 *     // Define initial conditions, timesteps, etc.
 *     djinn::Vec3 pos = djinn::Vec3(0.0, 0.0, 0.0);
 *     const djinn::real t0 = 0.0;
 *     const djinn::real dt = 0.1;
 *     const djinn::real tf = 10.0;
 *
 *     // Integrate
 *     for (djinn::real t = t0; t <= tf; t += dt) {
 *         // Output data (can be piped to a file for analysis)
 *         std::cout << t << "," << pos.x << "," << pos.y << "," << pos.z << std::endl;
 *
 *         // Update position according to RK4
 *         pos = djinn::rungeKutta4(func, pos, t, dt);
 *     }
 *
 *     return 0;
 * }
 */

void djinn::verletAlgorithm(djinn::Vec3 &x, djinn::Vec3 &v, djinn::Vec3 a, djinn::real dt) {
    djinn::Vec3 xNew = x + v * dt + a * 0.5 * dt * dt;
    djinn::Vec3 vNew = v + (a + a) * 0.5 * dt; // Assuming constant force (constant acceleration)

    x = xNew;
    v = vNew;
}

/*
 * Example verlet algorithm
 * ------------------------
 * #include <iostream>
 * #include <cmath>
 *
 * // Constants
 * const double mass = 1.0; // Mass of the particle (kg)
 * const double gravity = -9.81; // Gravitational acceleration (m/s^2)
 * const double timeStep = 0.01; // Time step for the simulation (s)
 * const int numSteps = 1000; // Number of simulation steps
 *                            //
 * int main() {
 *     double x = 10.0; // Initial position (m)
 *     double v = 0.0; // Initial velocity (m/s)
 *
 *     std::cout << "Initial position: " << x << " m" << std::endl;
 *     std::cout << "Initial velocity: " << v << " m/s" << std::endl;
 *
 *     for (int step = 0; step < numSteps; ++step) {
 *         double acceleration = gravity; // Constant force
 *         verletAlgorithm(x, v, acceleration, timeStep);
 *
 *         std::cout << "Step: " << step + 1 << ", Position: " << x << " m, Velocity: " << v << " m/s" << std::endl;
 *     }
 *
 *     return 0;
 * }
 */
