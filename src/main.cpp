#include "raylib.h"
#include "app.h"

int main(){
    //code
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(800, 520, "Bubble Sort Visualizer (Modular)");
    SetWindowMinSize(MIN_WINDOW_W, MIN_WINDOW_H);

    // ── Setup Smooth Font Rendering ──
    if (FileExists("C:\\Windows\\Fonts\\segoeui.ttf")) {
        gFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 64, 0, 0);
    } else if (FileExists("/System/Library/Fonts/Helvetica.ttc")) {
        gFont = LoadFontEx("/System/Library/Fonts/Helvetica.ttc", 64, 0, 0);
    }
    if (gFont.texture.id != 0) {
        GenTextureMipmaps(&gFont.texture);
        SetTextureFilter(gFont.texture, TEXTURE_FILTER_TRILINEAR);
    }

    SetTargetFPS(60);

    // Create the Main Application Object
    App app;
    
}