/**
 * @file thermo.cpp
 * @brief Demo of a gas reaching thermodynamic equilibrium
 * @author Catyre
 */

#include "djinn/particle.h"
#include "djinn/tooling.h"
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

djinn::real dt = 1e-2;
djinn::real dr = 1e-3;
djinn::real potential;
djinn::real sigma = 0.34;   // Lennard-Jones parameter
djinn::real epsilon = 0.38e-6; // Lennard-Jones parameter
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

        // Wrap around instead of reflecting
        if (pos.x >= bounds.x) pos.x -= bounds.x;
        else if (pos.x < 0) pos.x += bounds.x;

        if (pos.y >= bounds.y) pos.y -= bounds.y;
        else if (pos.y < 0) pos.y += bounds.y;

        if (pos.z >= bounds.z) pos.z -= bounds.z;
        else if (pos.z < 0) pos.z += bounds.z;

        particles[j].setPosition(pos.x, pos.y, pos.z);
    }
}

// Calculates Lennard-Jones force of each pair of particles
void calculateLJ(djinn::Particle particles[], int num_particles, djinn::Vec3 bounds) {
    djinn::Vec3 p_i, p_j, r_vec;
    djinn::real r_sq, r_mag;
    
    // Calculate this once outside the loop
    djinn::real cutoff_sq = 100; 

    for (int i = 0; i < num_particles; i++) {
        p_i = particles[i].getPosition();

        for (int j = 0; j < i; j++) {
            p_j = particles[j].getPosition();
            r_vec = p_i - p_j;

            // 2. Apply the Minimum Image Convention per axis
            if (r_vec.x > bounds.x * 0.5f) r_vec.x -= bounds.x;
            else if (r_vec.x < -bounds.x * 0.5f) r_vec.x += bounds.x;

            if (r_vec.y > bounds.y * 0.5f) r_vec.y -= bounds.y;
            else if (r_vec.y < -bounds.y * 0.5f) r_vec.y += bounds.y;

            if (r_vec.z > bounds.z * 0.5f) r_vec.z -= bounds.z;
            else if (r_vec.z < -bounds.z * 0.5f) r_vec.z += bounds.z;
            

            if (r_vec < djinn::Vec3(3e-1, 3e-1, 3e-1)) {
              r_vec = djinn::Vec3(3e-1, 3e-1, 3e-1);
            }

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
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Djinn - Thermodynamics Demo");
    ClearWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(0); // Uncapped

    rlFPCamera cam;
    cam.Setup(45, Vector3{0.5, 0.5, -2});
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
    Vector3 sq_center = Vector3{0.5, 0.5, 0.5};

    // Define a big array of particles
    int num_particles = 3000;
    djinn::Particle particles[num_particles];
    // And an array for the raylib-based particle objects
    Vector3 rl_particles[num_particles];

    // Randomize each of their positions and add them to the potential registry
    for (int i = 0; i < num_particles; i++) {
      particles[i].setPosition(randomReal(0.1, (int)bounds.x - 0.1),
                                randomReal(0.1, (int)bounds.y - 0.1),
                                randomReal(0.1, (int)bounds.z - 0.1));

       // Give the gas initial kinetic energy in all directions
      //particles[i].setVelocity(randomReal(-1.0e-1, 1.0e-1),
        //                      randomReal(-1.0e-1, 1.0e-1),
          //                    randomReal(-1.0e-1, 1.0e-1));
      particles[i].setVelocity(randomReal(-1, 1), randomReal(-1, 1), randomReal(-1, 1));
      particles[i].setMass(0.05f);

      u_reg.add(&particles[i], &lj);
    }


    if (num_particles == 1) {
        particles[0].setVelocity(randomReal(0, (int)bounds.x),
                                 randomReal(0, (int)bounds.y),
                                 randomReal(0, (int)bounds.z));
    }
    Model particleModel = LoadModelFromMesh(GenMeshSphere(0.005f, 8, 8));
    particleModel.materials[0].maps[MATERIAL_MAP_ALBEDO].color = WHITE;

    // Allocate matrix array for instanced rendering
    Matrix* transforms = (Matrix*)RL_MALLOC(num_particles * sizeof(Matrix));
    const int trail_length = 200;
    std::deque<Vector3> particle_trails[num_particles];

    // Initialize tooling
    djinn::ToolingState toolState;

    while (!WindowShouldClose()) {
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

        djinn::UpdateTooling(toolState, particles, num_particles, cam);
        if (!toolState.isPaused || toolState.stepFrame) {
            // 1. Record the current state before altering it
            std::vector<djinn::Particle> snapshot(particles, particles + num_particles);
            toolState.history.push_back(snapshot);
            if (toolState.history.size() > toolState.maxHistoryFrames) {
                toolState.history.pop_front();
            }

            // 2. Step physics forward
            // --- SUB-STEPPING ---
            int sub_steps = 3;
            djinn::real sim_dt = dt / sub_steps; 

            //for (int step = 0; step < sub_steps; step++) {
                calculateLJ(particles, num_particles, bounds);
                u_reg.integrateAll(sim_dt);
                checkBoundaries(&particles[0], bounds, num_particles);
            //}
            // --------------------
        } else if (toolState.isPaused && toolState.stepFrameReverse) {
            // Overwrite the live simulation with the most recent history state
            if (!toolState.history.empty()) {
                std::vector<djinn::Particle> lastState = toolState.history.back();
                toolState.history.pop_back();

                for (int i = 0; i < num_particles; i++) {
                    particles[i] = lastState[i];
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        cam.BeginMode3D();

        DrawCubeWires(sq_center, bounds.x, bounds.y, bounds.z, WHITE);

        for (int i = 0; i < num_particles; i++) {
            djinn::Vec3 p = particles[i].getPosition();
            djinn::real speed = particles[i].getVelocity().magnitude();
            Color particleTint = GREEN;

            if (speed > 5) {
              particleTint = RED;
            }

            DrawModel(particleModel, p.toVector3(), 1.0f, particleTint);

            // Draw the trailing lines
            for (size_t k = 1; k < particle_trails[i].size(); k++) {
                // Calculate an opacity scalar from 0.0 to 1.0 based on age
                float alpha = (float)k / (float)particle_trails[i].size();
               
                //DrawLine3D(particle_trails[i][k - 1], particle_trails[i][k], Fade(RED, alpha));
            }
        }

        // Draw the 3D tooling overlays (selection boxes, velocity vectors)
        djinn::DrawTooling3D(toolState, particles, num_particles);

        // Draw the HUD Compass last
        Vector2 compassScreenLocation = { 100.0f, screenHeight - 100.0f };
        djinn::DrawHUDCompass(cam, compassScreenLocation, 0.15f);

        cam.EndMode3D();

        // Draw the 2D tooling overlays (HUD, Inspector)
        djinn::DrawToolingUI(toolState, particles, num_particles);

        DrawFPS(10, 10);
        EndDrawing();
    }

    RL_FREE(transforms);
    UnloadModel(particleModel);

    return 0;
}
