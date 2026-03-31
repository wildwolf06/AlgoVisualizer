#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"

// ── WINDOW CONSTANTS ──
const int MIN_WINDOW_W = 600;
const int MIN_WINDOW_H = 400;

// ── ARRAY RANGE ──
const int MIN_ARRAY_SIZE = 2;
const int MAX_ARRAY_SIZE = 50;
const int DEFAULT_ARRAY_SIZE = 20;


const Color BG_COLOR      = { 10,  10,  15, 255 };  // Almost black to make neon pop
const Color PANEL_COLOR   = { 20,  22,  32, 255 };  // Sleek dark panel
const Color BORDER_COLOR  = { 45,  50,  70, 255 };  // Soft border

const Color TEXT_COLOR    = { 255, 255, 255, 255 }; // Pure crisp white
const Color DIM_TEXT      = { 160, 200, 240, 255 }; // Bright cyan-tinted dim text
const Color ACCENT_COLOR  = { 0,   200, 255, 255 }; // Neon cyan

// Bubble sort states 
const Color BAR_NORMAL    = { 0,   180, 255, 255 }; // Neon Blue
const Color BAR_COMPARING = { 255,  40,  90, 255 }; // Neon Red/Pink
const Color BAR_SWAPPED   = { 255, 220,   0, 255 }; // Neon Yellow
const Color BAR_DONE      = { 0,   255, 120, 255 }; // Neon Green

#endif 
