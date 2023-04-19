/**
 * @file thermo.cpp
 * @brief Demo of a gas reaching thermodynamic equilibrium
 * @author Catyre
 */

#include "djinn/particle.h"
#include "djinn/pfgen.h"
#include "djinn/potgen.h"
#include "raylib.h"
#include "rlFPCamera.h"
#include "rlHelper.h"
#include "rlgl.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <sstream>
#include <utility>

#define CAMERA_IMPLEMENTATION

typedef std::pair<djinn::Vec3, djinn::real> distance;
typedef std::vector<std::vector<distance>> distances;

// Some helpful functions for calculating random numbers on an interval
djinn::real randomReal() {
    return (djinn::real)(rand()) / (djinn::real)(RAND_MAX);
}

int randomInt(int a, int b) {
    if (a > b)
        return randomInt(b, a);
    if (a == b)
        return a;
    return a + (rand() % (b - a));
}

djinn::real randomReal(int a, int b) {
    if (a > b)
        return randomReal(b, a);
    if (a == b)
        return a;

    return (djinn::real)randomInt(a, b) + randomReal();
}

// Test boundary conditions
void checkBoundaries(djinn::Particle *particles, djinn::Vec3 bounds, int num_particles) {
    djinn::real upper_bound[3] = {bounds.x, bounds.y, bounds.z};

    for (int j = 0; j < num_particles; j++) {
        // Make some arrays to make checking each dimension easier (can do in one for loop)
        djinn::real pos[3] = {particles[j].getPosition().x,
                              particles[j].getPosition().y,
                              particles[j].getPosition().z};

        djinn::real vel[3] = {particles[j].getVelocity().x,
                              particles[j].getVelocity().y,
                              particles[j].getVelocity().z};

        // for (int i = 0; i < sizeof(pos) / sizeof(pos[0]); i++) {
        //     if (pos[i] >= upper_bound[i] || pos[i] <= 0) {
        //         vel[i] *= -1;
        //         particles[j].setVelocity(vel[0], vel[1], vel[2]);
        //     }
        // }

        if (pos[0] >= upper_bound[0] || pos[0] <= 0) {
            vel[0] *= -1;
        }

        if (pos[1] >= upper_bound[1] || pos[1] <= 0) {
            vel[1] *= -1;
        }

        if (pos[2] >= upper_bound[2] || pos[2] <= 0) {
            vel[2] *= -1;
        }

        particles[j].setVelocity(vel[0], vel[1], vel[2]);
    }
}

// Calculates the distances of each pair of particles
void calculateDistances(djinn::Particle particles[], distances &all_r, int num_particles) {
    // distances r;
    djinn::Vec3 p_i, p_j, r_vec;
    djinn::real r_mag, x, y, z;

    // Calculate distance between every pair of particles
    for (int i = 0; i < num_particles; i++) {
        p_i = particles[i].getPosition();

        // j < i is an optimization to fill only the bottom triangle of the 2x2 matrix
        //  (since r_ij = -r_ji)
        for (int j = 0; j < i; j++) {
            p_j = particles[j].getPosition();
            r_vec = p_i - p_j;
            r_mag = r_vec.magnitude();
            distance data = {r_vec, r_mag};

            all_r[i][j] = data;
        }
    }
}

int main() {
    // Set up logging
    try {
        auto logger = spdlog::basic_logger_mt("thermo", "logs/thermo.log", true);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 0;
    }

    // Raylib Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Djinn - Thermodynamics Demo");

    rlFPCamera cam;
    cam.Setup(45, Vector3{0, 2, 0});
    cam.MoveSpeed.z = 1.0;
    cam.MoveSpeed.x = 1.0;
    cam.MoveSpeed.y = 1.0;

    // SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Define some initial conditions
    srand(1000000);
    djinn::real dt = 1e-3;
    djinn::real dr = 1e-3;
    djinn::real sigma = 0.34e-9;   // Lennard-Jones parameter
    djinn::real epsilon = 0.38e-9; // Lennard-Jones parameter
    djinn::real potential;
    djinn::LennardJones lj = djinn::LennardJones(sigma, epsilon);
    distances all_r;

    djinn::PotentialRegistry u_reg;

    // Define boundary conditions
    djinn::Vec3 bounds = djinn::Vec3(1, 1, 1);
    Vector3 sq_center = bounds.normalize().toVector3();

    // Define a big array of particles
    int num_particles = 750;
    djinn::Particle particles[num_particles];
    // And an array for the raylib-based particle objects
    Vector3 rl_particles[num_particles];

    // Randomize each of their positions and add them to the potential registry
    for (int i = 0; i < num_particles; i++) {
        all_r.push_back(std::vector<distance>());

        for (int j = 0; j < i; j++) {
            all_r[i].push_back({djinn::Vec3{0, 0, 0}, 0.0});
        }

        particles[i].setPosition(randomReal(0, (int)bounds.x),
                                 randomReal(0, (int)bounds.y),
                                 randomReal(0, (int)bounds.z));

        u_reg.add(&particles[i], &lj);
    }

    if (num_particles == 1) {
        particles[0].setVelocity(randomReal(0, (int)bounds.x),
                                 randomReal(0, (int)bounds.y),
                                 randomReal(0, (int)bounds.z));
    }

    while (!WindowShouldClose()) {
        // Calculate new velocity according to Verlet Algorithm
        u_reg.integrateAll(dt);

        // Check boundaries and correct particles outside of them
        checkBoundaries(&particles[0], bounds, num_particles);

        // Calculate the distances of each pair of particles
        calculateDistances(particles, all_r, num_particles);

        // Update the force of each particle according to the Lennard-Jones
        // potential
        for (int i = 0; i < num_particles; i++) {
            for (int j = 0; j < all_r[i].size(); j++) {
                lj.updateForce(&particles[i], all_r[i][j].second, dr);
            }

            rl_particles[i] = particles[i].getPosition().toVector3();
        }

        cam.Update();
        BeginDrawing();
        ClearBackground(BLACK);

        cam.BeginMode3D();
        DrawGrid(100, 0.1f);

        DrawCubeWires(sq_center, 0.9, 0.9, 0.9, WHITE);

        for (int i = 0; i < num_particles; i++) {
            DrawSphere(rl_particles[i], 0.005, RED);
        }

        cam.EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    return 0;
}
