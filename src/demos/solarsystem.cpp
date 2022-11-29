/**
 * @file solarsystem.cpp
 * @brief Simulate the orbit of the inner planets around the sun
 * @author Catyre
 * @date 11-10-2022
*/

#include "djinn/particle.h"
#include "djinn/pfgen.h"
#include "raylib.h"
#include "rlgl.h"
#include "rlFPCamera.h"
#include "rlHelper.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#define CAMERA_IMPLEMENTATION

// Set some physical values for the simulation
#define SOLARMASS 1.989e30 // [kg]
#define SOLARORBIT 0 // [m]
#define SOLARVELOCITY 0 // [m/s] 
#define SOLARRADIUS 6.957e8 // [m]

#define MERCMASS 3.285e23 // [kg]
#define MERCORBIT 5.790e10 // [m]
#define MERCVELOCITY 4.790e4 // [m/s]
#define MERCRADIUS 2.439e6 // [m]

#define VENUSMASS 4.867e24 // [kg]
#define VENUSORBIT 1.082e11 // [m]
#define VENUSVELOCITY 3.500e4 // [m/s]
#define VENUSRADIUS 6.051e6 // [m]

#define EARTHMASS 5.97219e24 // [kg]
#define EARTHORBIT 1.496e11 // [m]
#define EARTHVELOCITY 2.978e4 // [m/s] (Relative to Sun)
#define EARTHRADIUS 6.371e6 // [m]

#define MOONMASS 7.34767309e22 // [kg]
#define MOONORBIT 3.844e8 // [m]
#define MOONVELOCITY 1.023e3 // [m/s] (Relative to Earth)
#define MOONRADIUS 1.737e6 // [m]

#define MARSMASS 6.39e23 // [kg]
#define MARSORBIT 2.279e11 // [m]
#define MARSVELOCITY 2.41e4 // [m/s] (Relative to Sun)
#define MARSRADIUS 3.3895e6 // [m]

using namespace djinn;
using namespace std;

