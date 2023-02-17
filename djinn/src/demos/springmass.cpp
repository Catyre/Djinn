/**
 * @file springmass.cpp
 * @brief Demonstration of the spring force generator in a gravitational field
 * @author Catyre
 * @date 11-10-2022
*/

#include "djinn/particle.h"
#include "djinn/pfgen.h"
#include "raylib.h"
#include "rlgl.h"
#include "rlFPCamera.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#define LINECOUNT 5

int main() {
    // Set up logging
    try {
        auto logger = spdlog::basic_logger_mt("springmass", "logs/springmass.log", true);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 0;
    }

    // Raylib Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Djinn - Springmass Demo");

    // Define the camera to look into our 3d world (position, target, up vector)
    rlFPCamera cam;
	cam.Setup(45, Vector3{ 0, 10, 0 });
	cam.MoveSpeed.z = 10;
	cam.MoveSpeed.x = 10;
    cam.MoveSpeed.y = 10;

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Create the necessary initial conditions:
    djinn::Vec3 x_i = djinn::Vec3(0, 10, 0); // [m]
    djinn::Vec3 v_i = djinn::Vec3(0, 0, 0); // [m/s]
    djinn::Vec3 a_i = djinn::Vec3(0, 0, 0); // [m/s^2]
    djinn::Vec3 springAnchor = djinn::Vec3(0, 15, 0); // [m]
    bool first = true;
    djinn::real dt = 1e-1; // [s]
    djinn::real mass = 1; // [kg]
    djinn::real damping = 1;
    djinn::real springConstant = 1;
    djinn::real restLength = 1; // [m]
    djinn::real elasticLimit = 1e1; // [m]

    djinn::Particle* p1 = new djinn::Particle(x_i, v_i, a_i, damping, 1/mass);

    djinn::ParticleForceRegistry registry;
    djinn::ParticleAnchoredSpring* spring = new djinn::ParticleAnchoredSpring(&springAnchor, springConstant, restLength, elasticLimit);
    djinn::ParticleEarthGravity* gravity = new djinn::ParticleEarthGravity(djinn::Vec3(0, -9.81, 0));

    registry.add(p1, spring);
    registry.add(p1, gravity);

    while(!WindowShouldClose()) {
        registry.updateForces(dt);
        p1->integrate(dt);

        Vector3 massPos = p1->getPosition().toVector3();

        cam.Update();

        BeginDrawing();
            ClearBackground(BLACK);

            cam.BeginMode3D();

                DrawGrid(100, 1.0f);        // Draw a grid
                DrawSphere(massPos, 1, RED);
    
            cam.EndMode3D();

            DrawRectangle(10, 30, 125, 75, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(10, 30, 125, 75, BLUE);

            DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
} // int main()