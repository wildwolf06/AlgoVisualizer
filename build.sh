#!/bin/bash
# ============================================
#  Bubble Sort Visualizer - Mac/Linux Build
# ============================================
#
#  Prerequisites:
#    Mac:   brew install raylib
#    Linux: sudo apt install libraylib-dev  (or build from source)
#
#  Files needed: src/main.cpp
# ============================================

echo ""
echo "=== Bubble Sort Visualizer - Mac/Linux Build ==="
echo ""

# Check source file exists
if [ ! -f "src/main.cpp" ]; then
    echo "[ERROR] src/main.cpp not found!"
    exit 1
fi

# Detect OS
OS="$(uname -s)"

if [ "$OS" = "Darwin" ]; then
    echo "[OK] Detected macOS"
    g++ src/main.cpp -o visualizer -Iraylib -Isrc -Lraylib -lraylib \
        -framework OpenGL -framework Cocoa \
        -framework IOKit -framework CoreVideo \
        -std=c++11
elif [ "$OS" = "Linux" ]; then
    echo "[OK] Detected Linux"
    g++ src/main.cpp -o visualizer -Iraylib -Isrc -Lraylib -lraylib \
        -lGL -lm -lpthread -ldl -lrt -lX11 \
        -std=c++11
else
    echo "[ERROR] Unknown OS: $OS"
    exit 1
fi

if [ $? -eq 0 ]; then
    echo ""
    echo "[OK] BUILD SUCCESSFUL!"
    echo "[OK] Run: ./visualizer"
else
    echo ""
    echo "[ERROR] BUILD FAILED - see errors above"
fi
