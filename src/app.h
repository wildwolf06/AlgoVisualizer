#ifndef APP_H
#define APP_H

#include "raylib.h"
#include "raymath.h"
#include "constants.h"
#include "sorting.h"
#include "ui.h"
#include <string>
#include <sstream>

using namespace std;
class App {
public:
    int screen; // 0 = Select Screen, 1 = Sort Screen
    
    // Select Screen State
    int sliderValue;
    bool isDragging;
    bool customMode;
    string inputText;
    bool inputFocused;
    string errorMsg;
    
    // Sort Screen State
    vector<int> originalArr;
    vector<Step> steps;
    int currentStep;
    int lastStep;
    float animProgress;
    bool playing;
    int speed;
    int timer;
    
    App() {
        screen = 0;
        sliderValue = DEFAULT_ARRAY_SIZE;
        isDragging = false;
        customMode = false;
        inputText = "";
        inputFocused = false;
        errorMsg = "";
        
        currentStep = 0;
        lastStep = 0;
        animProgress = 1.0f;
        playing = false;
        speed = 100;
        timer = 0;
    }

    bool parseCustom(const string& text, vector<int>& outArr) {
        outArr.clear();
        string clean = text;
        for (char& c : clean) if (c == ',') c = ' ';
        stringstream ss(clean);
        int num;
        while (ss >> num) {
            if (num < 1 || num > 999) return false;
            outArr.push_back(num);
        }
        if (outArr.size() < 2 || outArr.size() > 50) return false;
        return true;
    }

    void startSort(vector<int> arr) {
        originalArr = arr;
        steps = bubbleSort(arr);
        currentStep = 0;
        lastStep = 0;
        playing = false;
        timer = 0;
        animProgress = 1.0f;
        screen = 1; // Switch to sort screen
    }

};



