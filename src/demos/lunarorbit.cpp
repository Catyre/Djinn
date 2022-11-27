/**
 * @file lunarorbit.cpp
 * @brief Simulate the orbit of the Moon around Earth, and give output of the dynamics to stdout
 * @author Catyre
*/

#include "djinn/particle.h"
#include "djinn/pfgen.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "raylib.h"
#include "rlgl.h"

#define CAMERA_IMPLEMENTATION

#define LETTER_BOUNDRY_SIZE     0.25f
#define TEXT_MAX_LAYERS         32
#define LETTER_BOUNDRY_COLOR    VIOLET

bool SHOW_LETTER_BOUNDRY = false;
bool SHOW_TEXT_BOUNDRY = false;

// Set some physical values for the simulation
#define MOONMASS 7.34767309e22 // [kg]
#define MOONORBIT 3.844e8 // [m]
#define MOONVELOCITY 1.023e3 // [m/s]
#define MOONRADIUS 1.737e6 // [m]
#define EARTHMASS 5.97219e24 // [kg]
#define EARTHRADIUS 6.371e6 // [m]

using namespace djinn;
using namespace std;

// Draw a codepoint in 3D space
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint);

// Draw a 2D text in 3D space
static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint);

int main() {
    // Set up logging
    try {
        auto logger = spdlog::basic_logger_mt("lunarorbit", "logs/lunarorbit.log", true);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex& ex) {
        cout << "Log init failed: " << ex.what() << endl;
        return 0;
    }

    // Raylib Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1440;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Djinn - Lunar Orbit Demo");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 0.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
    SetCameraMoveControls(KEY_W, KEY_S, KEY_D, KEY_A, KEY_SPACE, KEY_LEFT_SHIFT);

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Initial conditions are at the rightmost point of the orbit (looking down on the system), 
    //   where the position is only in x and the velocity is only in z
    Vec3 moon_xi = Vec3(MOONORBIT, 0, 0); // [m]
    Vec3 moon_vi = Vec3(0, 0, MOONVELOCITY); // [m/s]
    Vec3 moon_ai = Vec3(0, 0, 0); // [m/s^2]

    Vec3 earth_xi = Vec3(0, 0, 0);
    Vec3 earth_vi = Vec3(0, 0, 0);
    Vec3 earth_ai = Vec3(0, 0, 0);

    Particle *moon = new Particle(moon_xi, moon_vi, moon_ai, 1, 1/MOONMASS, "Moon");
    Particle *earth = new Particle(earth_xi, earth_vi, earth_ai, 1, 1/EARTHMASS, "Earth");

    // Time resolution
    real dt = 1e3; // [s]

    // Define force registry
    ParticleUniversalForceRegistry gravityRegistry;

    // Register force generators with the particle
    gravityRegistry.add(moon);
    gravityRegistry.add(earth);

    // Alternatively:
    //gravityRegistry.add(vector<Particle*>{moon, earth});

    int frame = 0;
    while(!WindowShouldClose()) {
        // Increment frame
        frame += 1;

        gravityRegistry.applyGravity();

        // Output to log
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

        Vec3 moon_x = moon->getPosition() * 1e-7;
        Vec3 earth_x = earth->getPosition() * 1e-7; // Scale down to hundreds of km

        Vector3 rl_moon_x = moon_x.toVector3();
        Vector3 rl_earth_x = earth_x.toVector3();

        UpdateCamera(&camera);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);

                DrawGrid(100, 1.0f);        // Draw a grid
                DrawSphere(rl_moon_x, 1, WHITE);
                DrawText3D(GetFontDefault(), "Moon", Vec3(moon_x.x, 6, moon_x.z).toVector3(), 10, 1, 1, true, WHITE);

                DrawSphere(rl_earth_x, EARTHRADIUS/MOONRADIUS, BLUE);
                DrawText3D(GetFontDefault(), "Earth", Vec3(earth_x.x, 6, earth_x.z).toVector3(), 10, 1, 1, true, WHITE);

            EndMode3D();

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