/**
 * @file rlHelper.h
 * @brief Helper functions for the library raylib
 * @author Catyre
*/

#ifndef RLHELPER_H
#define RLHELPER_H

#include "raylib.h"

namespace djinn {
    // Draw a codepoint in 3D space
    void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint);

    // Draw a 2D text in 3D space
    void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint);
}; // namespace djinn

#endif // RLHELPER_H
