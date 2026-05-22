#include "djinn/tooling.h"
#include "raymath.h"
#include "rlgl.h"
#include "raylib.h"
#include <string>

namespace djinn {
    // Draws a 3D compass on the HUD
    void DrawHUDCompass(const rlFPCamera& cam, Vector2 screenPos, float scale) {
        // 1. Get a ray pointing through the desired HUD pixel on the screen
        Ray ray = cam.GetMouseRay(screenPos);
        
        // 2. Project the origin 2.0 units down that ray to prevent near-plane clipping
        Vector3 origin = Vector3Add(ray.position, Vector3Scale(ray.direction, 2.0f));

        // 3. Flush the render batch and disable depth testing to render over all 3D objects
        rlDrawRenderBatchActive();
        rlDisableDepthTest();

        // X-axis
        DrawLine3D(origin, Vector3{origin.x + scale, origin.y, origin.z}, RED);
        DrawSphere(Vector3{origin.x + scale, origin.y, origin.z}, scale * 0.1f, RED);
        
        // Y-axis
        DrawLine3D(origin, Vector3{origin.x, origin.y + scale, origin.z}, GREEN);
        DrawSphere(Vector3{origin.x, origin.y + scale, origin.z}, scale * 0.1f, GREEN);
        
        // Z-axis
        DrawLine3D(origin, Vector3{origin.x, origin.y, origin.z + scale}, BLUE);
        DrawSphere(Vector3{origin.x, origin.y, origin.z + scale}, scale * 0.1f, BLUE);

        // 4. Restore depth testing for the next frame
        rlDrawRenderBatchActive();
        rlEnableDepthTest();
    }


    void UpdateTooling(ToolingState& state, Particle* particles, int num_particles, const rlFPCamera& cam) {
        // Toggle pause state
        if (IsKeyPressed(KEY_P)) {
            state.isPaused = !state.isPaused;
        }

        // Frame stepping when paused
        state.stepFrame = false;
        state.stepFrameReverse = false;

        if (state.isPaused) {
            if (IsKeyDown(KEY_ENTER)) {
                state.stepFrame = true;
            } else if (IsKeyDown(KEY_BACKSPACE)) {
                state.stepFrameReverse = true;
            }
        }

        // Particle selection via Left Click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 screenCenter = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
            Ray ray = cam.GetMouseRay(screenCenter);
            
            float closestDistance = 10000.0f;
            int hitIndex = -1;

            for (int i = 0; i < num_particles; i++) {
                Vec3 pos = particles[i].getPosition();
                Vector3 center = { (float)pos.x, (float)pos.y, (float)pos.z };
                
                // Use a larger radius than the drawn mesh to make clicking easier
                RayCollision collision = GetRayCollisionSphere(ray, center, 0.05f);

                if (collision.hit && collision.distance < closestDistance) {
                    closestDistance = collision.distance;
                    hitIndex = i;
                }
            }

            state.selectedParticleIndex = hitIndex;
        }
        
        // Deselect via Right Click
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            state.selectedParticleIndex = -1;
        }
    }

    void DrawTooling3D(const ToolingState& state, Particle* particles, int num_particles) {
        if (state.selectedParticleIndex >= 0 && state.selectedParticleIndex < num_particles) {
            Vec3 pos = particles[state.selectedParticleIndex].getPosition();
            Vector3 center = { (float)pos.x, (float)pos.y, (float)pos.z };
            
            // Draw a yellow wireframe box around the selected particle
            DrawCubeWires(center, 0.03f, 0.03f, 0.03f, YELLOW);
            
            // Draw a green line indicating the velocity vector direction
            Vec3 vel = particles[state.selectedParticleIndex].getVelocity();
            Vector3 endPos = { 
                (float)(pos.x + vel.x * 0.1), 
                (float)(pos.y + vel.y * 0.1), 
                (float)(pos.z + vel.z * 0.1) 
            };
            DrawLine3D(center, endPos, GREEN);
        }
    }

    void DrawToolingUI(const ToolingState& state, Particle* particles, int num_particles) {
        // Draw execution status
        DrawRectangle(10, 40, 200, 30, Fade(BLACK, 0.7f));
        if (state.isPaused) {
            DrawText("STATUS: PAUSED", 20, 45, 20, RED);
            DrawText("ENTER: Step Fwd | BACKSPACE: Step Rev", 10, 75, 10, WHITE);
        } else {
            DrawText("STATUS: RUNNING", 20, 45, 20, GREEN);
            DrawText("Press P to Pause", 10, 75, 10, WHITE);
        }

        // Draw particle inspector panel
        if (state.selectedParticleIndex >= 0 && state.selectedParticleIndex < num_particles) {
            DrawRectangle(GetScreenWidth() - 250, 10, 240, 100, Fade(BLACK, 0.7f));
            DrawText("PARTICLE INSPECTOR", GetScreenWidth() - 240, 20, 10, YELLOW);
            
            std::string idText = "Array Index: " + std::to_string(state.selectedParticleIndex);
            DrawText(idText.c_str(), GetScreenWidth() - 240, 40, 10, WHITE);

            Vec3 pos = particles[state.selectedParticleIndex].getPosition();
            std::string posText = "Pos: " + std::to_string(pos.x).substr(0, 5) + ", " 
                                         + std::to_string(pos.y).substr(0, 5) + ", " 
                                         + std::to_string(pos.z).substr(0, 5);
            DrawText(posText.c_str(), GetScreenWidth() - 240, 60, 10, WHITE);

            Vec3 vel = particles[state.selectedParticleIndex].getVelocity();
            std::string velText = "Vel: " + std::to_string(vel.x).substr(0, 5) + ", " 
                                         + std::to_string(vel.y).substr(0, 5) + ", " 
                                         + std::to_string(vel.z).substr(0, 5);
            DrawText(velText.c_str(), GetScreenWidth() - 240, 80, 10, WHITE);
        }

        // Draw Mouse Crosshair (Fixed to Center)
        Vector2 screenCenter = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
        float chSize = 10.0f; 
        Color chColor = Fade(WHITE, 0.5f); 

        // Inner circle
        DrawCircleLines((int)screenCenter.x, (int)screenCenter.y, 4, chColor);

        // Horizontal line
        DrawLine((int)(screenCenter.x - chSize), (int)screenCenter.y, 
                (int)(screenCenter.x + chSize), (int)screenCenter.y, chColor);

        // Vertical line
        DrawLine((int)screenCenter.x, (int)(screenCenter.y - chSize), 
                (int)screenCenter.x, (int)(screenCenter.y + chSize), chColor);
    }

}