int main() {
    // Set up logging
    try {
        auto logger = spdlog::basic_logger_mt("solarsystem", "logs/solarsystem.log", true);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex& ex) {
        cout << "Log init failed: " << ex.what() << endl;
        return 0;
    }

    // Raylib Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Djinn - Solar Orbit Demo");

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
    Vec3 moon_xi = Vec3(MOONORBIT + EARTHORBIT, 0, 0); // [m]
    Vec3 moon_vi = Vec3(0, 0, MOONVELOCITY + EARTHVELOCITY); // [m/s]
    Vec3 moon_ai = Vec3(0, 0, 0); // [m/s^2]

    Vec3 earth_xi = Vec3(EARTHORBIT, 0, 0); // [m]
    Vec3 earth_vi = Vec3(0, 0, EARTHVELOCITY); // [m/s]
    Vec3 earth_ai = Vec3(0, 0, 0); // [m/s^2]

    Vec3 mars_xi = Vec3(MARSORBIT, 0, 0); // [m]
    Vec3 mars_vi = Vec3(0, 0, MARSVELOCITY); // [m/s]
    Vec3 mars_ai = Vec3(0, 0, 0); // [m/s^2]

    Vec3 venus_xi = Vec3(VENUSORBIT, 0, 0); // [m]
    Vec3 venus_vi = Vec3(0, 0, VENUSVELOCITY); // [m/s]
    Vec3 venus_ai = Vec3(0, 0, 0); // [m/s^2]

    Vec3 mercury_xi = Vec3(MERCORBIT, 0, 0); // [m]
    Vec3 mercury_vi = Vec3(0, 0, MERCVELOCITY); // [m/s]
    Vec3 mercury_ai = Vec3(0, 0, 0); // [m/s^2]

    Particle *sol = new Particle(Vec3(), Vec3(), Vec3(), 1, 1/SOLARMASS, "Sol");
    Particle *earth = new Particle(earth_xi, earth_vi, earth_ai, 1, 1/EARTHMASS, "Earth");
    Particle *moon = new Particle(moon_xi, moon_vi, moon_ai, 1, 1/MOONMASS, "Moon");
    Particle *mars = new Particle(mars_xi, mars_vi, mars_ai, 1, 1/MARSMASS, "Mars");
    Particle *venus = new Particle(venus_xi, venus_vi, venus_ai, 1, 1/VENUSMASS, "Venus");
    Particle *mercury = new Particle(mercury_xi, mercury_vi, mercury_ai, 1, 1/MERCMASS, "Mercury");

    real scale = 1e-10;

    // Time resolution
    real dt = 1e4; // [s]

    // Define force registry
    ParticleUniversalForceRegistry gravityRegistry;

    vector<Particle*> particles = {earth, moon, mars, venus, mercury, sol};
    // vector<Particle*> particles = {earth, sol};

    gravityRegistry.add(particles);

    while(!WindowShouldClose()) {
        gravityRegistry.applyGravity();

        gravityRegistry.integrateAll(dt);

        // Convert my Vec3 object to a Raylib Vector3 object and scale down to tens of meters
        Vector3 rl_earth_x = (earth->getPosition() * scale).toVector3();
        Vector3 rl_moon_x = (moon->getPosition() * scale).toVector3();
        Vector3 rl_mars_x = (mars->getPosition() * scale).toVector3();
        Vector3 rl_venus_x = (venus->getPosition() * scale).toVector3();
        Vector3 rl_mercury_x = (mercury->getPosition() * scale).toVector3();
        Vector3 rl_sol_x = (sol->getPosition() * scale).toVector3();

        cam.Update();

        BeginDrawing();
            ClearBackground(BLACK);

            cam.BeginMode3D();

                DrawGrid(100, 1.0f);        // Draw a grid
                DrawSphere(rl_earth_x, 0.25, BLUE); // Draw earth
                DrawText3D(GetFontDefault(), earth->getName().c_str(), (Vector3){rl_earth_x.x, 2, rl_earth_x.z}, 10, 1, 1, true, WHITE); // Label Earth
                //DrawLine3D((Vector3){rl_earth_x.x-1, 0, rl_earth_x.z-1}, *rl_earth_x, BLUE);

                DrawSphere(rl_moon_x, 0.1, GRAY); // Draw moon
                DrawText3D(GetFontDefault(), moon->getName().c_str(), (Vector3){rl_moon_x.x, 4, rl_moon_x.z}, 10, 1, 1, true, WHITE); // Label Moon

                DrawSphere(rl_mars_x, 0.2, RED); // Draw mars
                DrawText3D(GetFontDefault(), mars->getName().c_str(), (Vector3){rl_mars_x.x, 2, rl_mars_x.z}, 10, 1, 1, true, WHITE); // Label Mars

                DrawSphere(rl_venus_x, 0.2, BEIGE); // Draw venus
                DrawText3D(GetFontDefault(), venus->getName().c_str(), (Vector3){rl_venus_x.x, 2, rl_venus_x.z}, 10, 1, 1, true, WHITE); // Label Venus

                DrawSphere(rl_mercury_x, 0.1, ORANGE); // Draw mercury
                DrawText3D(GetFontDefault(), mercury->getName().c_str(), (Vector3){rl_mercury_x.x, 2, rl_mercury_x.z}, 10, 1, 1, true, WHITE); // Label Mercury

                DrawSphere(rl_sol_x, 1, YELLOW); // Draw Sol
                DrawText3D(GetFontDefault(), sol->getName().c_str(), (Vector3){rl_sol_x.x, 2, rl_sol_x.z}, 10, 1, 1, true, WHITE); // Label Sol

            cam.EndMode3D();

            DrawRectangle(10, 30, 300, 75, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(10, 30, 300, 75, BLUE);

            DrawText("Earth:", 20, 40, 10, WHITE);
            DrawText(TextFormat("X: %02.02f", rl_earth_x.x), 20, 55, 10, WHITE);
            DrawText(TextFormat("Y: %02.02f", rl_earth_x.y), 20, 70, 10, WHITE);
            DrawText(TextFormat("Z: %02.02f", rl_earth_x.z), 20, 85, 10, WHITE);

            DrawText("Moon:", 80, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x.x), 80, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x.y), 80, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x.z), 80, 85, 10, WHITE);

            DrawText("Mars:", 140, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mars_x.x), 140, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mars_x.y), 140, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mars_x.z), 140, 85, 10, WHITE);

            DrawText("Venus:", 200, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_venus_x.x), 200, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_venus_x.y), 200, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_venus_x.z), 200, 85, 10, WHITE);

            DrawText("Mercury:", 260, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mercury_x.x), 260, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mercury_x.y), 260, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mercury_x.z), 260, 85, 10, WHITE);

            DrawFPS(10, 10);

        EndDrawing();

    }

    CloseWindow();

    //delete pointGravity;
    delete earth;

    return 0;
}
