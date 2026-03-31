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
    }