void update(int screenW, int screenH) {
        float dt = GetFrameTime();
        Vector2 mPos = GetMousePosition();

        if (screen == 0) {
            // ── SELECT SCREEN INPUT ──
            int cx = screenW / 2;
            int sliderX = cx - 200;
            int sliderY = 300;
            int sliderW = 400;

            if (IsKeyPressed(KEY_TAB)) {
                customMode = !customMode;
                inputFocused = customMode;
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                inputFocused = false;
                
                // Algo Tabs bounding box rough check
                if (mPos.y > 140 && mPos.y < 180) {
                    int tw = 120, gap = 15;
                    int startX = cx - ((tw * 3 + gap * 2) / 2);
                    if (mPos.x > startX && mPos.x < startX + tw) algo = 0;
                    if (mPos.x > startX + tw + gap && mPos.x < startX + tw * 2 + gap) algo = 1;
                    if (mPos.x > startX + (tw + gap) * 2 && mPos.x < startX + tw * 3 + gap * 2) algo = 2;
                }

                // Input Mode Tabs bounding box rough check
                if (mPos.y > 190 && mPos.y < 230) {
                    if (mPos.x > cx - 140 && mPos.x < cx - 10) { customMode = false;}
                    if (mPos.x > cx + 10 && mPos.x < cx + 140) { customMode = true; inputFocused = true;}
                }
                
                // Start button
                if (mPos.y > 380 && mPos.y < 430 && mPos.x > cx - 100 && mPos.x < cx + 100) {
                    if (customMode) {
                        vector<int> arr;
                        if (parseCustom(inputText, arr)) startSort(arr);
                        else errorMsg = "Enter 2 to 50 numbers (1-999) separated by commas.";
                    } else {
                        startSort(makeRandomArray(sliderValue));
                    }
                }
                
                if (customMode) {
                    // Focus custom input box
                    if (mPos.x > cx - 250 && mPos.x < cx + 250 && mPos.y > 280 && mPos.y < 325) {
                        inputFocused = true;
                    }
                } else {
                    // Grab slider
                    int knobX = sliderX + (int)(((float)(sliderValue - MIN_ARRAY_SIZE) / (MAX_ARRAY_SIZE - MIN_ARRAY_SIZE)) * sliderW);
                    if (Vector2Distance(mPos, {(float)knobX, (float)sliderY}) < 20.0f) isDragging = true;
                }
            }

            if (isDragging) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float pct = (mPos.x - sliderX) / sliderW;
                    if (pct < 0.0f) pct = 0.0f;
                    if (pct > 1.0f) pct = 1.0f;
                    sliderValue = MIN_ARRAY_SIZE + (int)(pct * (MAX_ARRAY_SIZE - MIN_ARRAY_SIZE) + 0.5f);
                } else {
                    isDragging = false;
                }
            }

            if (inputFocused) {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= '0' && key <= '9') || key == ',' || key == ' ') {
                        inputText += (char)key;
                        errorMsg = "";
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
                    inputText.pop_back();
                    errorMsg = "";
                }
            }
            
            if (IsKeyPressed(KEY_ENTER)) {
                if (customMode) {
                    vector<int> arr;
                    if (parseCustom(inputText, arr)) startSort(arr);
                    else errorMsg = "Enter 2 to 50 numbers (1-999) separated by commas.";
                } else {
                    startSort(makeRandomArray(sliderValue));
                }
            }

        } else if (screen == 1) {
            // ── SORT SCREEN INPUT ──
            if (IsKeyPressed(KEY_SPACE)) playing = !playing;
            if (IsKeyPressed(KEY_RIGHT) && !playing && currentStep < (int)steps.size() - 1) currentStep++;
            if (IsKeyPressed(KEY_LEFT) && !playing && currentStep > 0) currentStep--;
            if (IsKeyPressed(KEY_UP)) { speed -= 20; if (speed < 20) speed = 20; }
            if (IsKeyPressed(KEY_DOWN)) { speed += 20; if (speed > 500) speed = 500; }
            if (IsKeyPressed(KEY_R)) { screen = 0; playing = false; } // Reset to Menu

            // Auto-play
            if (playing && currentStep < (int)steps.size() - 1) {
                timer += (int)(dt * 1000);
                if (timer >= speed) { currentStep++; timer = 0; }
            }

            // Animation interpolation
            if (currentStep != lastStep) { animProgress = 0.0f; lastStep = currentStep; }
            if (animProgress < 1.0f) {
                if (playing) animProgress = (float)timer / speed;
                else animProgress += (dt * 1000.0f) / 150.0f; 
                if (animProgress > 1.0f) animProgress = 1.0f;
            }
        }
    void draw(int screenW, int screenH) {
        ClearBackground(BG_COLOR);
        int cx = screenW / 2;

        if (screen == 0) {
            // ── SELECT SCREEN DRAW ──
            DrawTextSmoothCentered("Algorithm Visualizer", 0, 70, screenW, 36, TEXT_COLOR);
            DrawTextSmoothCentered("Cross-Platform C++ Architecture", 0, 115, screenW, 16, DIM_TEXT);
            
            int tw = 120, gap = 15;
            int startX = cx - ((tw * 3 + gap * 2) / 2);
            // Draw Algo Tabs
            DrawRoundedRect(startX, 140, tw, 40, 0.4f, algo == 0 ? ACCENT_COLOR : BORDER_COLOR);
            DrawRoundedRect(startX + tw + gap, 140, tw, 40, 0.4f, algo == 1 ? ACCENT_COLOR : BORDER_COLOR);
            DrawRoundedRect(startX + (tw + gap) * 2, 140, tw, 40, 0.4f, algo == 2 ? ACCENT_COLOR : BORDER_COLOR);
            DrawTextSmoothCentered("Bubble", startX, 150, tw, 20, TEXT_COLOR);
            DrawTextSmoothCentered("Insertion", startX + tw + gap, 150, tw, 20, TEXT_COLOR);
            DrawTextSmoothCentered("Selection", startX + (tw + gap) * 2, 150, tw, 20, TEXT_COLOR);
            
            int tx1 = cx - 140, tx2 = cx + 10;
            // Draw Input Tabs
            DrawRoundedRect(tx1, 190, 130, 40, 0.4f, customMode ? BORDER_COLOR : ACCENT_COLOR);
            DrawRoundedRect(tx2, 190, 130, 40, 0.4f, customMode ? ACCENT_COLOR : BORDER_COLOR);
            DrawTextSmoothCentered("Random", tx1, 200, 130, 20, TEXT_COLOR);
            DrawTextSmoothCentered("Custom", tx2, 200, 130, 20, TEXT_COLOR);

            if (!customMode) {
                // Slider UI
                string label = "Number of Bars: " + to_string(sliderValue);
                DrawTextSmoothCentered(label.c_str(), 0, 260, screenW, 20, TEXT_COLOR);
                
                int sw = 400; int sx = cx - sw/2; int sy = 300;
                DrawRoundedRect(sx, sy - 4, sw, 8, 1.0f, BORDER_COLOR); // Track
                
                int knobX = sx + (int)(((float)(sliderValue - MIN_ARRAY_SIZE) / (MAX_ARRAY_SIZE - MIN_ARRAY_SIZE)) * sw);
                DrawRoundedRect(sx, sy - 4, knobX - sx, 8, 1.0f, ACCENT_COLOR); // Fill
                DrawCircle(knobX, sy, 14, ACCENT_COLOR); // Knob
                DrawCircle(knobX, sy, 6, TEXT_COLOR); 
                
                DrawTextSmooth(to_string(MIN_ARRAY_SIZE).c_str(), sx - 30, sy - 10, 20, DIM_TEXT);
                DrawTextSmooth(to_string(MAX_ARRAY_SIZE).c_str(), sx + sw + 15, sy - 10, 20, DIM_TEXT);
            } else {
                // Custom Input UI
                DrawTextSmoothCentered("Enter comma separated numbers:", 0, 255, screenW, 18, TEXT_COLOR);
                
                int iw = 500; int ix = cx - iw/2; int iy = 280;
                DrawRoundedRect(ix, iy, iw, 45, 0.3f, PANEL_COLOR);
                DrawRoundedRectLines(ix, iy, iw, 45, 0.3f, 2.0f, inputFocused ? ACCENT_COLOR : BORDER_COLOR);
                
                string display = inputText;
                if (inputFocused && (int)(GetTime() * 2) % 2 == 0) display += "|";
                if (display.empty() && !inputFocused) DrawTextSmooth("e.g. 5, 10, 15, 2", ix + 15, iy + 12, 20, DIM_TEXT);
                else DrawTextSmooth(display.c_str(), ix + 15, iy + 12, 20, TEXT_COLOR);
                
                if (!errorMsg.empty()) DrawTextSmoothCentered(errorMsg.c_str(), 0, iy + 55, screenW, 16, BAR_COMPARING);
            }

            // Start Button
            int bw = 200; int bx = cx - bw/2; int by = 380;
            bool hovering = (GetMousePosition().x > bx && GetMousePosition().x < bx + bw && GetMousePosition().y > by && GetMousePosition().y < by + 50);
            DrawRoundedRect(bx, by, bw, 50, 0.4f, hovering ? BAR_DONE : ACCENT_COLOR);
            DrawTextSmoothCentered("Start Sort", bx, by + 12, bw, 24, BG_COLOR);
            
        } else if (screen == 1) {
            // ── SORT SCREEN DRAW ──
            Step& cur = steps[currentStep];
            bool isDone = (cur.compared == -1);
            int swaps = countSwaps(steps, currentStep);
            
            // Header Top Bar
            DrawRoundedRect(0, 0, screenW, 70, 0.0f, PANEL_COLOR);
            if (algo == 0) DrawTextSmooth("Bubble Sort", 20, 18, 32, TEXT_COLOR);
            else if (algo == 1) DrawTextSmooth("Insertion Sort", 20, 18, 32, TEXT_COLOR);
            else DrawTextSmooth("Selection Sort", 20, 18, 32, TEXT_COLOR);
            if (isDone) DrawTextSmooth("COMPLETE", screenW - 160, 22, 26, BAR_DONE);

            // ── ARRAY TEXT DISPLAY (Extra Large, Highlighted) ──
            string origText = "Original: ";
            for (size_t i = 0; i < originalArr.size(); i++) origText += to_string(originalArr[i]) + (i < originalArr.size() - 1 ? ", " : "");
            
            string currText = "Current : ";
            for (size_t i = 0; i < cur.arr.size(); i++) currText += to_string(cur.arr[i]) + (i < cur.arr.size() - 1 ? ", " : "");
            
            int maxTextW = screenW - 40;
            auto truncateStr = [&](string s) {
                if (MeasureTextSmooth(s.c_str(), 24) <= maxTextW) return s;
                while (s.length() > 0 && MeasureTextSmooth((s + "...").c_str(), 24) > maxTextW) s.pop_back();
                return s + "...";
            };

            DrawTextSmooth(truncateStr(origText).c_str(), 20, 85, 24, DIM_TEXT);
            if (!isDone) {
                DrawTextSmooth(truncateStr(currText).c_str(), 20, 120, 24, ACCENT_COLOR);
            } else {
                string sortText = "Sorted  : ";
                for (size_t i = 0; i < cur.arr.size(); i++) sortText += to_string(cur.arr[i]) + (i < cur.arr.size() - 1 ? ", " : "");
                DrawTextSmooth(truncateStr(sortText).c_str(), 20, 120, 24, BAR_DONE);
            }

            // Status Indicator (Swapping/Comparing)
            int labelY = 160;
            if (!isDone) {
                if (cur.swapped) DrawTextSmoothCentered("SWAPPING", 0, labelY, screenW, 30, BAR_COMPARING); // Neon Red/Pink
                else DrawTextSmoothCentered("NO SWAPPING", 0, labelY, screenW, 30, BAR_DONE); // Neon Green
            }

            // Draw Rounded Vibrant Bars
            int n = cur.arr.size();
            int areaTop = labelY + 45;
            int areaBottom = screenH - 100;
            int areaH = areaBottom - areaTop;
            int maxVal = *max_element(originalArr.begin(), originalArr.end());
            if (maxVal < 1) maxVal = 1;
            
            float exactBarW = (float)(screenW - 40) / n;
            float pad = exactBarW * 0.15f; // Gap between bars
            
            for (int i = 0; i < n; i++) {
                int val = cur.arr[i];
                float barH = ((float)val / maxVal) * areaH;
                if (barH < 5) barH = 5; // minimum height
                
                float x = 20.0f + i * exactBarW;
                float y = areaBottom - barH;
                
                Color c = isDone ? BAR_DONE : BAR_NORMAL;
                
                bool isActive = false;
                if (!isDone) {
                    if (cur.compared_sec == -1) {
                        isActive = (i == cur.compared || i == cur.compared + 1);
                    } else {
                        isActive = (i == cur.compared || i == cur.compared_sec);
                    }
                }

                if (isActive) {
                    c = cur.swapped ? BAR_SWAPPED : BAR_COMPARING;
                    
                    // Smooth Transition Slider
                    if (animProgress < 1.0f && cur.swapped) {
                        int otherIdx = -1;
                        if (cur.compared_sec == -1) {
                            otherIdx = (i == cur.compared) ? (cur.compared + 1) : cur.compared;
                        } else {
                            otherIdx = (i == cur.compared) ? cur.compared_sec : cur.compared;
                        }
                        
                        float startX = 20.0f + otherIdx * exactBarW;
                        float easedT = 1.0f - (1 - animProgress) * (1 - animProgress) * (1 - animProgress);
                        x = startX + (x - startX) * easedT;
                    }
                }
                
                // Draw precisely rounded rects!
                DrawRoundedRect((int)(x + pad), (int)y, (int)(exactBarW - pad * 2.0f), (int)barH, 0.25f, c);
                
                // Value Label above/inside
                string numStr = to_string(val);
                int tw = MeasureTextSmooth(numStr.c_str(), 18);
                float tx = (x + pad) + ((exactBarW - pad*2) / 2.0f) - (tw / 2.0f);
                float ty = (barH > 24) ? y + 6 : y - 22;
                DrawTextSmooth(numStr.c_str(), (int)tx, (int)ty, 18, TEXT_COLOR);
            }

            // Footer Bottom Bar
            int footH = 100;
            DrawRoundedRect(0, screenH - footH, screenW, footH, 0.0f, PANEL_COLOR);
            
            float p = (steps.size() > 1) ? (float)currentStep / (steps.size() - 1) : 0.0f;
            DrawRoundedRect(20, screenH - 95, screenW - 40, 6, 1.0f, BORDER_COLOR);
            DrawRoundedRect(20, screenH - 95, (int)(p * (screenW - 40)), 6, 1.0f, isDone ? BAR_DONE : ACCENT_COLOR);

            // Left Side: Status Info
            string state = isDone ? "DONE" : (playing ? "PLAYING" : "PAUSED");
            string info = state + "   Steps: " + to_string(currentStep) + "/" + to_string(steps.size()-1) + 
                          "   Swaps: " + to_string(swaps) + "   Speed: " + to_string(speed) + "ms";
            DrawTextSmoothCentered(info.c_str(), 0, screenH - 78, screenW, 22, TEXT_COLOR);
            
            // Highlighted Guide Texts directly below
            string hint = "Space: Play | Left/Right: Step | Up/Down: Speed | R: Back To Menu";
            DrawTextSmoothCentered(hint.c_str(), 0, screenH - 45, screenW, 24, ACCENT_COLOR);
        }
    }

};    
    
#endif
