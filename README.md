# Algorithm Visualizer

A modular, cross-platform C++ application that visually demonstrates sorting algorithms using vibrant, animated bars and an interactive interface. Built primarily for educational purposes and project demonstrations (e.g., 1st-year viva presentations).

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Raylib](https://img.shields.io/badge/Raylib-000000?style=for-the-badge&logo=raylib&logoColor=white)

## ✨ Features

- **Multiple Algorithms**: Currently supports **Bubble Sort** and **Insertion Sort**.
- **Interactive Controls**: Users can pause, step forward/backward, and adjust playback speed dynamically.
- **Customization**: Choose the dataset size (from 10 to 50 elements) and see real-time updates.
- **Vibrant UI**: Built using Raylib, featuring a neon-themed dark mode UI, smooth rectangle rendering, and clear visual indicators for element comparisons and swaps.
- **Live Statistics**: Tracks the number of comparisons and swaps performed during sorting.
- **Modular Codebase**: Clean architecture separating core algorithm math from graphical drawing logic.

## 📂 Project Structure

This project has been heavily modularized to decouple the UI from the algorithms and make introducing new algorithms incredibly simple.

| Folder / File | Purpose |
|------|---------|
| `src/main.cpp` | Application entry point and view registry. |
| `src/app.h` | Core application class containing view states (Menu/Sort) and events. |
| `src/sorting.h` | 100% graphical-free logical sorting implementations (Bubble & Insertion). |
| `src/ui.h` | Drawing abstraction wrappers (smooth pixel-perfect rects/text). |
| `src/constants.h` | Design tokens (vibrant hues, screen configuration sizes). |
| `raylib/` | Folder containing the `raylib` headers and statically linked library for Windows. |
| `build.bat` | Portable, zero-config build script for Windows. |
| `build.sh` | Build script for Mac/Linux environments. |

##  How to Build & Run

### Windows (Portable Build System)
This project uses a portable build script, meaning if you have a `g++` compiler, it will compile automatically. It is configured to automatically detect the `w64devkit` compiler bundled with the Windows Raylib installer off the `C:\` drive.

1. Ensure your system has a valid GCC compiler. If you don't have one, just download the [raylib windows installer](https://github.com/raysan5/raylib/releases/download/5.0/raylib_installer_v5.0.mingw.exe) and install it to `C:\raylib`.
2. Double-click the `build.bat` file in the project directory.
3. The script will output an executable named `visualizer.exe`. Run it!

### Mac
1. Install raylib via Homebrew: `brew install raylib`
2. Make the script executable and build: `chmod +x build.sh && ./build.sh`
3. Run the application: `./visualizer`

### Linux
1. Install raylib dependencies: `sudo apt install libraylib-dev`
2. Make the script executable and build: `chmod +x build.sh && ./build.sh`
3. Run the application: `./visualizer`

## 🎮 Controls

| Key | Action |
|-----|--------|
| **SPACE** | Play / Pause Sorting |
| **RIGHT ARROW** | Step forward (when paused) |
| **LEFT ARROW** | Step backward (when paused) |
| **UP ARROW** | Increase Playback Speed |
| **DOWN ARROW** | Decrease Playback Speed |
| **R** | Return to Menu/Setup |
| **ESC** | Quit Application |

## 🛠️ Adding New Algorithms
Thanks to the modular state-machine design in `src/sorting.h`, adding new algorithms is straightforward. You only need to create a new class inheriting from `SortAlgorithm`, implement the `step()` and `reset()` logic, and register it inside `src/app.h`.
