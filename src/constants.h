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

// ── ABSTRACT CATEGORY COLORS ──
const Color COLOR_SORTING   = { 0,   180, 255, 255 }; 
const Color COLOR_SEARCHING = { 0,   220, 130, 255 }; 
const Color COLOR_GRAPHS    = { 180,  80, 255, 255 }; 

// ── UI OVERHAUL PALETTE ──
const Color BG_COLOR      = { 255, 255, 255, 255 }; 
const Color PANEL_COLOR   = { 15,  15,  15,  255 }; 
const Color HOVER_COLOR   = { 30,  30,  30,  255 }; 
const Color BORDER_COLOR  = { 45,  45,  52,  255 }; 

const Color TEXT_COLOR    = { 0,   0,   0,   255 }; 
const Color CARD_TEXT     = { 255, 255, 255, 255 }; 
const Color DIM_TEXT      = { 120, 120, 130, 255 }; 
const Color CARD_DIM_TEXT = { 150, 150, 150, 255 }; 
const Color ACCENT_COLOR  = { 0,   160, 255, 255 }; 
const Color ACCENT_HOVER  = { 40,  180, 255, 255 }; 

// ── DASHBOARD SOFT PALETTE (Visualizer Screens) ──
const Color DASH_BG       = { 248, 246, 240, 255 }; 
const Color DASH_PANEL    = { 255, 255, 255, 255 }; 
const Color DASH_TEXT     = { 45,  50,  55,  255 }; 
const Color DASH_ACCENT   = { 170, 210, 240, 255 }; 
const Color DASH_BORDER   = { 220, 220, 225, 255 }; 
const Color DASH_DIM      = { 160, 165, 170, 255 }; 

// ── VISUALIZER BAR STATES ──
const Color BAR_NORMAL    = { 170, 210, 240, 255 }; 
const Color BAR_COMPARING = { 250, 210, 130, 255 }; 
const Color BAR_SWAPPING  = { 80,  170, 255, 255 }; 
const Color BAR_DONE      = { 130, 200, 220, 255 }; 

// ── SEARCH VISUALIZER PALETTE ──
const Color SEARCH_NORMAL   = { 210, 200, 240, 255 }; 
const Color SEARCH_DIM      = { 230, 230, 235, 255 }; 
const Color SEARCH_BOUNDS   = { 140, 100, 200, 255 }; 
const Color SEARCH_MID      = { 255, 200, 150, 255 }; 
const Color SEARCH_FOUND    = { 160, 230, 180, 255 }; 

#endif // CONSTANTS_H
