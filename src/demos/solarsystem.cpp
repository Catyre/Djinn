/**
 * @file solarsystem.cpp
 * @brief Simulate the orbit of the Moon around Earth, and Earth around Sol
 * @author Catyre
 * @date 11-10-2022
*/

#include "engine/particle.h"
#include "engine/pfgen.h"
#include "raylib.h"
#include "rlgl.h"

#define CAMERA_IMPLEMENTATION
#define CAMERA_FIRST_PERSON_SWINGING_DELTA               0.0f      // Maximum up-down swinging distance when walking
#define CAMERA_FIRST_PERSON_TILTING_DELTA                0.0f     // Maximum left-right tilting distance when walking

#define LETTER_BOUNDRY_SIZE     0.25f
#define TEXT_MAX_LAYERS         32
#define LETTER_BOUNDRY_COLOR    VIOLET

bool SHOW_LETTER_BOUNDRY = false;
bool SHOW_TEXT_BOUNDRY = false;

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

using namespace engine;
using namespace std;

// Draw a codepoint in 3D space
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint);

// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint);

int main() {
    // Raylib Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Djinn - Solar Orbit Demo");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 5.0f, 0.0f };
    camera.target = (Vector3){ 15.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
    SetCameraMoveControls(KEY_W, KEY_S, KEY_D, KEY_A, KEY_SPACE, KEY_LEFT_SHIFT);

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

    Particle *sol = new Particle(Vec3(), Vec3(), Vec3(), 1, 1/SOLARMASS);
    Particle *earth = new Particle(earth_xi, earth_vi, earth_ai, 1, 1/EARTHMASS);
    Particle *moon = new Particle(moon_xi, moon_vi, moon_ai, 1, 1/MOONMASS);
    Particle *mars = new Particle(mars_xi, mars_vi, mars_ai, 1, 1/MARSMASS);
    Particle *venus = new Particle(venus_xi, venus_vi, venus_ai, 1, 1/VENUSMASS);
    Particle *mercury = new Particle(mercury_xi, mercury_vi, mercury_ai, 1, 1/MERCMASS);

    real scale = 1e-10;

    // Time resolution
    real dt = 1e3; // [s]

    // Define force registry
    ParticleUniversalForceRegistry gravityRegistry;

    // Define force generators to be applied to the particle
    // ParticlePointGravity* solGravity = new ParticlePointGravity(solGravityOrigin, solMass); // Args: (Vec3) origin, (real) mass
    // ParticlePointGravity* earthGravity = new ParticlePointGravity(earth_xi, EARTHMASS);

    //vector<Particle*> particles = {earth, moon, mars, venus, mercury, sol};
    vector<Particle*> particles = {earth, sol};

    gravityRegistry.add(particles);

    // Register force generators with the force registry
    // registry.add(earth, solGravity);
    // registry.add(moon, earthGravity);
    // registry.add(mars, solGravity);
    // registry.add(venus, solGravity);
    // registry.add(mercury, solGravity);
    while(!WindowShouldClose()) {
        cout << "Before applyGravity: " << earth->getPosition().toString() << endl;
        gravityRegistry.applyGravity();

        cout << "After applyGravity: " << earth->getPosition().toString() << endl;
        gravityRegistry.integrateAll(dt);
        cout << "After integrateAll: " << earth->getPosition().toString() << endl;

        // registry.updateForces(dt);
        // earth->integrate(dt);
        // moon->integrate(dt);
        // mars->integrate(dt);
        // venus->integrate(dt);
        // mercury->integrate(dt);

        //earthGravity->setOrigin(earth->getPosition());

        cout << "Line 153: " << earth->getPosition().toString() << endl;
        Vec3 earth_x = earth->getPosition() * scale; // Scale down to tens of meters
        Vec3 moon_x = moon->getPosition() * scale; // Scale down to tens of meters
        Vec3 mars_x = mars->getPosition() * scale; // Scale down to tens of meters
        Vec3 venus_x = venus->getPosition() * scale; // Scale down to tens of meters
        Vec3 mercury_x = mercury->getPosition() * scale; // Scale down to tens of meters
        Vec3 sol_x = sol->getPosition() * scale; // Scale down to tens of meters


        cout << "Line 162: " << earth->getPosition().toString() << endl;
        // Convert my Vec3 object to a Raylib Vector3 object
        Vector3 *rl_earth_x = reinterpret_cast<Vector3*>(&earth_x); // Raylib vector for drawing
        Vector3 *rl_moon_x = reinterpret_cast<Vector3*>(&moon_x);
        Vector3 *rl_mars_x = reinterpret_cast<Vector3*>(&mars_x);
        Vector3 *rl_venus_x = reinterpret_cast<Vector3*>(&venus_x);
        Vector3 *rl_mercury_x = reinterpret_cast<Vector3*>(&mercury_x);
        Vector3 *rl_sol_x = reinterpret_cast<Vector3*>(&sol_x);

        UpdateCamera(&camera);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);

                DrawGrid(100, 1.0f);        // Draw a grid
                DrawSphere(*rl_earth_x, 0.25, BLUE); // Draw earth
                DrawText3D(GetFontDefault(), "Earth", (Vector3){rl_earth_x->x, 2, rl_earth_x->z}, 10, 1, 1, true, WHITE); // Label Earth
                //DrawLine3D((Vector3){rl_earth_x->x-1, 0, rl_earth_x->z-1}, *rl_earth_x, BLUE);

                DrawSphere(*rl_moon_x, 0.1, GRAY); // Draw moon
                DrawText3D(GetFontDefault(), "Moon", (Vector3){rl_moon_x->x, 2, rl_moon_x->z}, 10, 1, 1, true, WHITE); // Label Moon

                DrawSphere(*rl_mars_x, 0.2, RED); // Draw mars
                DrawText3D(GetFontDefault(), "Mars", (Vector3){rl_mars_x->x, 2, rl_mars_x->z}, 10, 1, 1, true, WHITE); // Label Mars

                DrawSphere(*rl_venus_x, 0.2, YELLOW); // Draw venus
                DrawText3D(GetFontDefault(), "Venus", (Vector3){rl_venus_x->x, 2, rl_venus_x->z}, 10, 1, 1, true, WHITE); // Label Venus

                DrawSphere(*rl_mercury_x, 0.1, ORANGE); // Draw mercury
                DrawText3D(GetFontDefault(), "Mercury", (Vector3){rl_mercury_x->x, 2, rl_mercury_x->z}, 10, 1, 1, true, WHITE); // Label Mercury

                DrawSphere(*rl_sol_x, 1, YELLOW); // Draw Sol
                DrawText3D(GetFontDefault(), "Sol", (Vector3){rl_sol_x->x, 2, rl_sol_x->z}, 10, 1, 1, true, WHITE); // Label Sol

            EndMode3D();

            DrawRectangle(10, 30, 300, 75, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(10, 30, 300, 75, BLUE);

            DrawText("Earth:", 20, 40, 10, WHITE);
            DrawText(TextFormat("X: %02.02f", rl_earth_x->x), 20, 55, 10, WHITE);
            DrawText(TextFormat("Y: %02.02f", rl_earth_x->y), 20, 70, 10, WHITE);
            DrawText(TextFormat("Z: %02.02f", rl_earth_x->z), 20, 85, 10, WHITE);

            DrawText("Moon:", 80, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x->x), 80, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x->y), 80, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_moon_x->z), 80, 85, 10, WHITE);

            DrawText("Mars:", 140, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mars_x->x), 140, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mars_x->y), 140, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mars_x->z), 140, 85, 10, WHITE);

            DrawText("Venus:", 200, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_venus_x->x), 200, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_venus_x->y), 200, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_venus_x->z), 200, 85, 10, WHITE);

            DrawText("Mercury:", 260, 40, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mercury_x->x), 260, 55, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mercury_x->y), 260, 70, 10, WHITE);
            DrawText(TextFormat("%02.02f", rl_mercury_x->z), 260, 85, 10, WHITE);

            DrawFPS(10, 10);

        EndDrawing();

    }

    CloseWindow();

    //delete pointGravity;
    delete earth;

    return 0;
}

