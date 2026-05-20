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
#include <deque>

#define CAMERA_IMPLEMENTATION

djinn::real dt = 6e-4;
djinn::real dr = 1e-3;
djinn::real potential;
djinn::real sigma = 0.34;   // Lennard-Jones parameter
djinn::real epsilon = 0.38; // Lennard-Jones parameter
djinn::LennardJones lj = djinn::LennardJones(sigma, epsilon);

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

djinn::real randomReal(djinn::real a, djinn::real b) {
    if (a > b)
        return randomReal(b, a);
    if (a == b)
        return a;

    return (djinn::real)randomInt(a, b) + randomReal();
}
void checkBoundaries(djinn::Particle *particles, djinn::Vec3 bounds, int num_particles) {
    for (int j = 0; j < num_particles; j++) {
        djinn::Vec3 pos = particles[j].getPosition();
        djinn::Vec3 vel = particles[j].getVelocity();

        if (pos.x >= bounds.x || pos.x <= 0) vel.x *= -1;
        if (pos.y >= bounds.y || pos.y <= 0) vel.y *= -1;
        if (pos.z >= bounds.z || pos.z <= 0) vel.z *= -1;

        particles[j].setVelocity(vel.x, vel.y, vel.z);
    }
}

// Calculates Lennard-Jones force of each pair of particles
void calculateLJ(djinn::Particle particles[], int num_particles) {
    djinn::Vec3 p_i, p_j, r_vec;
    djinn::real r_sq, r_mag;
    
    // Calculate this once outside the loop
    djinn::real cutoff_sq = 100; 

    for (int i = 0; i < num_particles; i++) {
        p_i = particles[i].getPosition();

        for (int j = 0; j < i; j++) {
            p_j = particles[j].getPosition();
            r_vec = p_i - p_j;
            
            // Calculate squared distance manually to avoid library overhead
            r_sq = (r_vec.x * r_vec.x) + (r_vec.y * r_vec.y) + (r_vec.z * r_vec.z);
            // Prevent division by zero and physics explosions
            djinn::real safe_r_sq = r_sq;
            if (safe_r_sq < 0.01) {
                safe_r_sq = 0.01; 
            }

            r_mag = sqrt(safe_r_sq);

            if (r_sq < cutoff_sq) {
                // Only calculate the expensive square root if interaction is guaranteed
                r_mag = sqrt(r_sq); 
                
                // Apply force to particle i
                lj.updateForce(&particles[i], r_vec, r_mag, dr);
                
                // Apply equal and opposite force to particle j
                lj.updateForce(&particles[j], r_vec * -1.0, r_mag, dr);
            }
        }
    }
}


int main() {
    // Set up logging
    try {
        auto logger = spdlog::basic_logger_mt("thermo", "logs/thermo.log", true);
        spdlog::set_default_logger(logger);
        spdlog::flush_on(spdlog::level::info);
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 0;
    }

    // Raylib Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Djinn - Thermodynamics Demo");
    ClearWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(0); // Uncapped

    rlFPCamera cam;
    cam.Setup(45, Vector3{0, 2, 0});
    cam.MoveSpeed.z = 1.0;
    cam.MoveSpeed.x = 1.0;
    cam.MoveSpeed.y = 1.0;

    // SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Define some initial conditions
    srand(1000000);

    djinn::PotentialRegistry u_reg;

    // Define boundary conditions
    djinn::Vec3 bounds = djinn::Vec3(1, 1, 1);
    Vector3 sq_center = bounds.normalize().toVector3();

    // Define a big array of particles
    int num_particles = 800;
    djinn::Particle particles[num_particles];
    // And an array for the raylib-based particle objects
    Vector3 rl_particles[num_particles];

    // Randomize each of their positions and add them to the potential registry
    for (int i = 0; i < num_particles; i++) {
      particles[i].setPosition(randomReal(0, (int)bounds.x),
                                randomReal(0, (int)bounds.y),
                                randomReal(0, (int)bounds.z));

       // Give the gas initial kinetic energy in all directions
      //particles[i].setVelocity(randomReal(-1.0e-1, 1.0e-1),
        //                      randomReal(-1.0e-1, 1.0e-1),
          //                    randomReal(-1.0e-1, 1.0e-1));
      particles[i].setVelocity(0, 0, 0);
      particles[i].setMass(0.5f);

      u_reg.add(&particles[i], &lj);
    }

    if (num_particles == 1) {
        particles[0].setVelocity(randomReal(0, (int)bounds.x),
                                 randomReal(0, (int)bounds.y),
                                 randomReal(0, (int)bounds.z));
    }
    Model particleModel = LoadModelFromMesh(GenMeshSphere(0.005f, 8, 8));
    particleModel.materials[0].maps[MATERIAL_MAP_ALBEDO].color = RED;

    // Allocate matrix array for instanced rendering
    Matrix* transforms = (Matrix*)RL_MALLOC(num_particles * sizeof(Matrix));
    const int trail_length = 200;
std::deque<Vector3> particle_trails[num_particles];

    while (!WindowShouldClose()) {
        // Calculate Lennard-Jones force of each pair of particles
        calculateLJ(particles, num_particles);
// --- DIAGNOSTIC TEST ---
for (int i = 0; i < num_particles; i++) {
    particles[i].addForce(djinn::Vec3(0.0f, -9.8f, 0.0f));
}
// -----------------------

        // Calculate new velocity according to Verlet Algorithm
        u_reg.integrateAll(dt);

        // Check boundaries and correct particles outside of them
        checkBoundaries(&particles[0], bounds, num_particles);


        // Update transform matrices
        for (int i = 0; i < num_particles; i++) {
            djinn::Vec3 p = particles[i].getPosition();
            transforms[i] = MatrixTranslate(p.x, p.y, p.z);
            particle_trails[i].push_back(p.toVector3());
    
    if (particle_trails[i].size() > trail_length) {
        particle_trails[i].pop_front();
    }
        }

        cam.Update();
        BeginDrawing();
        ClearBackground(BLACK);

        cam.BeginMode3D();
        DrawGrid(100, 0.1f);

        DrawCubeWires(sq_center, 1.1, 1.1, 1.1, WHITE);

        for (int i = 0; i < num_particles; i++) {
            djinn::Vec3 p = particles[i].getPosition();
            DrawModel(particleModel, p.toVector3(), 1.0f, WHITE);
            // Draw the trailing lines
            for (size_t k = 1; k < particle_trails[i].size(); k++) {
                // Calculate an opacity scalar from 0.0 to 1.0 based on age
                float alpha = (float)k / (float)particle_trails[i].size();
                
                DrawLine3D(particle_trails[i][k - 1], 
                          particle_trails[i][k], 
                          Fade(RED, alpha));
            }
        }

        cam.EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    RL_FREE(transforms);
    UnloadModel(particleModel);

    return 0;
}
