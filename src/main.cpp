#include "raylib.h"
#include "app.h"

Font gFont = {0};

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(800, 520, "Bubble Sort Visualizer (Modular)");
    SetWindowMinSize(MIN_WINDOW_W, MIN_WINDOW_H);

    if (FileExists("C:\\Windows\\Fonts\\segoeui.ttf")) {
        gFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 64, 0, 0);
    } else if (FileExists("/Library/Fonts/Arial.ttf")) {
        gFont = LoadFontEx("/Library/Fonts/Arial.ttf", 64, 0, 0);
    }
    if (gFont.texture.id != 0) {
        SetTextureFilter(gFont.texture, TEXTURE_FILTER_BILINEAR);
    }

    SetTargetFPS(60);

    App app;

    while (!WindowShouldClose()) {
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        app.update(w, h);

        BeginDrawing();
        app.draw(w, h);
        EndDrawing();
    }

    if (gFont.texture.id != 0) UnloadFont(gFont);
    CloseWindow();
    return 0;
}