// Draw codepoint at specified position in 3D space
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint)
{
    // Character index position in sprite font
    // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scale = fontSize/(float)font.baseSize;

    // Character destination rectangle on screen
    // NOTE: We consider charsPadding on drawing
    position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding)/(float)font.baseSize*scale;
    position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding)/(float)font.baseSize*scale;

    // Character source rectangle from font texture atlas
    // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f*font.glyphPadding, font.recs[index].height + 2.0f*font.glyphPadding };

    float width = (float)(font.recs[index].width + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;
    float height = (float)(font.recs[index].height + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;

    if (font.texture.id > 0)
    {
        const float x = 0.0f;
        const float y = 0.0f;
        const float z = 0.0f;

        // normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
        const float tx = srcRec.x/font.texture.width;
        const float ty = srcRec.y/font.texture.height;
        const float tw = (srcRec.x+srcRec.width)/font.texture.width;
        const float th = (srcRec.y+srcRec.height)/font.texture.height;

        if (SHOW_LETTER_BOUNDRY) DrawCubeWiresV((Vector3){ position.x + width/2, position.y, position.z + height/2}, (Vector3){ width, LETTER_BOUNDRY_SIZE, height }, LETTER_BOUNDRY_COLOR);

        rlCheckRenderBatchLimit(4 + 4*backface);
        rlSetTexture(font.texture.id);

        rlPushMatrix();
            rlTranslatef(position.x, position.y, position.z);

            rlBegin(RL_QUADS);
                rlColor4ub(tint.r, tint.g, tint.b, tint.a);

                // Front Face
                rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
                rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);              // Top Left Of The Texture and Quad
                rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height);     // Bottom Left Of The Texture and Quad
                rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);     // Bottom Right Of The Texture and Quad
                rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad

                if (backface)
                {
                    // Back Face
                    rlNormal3f(0.0f, -1.0f, 0.0f);                              // Normal Pointing Down
                    rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);          // Top Right Of The Texture and Quad
                    rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);          // Top Left Of The Texture and Quad
                    rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height); // Bottom Left Of The Texture and Quad
                    rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height); // Bottom Right Of The Texture and Quad
                }
            rlEnd();
        rlPopMatrix();

        rlSetTexture(0);
    }
}

// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint) {
    int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;               // Offset X to next character to draw

    float scale = fontSize/(float)font.baseSize;

    for (int i = 0; i < length;) {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;

        if (codepoint == '\n') {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += scale + lineSpacing/(float)font.baseSize*scale;
            textOffsetX = 0.0f;
        } else {
            if ((codepoint != ' ') && (codepoint != '\t')) {
                //DrawTextCodepoint3D(font, codepoint, (Vector3){ position.x + textOffsetX, position.y, position.z + textOffsetY }, fontSize, backface, tint);
                DrawTextCodepoint3D(font, codepoint, (Vector3){ position.x + textOffsetX, position.y + textOffsetY, position.z}, fontSize, backface, tint);
            }

            if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing)/(float)font.baseSize*scale;
            else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing)/(float)font.baseSize*scale;
        }

        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }
}