echo.
echo === Bubble Sort Visualizer - Windows Build ===
echo.

REM --- Check required files ---
if not exist "src\main.cpp" ( echo [ERROR] src\main.cpp not found! & pause & exit /b 1 )
if not exist "raylib\raylib.h" ( echo [ERROR] raylib\raylib.h not found! & pause & exit /b 1 )
if not exist "raylib\libraylib.a" ( echo [ERROR] raylib\libraylib.a not found! & pause & exit /b 1 )

REM --- Find compiler (prefer raylib's bundled one) ---
set "GCC="
if exist "C:\raylib\w64devkit\bin\g++.exe" (
    set "GCC=C:\raylib\w64devkit\bin\g++.exe"
    set "PATH=C:\raylib\w64devkit\bin;%PATH%"
    echo [OK] Using raylib w64devkit compiler
    goto :build
)

where g++ >nul 2>nul
if %ERRORLEVEL% == 0 (
    set "GCC=g++"
    echo [OK] Using system g++ compiler
    goto :build
)

echo [ERROR] No g++ compiler found!
echo   Install raylib from https://raylib.com (includes compiler)
pause
exit /b 1

:build
echo Compiling...
"%GCC%" src\main.cpp -o visualizer.exe -Iraylib -Isrc -Lraylib -lraylib -lgdi32 -lwinmm -lopengl32 -std=c++11 -static -O2

if %ERRORLEVEL% == 0 (
    echo.
    echo [OK] BUILD SUCCESSFUL!
    echo [OK] Run: visualizer.exe
) else (
    echo.
    echo [ERROR] BUILD FAILED - see errors above
)
echo.
pause