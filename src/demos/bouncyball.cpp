/**
 * @file bounceyball.cpp
 * @brief Demo of a bouncing ball to demonstrate collision resolutions in the engine
 * @author Catyre
*/

#include "djinn/plinks.h"
#include "djinn/pcontacts.h"
#include "djinn/pfgen.h"
#include "raylib.h"
#include "rlgl.h"
#include "rlFPCamera.h"
#include "rlHelper.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <iostream>

#define MAX_ITERATIONS 100
#define BALLRADIUS 1

using namespace djinn;
using namespace std;

// Define a plane (the ground [y = 0])
class Floor : public ParticleContactGenerator {
    public:
        Vec3 start;
        Vec3 end;

        // Holds a pointer to the particles we're checking for collisions with
        Particle *particles;

        virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;

        Floor(Vec3 start, Vec3 end, Particle *particles) : start(start), end(end), particles(particles) {};
};

unsigned Floor::addContact(ParticleContact *contact, unsigned limit) const {
    Vec3 ball_bottom = particles->getPosition() - Vec3(0, BALLRADIUS, 0);
    // Check if the particle is below the floor
    if (ball_bottom.y < 0) {
        // If so, generate a contact
        contact->particles[0] = particles;
        contact->contactNormal = Vec3(0, 1, 0); // Already a unit vector, but normally you would normalize this vector
        contact->penetration = -ball_bottom.y;
        contact->restitution = 0.9f;

        return 1;
    }

    return 0;
}

int main() {
    // Set up logging
    try {
        auto logger = spdlog::basic_logger_mt("bouncyball", "logs/bouncyball.log", true);
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
	cam.Setup(45, Vector3{ 10, 5, 10 });
	cam.MoveSpeed.z = 10;
	cam.MoveSpeed.x = 10;
    cam.MoveSpeed.y = 10;
    //cam.lookAt(Vector3{ 0, 0, 0 });

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Define a ball
    Vec3 ball_xi = Vec3(0, 100, 0); // [m]
    Vec3 ball_vi = Vec3(0, 0, 0); // [m/s]
    Vec3 ball_ai = Vec3(0, 0, 0); // [m/s^2]

    real ballMass = 10; // [kg]

    Particle *ball = new Particle(ball_xi, ball_vi, ball_ai, 1, 1/ballMass, "Ball");

    // Time resolution
    real dt = 5e-2; // [s]

    // Define force generator (ParticleEarthGravity)
    ParticleEarthGravity gravity = ParticleEarthGravity(Vec3(0, -9.81, 0));

    // Define force registry and add to it
    ParticleForceRegistry registry;
    registry.add(ball, &gravity);

    // Define a contact generator
    Floor *floorContact = new Floor(Vec3(1, 0, 1), Vec3(-1, 0, -1), ball);

    ParticleContact *contact = new ParticleContact();

    unsigned iterationUsed = 0;
    while(!WindowShouldClose()) {
        iterationUsed += 1;

        // Update forces
        registry.updateForces(dt);

        // Update position
        ball->integrate(dt);

        // Update contacts
        

        unsigned usedContacts = floorContact->addContact(contact, 1);

        // Resolve contacts
        ParticleContactResolver resolver(MAX_ITERATIONS);
        resolver.resolveContacts(contact, usedContacts, dt);

        // Draw
        cam.Update();

        BeginDrawing();

            ClearBackground(BLACK);

            cam.BeginMode3D();

                DrawGrid(25, 1.0f);        // Draw a grid

                DrawSphere(ball->getPosition().toVector3(), BALLRADIUS, RED);

            cam.EndMode3D();

            DrawFPS(10, 10);

        EndDrawing();


    }

    delete ball;

    return 0;
}