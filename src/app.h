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

            
    }
