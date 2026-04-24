#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "constants.h"
#include <string>

// Global UI font loaded in main
extern Font gFont;

// ── SMOOTH TEXT WRAPPERS ──
inline void DrawTextSmooth(const char* text, int x, int y, int fontSize, Color c) {
    if (gFont.texture.id == 0) DrawText(text, x, y, fontSize, c);
    else DrawTextEx(gFont, text, {(float)x, (float)y}, fontSize, 1.0f, c);
}

inline void DrawTextSmoothCentered(const char* text, int x, int y, int w, int fontSize, Color c) {
    int tw = (gFont.texture.id == 0) ? MeasureText(text, fontSize) : MeasureTextEx(gFont, text, fontSize, 1.0f).x;
    DrawTextSmooth(text, x + (w - tw) / 2, y, fontSize, c);
}

inline int MeasureTextSmooth(const char* text, int fontSize) {
    if (gFont.texture.id == 0) return MeasureText(text, fontSize);
    return MeasureTextEx(gFont, text, fontSize, 1.0f).x;
}

// ── ROUNDED RECTANGLE HELPERS (Fixes pixelation) ──
inline void DrawRoundedRect(int x, int y, int w, int h, float roundness, Color color) {
    Rectangle rec = {(float)x, (float)y, (float)w, (float)h};
    // Segment count of 8 to ensure smooth corners
    DrawRectangleRounded(rec, roundness, 8, color);
}

inline void DrawRoundedRectLines(int x, int y, int w, int h, float roundness, float thickness, Color color) {
    Rectangle rec = {(float)x, (float)y, (float)w, (float)h};
    DrawRectangleRoundedLines(rec, roundness, 8, color);
}

inline void DrawGlowRect(int x, int y, int w, int h, float roundness, Color color, int glowRadius) {
    for (int i = 1; i <= glowRadius; i += 2) {
        Color c = color;
        c.a = (unsigned char)(255 * 0.05f * (1.0f - (float)i / glowRadius)); 
        Rectangle rec = {(float)x - i, (float)y - i, (float)w + i * 2, (float)h + i * 2};
        DrawRectangleRounded(rec, roundness, 8, c);
    }
}

#endif // UI_H
