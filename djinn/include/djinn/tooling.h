#ifndef DJINN_TOOLING_H
#define DJINN_TOOLING_H

#include "raylib.h"
#include "djinn/particle.h"
#include "rlFPCamera.h"

namespace djinn {

    struct ToolingState {
        bool isPaused = false;
        bool stepFrame = false;
        bool stepFrameReverse = false;
        int selectedParticleIndex = -1;
        
        // A sliding window of previous states
        std::deque<std::vector<Particle>> history;
        size_t maxHistoryFrames = 600; // 10 seconds of history at 60 FPS
    };

    // Draw 3D compass on the HUD
    void DrawHUDCompass(const rlFPCamera& cam, Vector2 screenPos, float scale);

    // Process inputs for pausing, stepping, and raycasting
    void UpdateTooling(ToolingState& state, Particle* particles, int num_particles, const rlFPCamera& cam);

    // Draw 3D overlays like selection highlights and velocity vectors
    void DrawTooling3D(const ToolingState& state, Particle* particles, int num_particles);

    // Draw the 2D HUD text and inspector panels
    void DrawToolingUI(const ToolingState& state, Particle* particles, int num_particles);

}

#endif
