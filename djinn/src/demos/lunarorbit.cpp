/**
 * @file lunarorbit.cpp
 * @brief Simulate the orbit of the Moon around Earth, and give output of the dynamics to logs/lunarorbit.log
 * @author Catyre
*/

#include "djinn/particle.h"
#include "djinn/pfgen.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "raylib.h"
#include "rlgl.h"
#include "rlFPCamera.h"
#include "rlHelper.h"

#define CAMERA_IMPLEMENTATION

// Set some physical values for the simulation
#define MOONMASS 7.34767309e22 // [kg]
#define MOONORBIT 3.844e8 // [m]
#define MOONSPEED 1.023e3 // [m/s]
#define MOONRADIUS 1.737e6 // [m]
#define EARTHMASS 5.97219e24 // [kg]
#define EARTHRADIUS 6.371e6 // [m]

int main() {
    // Set up logging
    try {
        auto logger = spdlog::basic_logger_mt("lunarorbit", "logs/lunarorbit.log", true);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 0;
    }

    // Raylib Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Djinn - Lunar Orbit Demo");

    // Define the camera to look into our 3d world (position, target, up vector)
    rlFPCamera cam;
	cam.Setup(45, Vector3{ 0, 10, 0 });
	cam.MoveSpeed.z = 10;
	cam.MoveSpeed.x = 10;
    cam.MoveSpeed.y = 10;

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Initial conditions are at the rightmost point of the orbit (looking down on the system), 
    //   where the position is only in x and the velocity is only in z
    djinn::Vec3 moon_xi = djinn::Vec3(MOONORBIT, 0, 0); // [m]
    djinn::Vec3 moon_vi = djinn::Vec3(0, 0, MOONSPEED); // [m/s]
    djinn::Vec3 moon_ai = djinn::Vec3(0, 0, 0); // [m/s^2]

    djinn::Vec3 earth_xi = djinn::Vec3(0, 0, 0);
    djinn::Vec3 earth_vi = djinn::Vec3(0, 0, 0);
    djinn::Vec3 earth_ai = djinn::Vec3(0, 0, 0);

    djinn::Particle *moon = new djinn::Particle(moon_xi, moon_vi, moon_ai, 1, 1/MOONMASS, "Moon");
    djinn::Particle *earth = new djinn::Particle(earth_xi, earth_vi, earth_ai, 1, 1/EARTHMASS, "Earth");

    // Time resolution
    djinn::real dt = 1e3; // [s]

    // Define force registry
    djinn::ParticleUniversalForceRegistry gravityRegistry;

    // Register force generators with the particle
    gravityRegistry.add(moon);
    gravityRegistry.add(earth);

    // Deliberately added to demonstrate that the force registry will only take one copy of any given particle 
    //  and discard the duplicates (see logs/lunarorbit.log)
    gravityRegistry.add(moon);

    // Alternatively:
    //gravityRegistry.add(vector<Particle*>{moon, earth});

    int frame = 0;
    while(!WindowShouldClose()) {
        // Increment frame
        frame += 1;

        gravityRegistry.applyGravity();

        // Output to log
        spdlog::info("---------------------------------------------------------------------------------------------------------------------------");
        spdlog::info("Frame: {}", frame);
        spdlog::info("Moon position:     {} | Earth position:     {}", moon->getPosition().toString(), earth->getPosition().toString());
        spdlog::info("Moon velocity:     {} | Earth velocity:     {}", moon->getVelocity().toString(), earth->getVelocity().toString());
        spdlog::info("Moon acceleration: {} | Earth Acceleration: {}", moon->getAcceleration().toString(), earth->getAcceleration().toString());
        spdlog::info("Moon net force:    {} | Earth net force:    {}", moon->getNetForce().toString(), earth->getNetForce().toString());
        spdlog::info("---------------------------------------------------------------------------------------------------------------------------");
        
        moon->integrate(dt);
        earth->integrate(dt);

        // Alternatively:
        //gravityRegistry.integrateAll(dt);

        djinn::Vec3 moon_x = moon->getPosition() * 1e-7;
        djinn::Vec3 earth_x = earth->getPosition() * 1e-7; // Scale down to hundreds of km

        Vector3 rl_moon_x = moon_x.toVector3();
        Vector3 rl_earth_x = earth_x.toVector3();


        cam.Update();

        BeginDrawing();
            ClearBackground(BLACK);

            cam.BeginMode3D();

                DrawGrid(100, 1.0f);        // Draw a grid
                DrawSphere(rl_moon_x, 1, WHITE);
                djinn::DrawText3D(GetFontDefault(), moon->getName().c_str(), djinn::Vec3(moon_x.x, 6, moon_x.z).toVector3(), 10, 1, 1, true, WHITE);

                DrawSphere(rl_earth_x, EARTHRADIUS/MOONRADIUS, BLUE);
                djinn::DrawText3D(GetFontDefault(), earth->getName().c_str(), djinn::Vec3(earth_x.x, 6, earth_x.z).toVector3(), 10, 1, 1, true, WHITE);

            cam.EndMode3D();

            DrawRectangle(10, 30, 125, 75, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(10, 30, 125, 75, BLUE);

            DrawText("Earth:", 20, 40, 10, WHITE);
            DrawText(TextFormat("X: %02.02f", rl_earth_x.x), 20, 55, 10, WHITE);
            DrawText(TextFormat("Y: %02.02f", rl_earth_x.y), 20, 70, 10, WHITE);
            DrawText(TextFormat("Z: %02.02f", rl_earth_x.z), 20, 85, 10, WHITE);

            DrawText("Moon:", 80, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x.x), 80, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x.y), 80, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x.z), 80, 85, 10, WHITE);

            DrawFPS(10, 10);

        EndDrawing();

        spdlog::info("===========================================================================================================================");
    }

    CloseWindow();

    delete moon;
    delete earth;

    return 0;
}