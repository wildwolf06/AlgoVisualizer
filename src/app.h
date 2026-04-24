#ifndef APP_H
#define APP_H

#include "raylib.h"
#include "raymath.h"
#include "constants.h"
#include "sorting.h"
#include "searching.h"
#include "graph.h"
#include "ui.h"
#include <string>
#include <sstream>

using namespace std;

// ── APPLICATION STATE ──
class App {
public:
    int screen; // 0=Select, 1=Sort, 2=Search, 3=Graph
    int menuState; // 0=Category, 1=Algo, 2=Input
    int category; // 0=Sorting, 1=Searching, 2=Graphs (Graphs)
    int algo; 
    
    // Select Screen State
    int sliderValue;
    bool isDraggingSize;
    bool isDraggingSpeed;
    bool customMode;
    string inputText;
    bool inputFocused;
    string searchTargetText;
    bool searchTargetFocused;
    string errorMsg;
    
    // Sorting Dashboard State
    enum DistributionType { RANDOM_DIST, SORTED_NON_DECREASING, SORTED_NON_INCREASING, MANY_DUPLICATES };
    DistributionType current_distribution;
    string manual_array_buffer;
    string num_elements_buffer;
    string target_buffer;
    int active_input_field; // 0: none, 1: N buffer, 2: manual buffer, 3: target buffer
    int comparison_count;
    int swap_count;
    string theoretical_complexity_str;
    char realtime_status_msg[128];
    bool sort_ascending;
    
    // Screen 1: Sort
    vector<int> originalArr;
    vector<Step> steps;
    int currentStep;
    
    // Screen 2: Search
    vector<SearchStep> searchSteps;
    int searchTarget;

    // Screen 3: Graphs
    vector<GraphNode> graphNodes;
    vector<GraphStep> graphSteps;

    // Common Animation State
    int lastStep;
    float animProgress;
    bool playing;
    int speed;
    int timer;
    
    App() {
        screen = 0;
        menuState = 0;
        category = 0;
        algo = 0;
        sliderValue = DEFAULT_ARRAY_SIZE;
        isDraggingSize = false;
        isDraggingSpeed = false;
        customMode = false;
        inputText = "";
        inputFocused = false;
        searchTargetText = "";
        searchTargetFocused = false;
        errorMsg = "";
        
        current_distribution = RANDOM_DIST;
        manual_array_buffer = "";
        num_elements_buffer = "20";
        active_input_field = 0;
        comparison_count = 0;
        swap_count = 0;
        theoretical_complexity_str = "O(N^2)";
        memset(realtime_status_msg, 0, sizeof(realtime_status_msg));
        sort_ascending = true;
        
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

    vector<int> parseManualInput(string input) {
        vector<int> res;
        string current = "";
        for (char c : input) {
            if (c == ',' || c == ' ') {
                if (!current.empty()) {
                    try { res.push_back(stoi(current)); } catch (...) {}
                    current = "";
                }
            } else if (c >= '0' && c <= '9') {
                current += c;
            }
        }
        if (!current.empty()) {
            try { res.push_back(stoi(current)); } catch (...) {}
        }
        return res;
    }
void startSort(vector<int> arr) {
        if (arr.empty()) return;
        originalArr = arr;
        if (algo == 0) { steps = bubbleSort(arr, sort_ascending); theoretical_complexity_str = "O(N^2)"; }
        else if (algo == 1) { steps = insertionSort(arr, sort_ascending); theoretical_complexity_str = "O(N^2)"; }
        else if (algo == 2) { steps = selectionSort(arr, sort_ascending); theoretical_complexity_str = "O(N^2)"; }
        
        setupAnimation();
        screen = 1;
        currentStep = 0;
        playing = true;
        timer = 0;
        comparison_count = 0;
        swap_count = 0;
        memset(realtime_status_msg, 0, sizeof(realtime_status_msg));
    }

    void startSearch(vector<int> arr, int target) {
        originalArr = arr;
        sort(originalArr.begin(), originalArr.end());
        searchTarget = target;
        searchSteps = binarySearch(originalArr, searchTarget);
        setupAnimation();
        speed = 400; // Search goes a little slower
        screen = 2;
    }

    void startGraphDFS_BFS(int d) {
        graphNodes = generateGraph(d);
        if (algo == 0) {
            graphSteps = dfs(graphNodes, 0); // DFS
        } else {
            graphSteps = bfs(graphNodes, 0); // BFS
        }
        setupAnimation();
        speed = 800; // Slower for clear graph traversal view
        screen = 3;
    }

    void setupAnimation() {
        currentStep = 0;
        lastStep = 0;
        playing = false;
        timer = 0;
        animProgress = 1.0f;
        speed = 100;
    }
    
    void updateStatusMessage() {
        if (screen != 1 || steps.empty()) return;
        memset(realtime_status_msg, 0, sizeof(realtime_status_msg));
        
        Step& cur = steps[currentStep];
        if (cur.compared == -1) {
            snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Sorted!");
            return;
        }
        
        int idx1 = cur.compared;
        int idx2 = (cur.compared_sec != -1) ? cur.compared_sec : cur.compared + 1;
        
        if (idx1 >= 0 && idx1 < (int)cur.arr.size() && idx2 >= 0 && idx2 < (int)cur.arr.size()) {
            int v1 = cur.arr[idx1];
            int v2 = cur.arr[idx2];
            
            const char* op = cur.sortAscending ? ">" : "<";
            
            if (cur.swapped) {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Comparing Index %d (Val: %d) and Index %d (Val: %d). Swapping because %d %s %d.", idx1, v1, idx2, v2, v1, op, v2);
            } else if (cur.isComparison) {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Comparing Index %d (Val: %d) and Index %d (Val: %d).", idx1, v1, idx2, v2);
            } else {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Selecting Index %d (Val: %d).", idx1, v1);
            }
        }
    }

    void updateSearchStatusMessage() {
        if (screen != 2 || searchSteps.empty()) return;
        memset(realtime_status_msg, 0, sizeof(realtime_status_msg));
        
        SearchStep& cur = searchSteps[currentStep];
        
        if (cur.done) {
            if (cur.found) {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Target found at index %d.", cur.mid);
            } else {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Target not found. Search space exhausted. Returning -1.");
            }
            return;
        }
        
        if (cur.mid >= 0 && cur.mid < (int)cur.arr.size()) {
            int midVal = cur.arr[cur.mid];
            if (cur.target > midVal) {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Target (%d) > Mid Value (%d). Discarding left half. Moving lo to mid + 1.", cur.target, midVal);
            } else if (cur.target < midVal) {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Target (%d) < Mid Value (%d). Discarding right half. Moving hi to mid - 1.", cur.target, midVal);
            } else {
                snprintf(realtime_status_msg, sizeof(realtime_status_msg), "Target (%d) == Mid Value (%d). Match found!", cur.target, midVal);
            }
        }
    }
void update(int screenW, int screenH) {
        float dt = GetFrameTime();
        Vector2 mPos = GetMousePosition();

        if (screen == 0) {
            // ── SELECT SCREEN INPUT ──
            int cx = screenW / 2;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                int catY = 180, catW = 260, catH = 340, catGap = 40;
                int catStartX = cx - ((catW * 3 + catGap * 2) / 2);
                for (int i=0; i<3; i++) {
                    int cx_cat = catStartX + i * (catW + catGap);
                    if (mPos.y > catY && mPos.y < catY + catH && mPos.x > cx_cat && mPos.x < cx_cat + catW) {
                        category = i; 
                        algo = 0; 
                        sliderValue = (i == 2) ? 4 : 20;
                        customMode = false;
                        
                        // Immediately launch the dashboard
                        if (category == 0) startSort(makeRandomArray(sliderValue));
                        else if (category == 1) {
                            vector<int> arr = makeRandomArray(sliderValue);
                            startSearch(arr, arr[rand() % arr.size()]);
                        }
                        else if (category == 2) startGraphDFS_BFS(sliderValue);
                    }
                }
            }
        } else {
            // VISUALIZATION SCREENS INPUT 
            int totalSteps = 0;
            if (screen == 1) totalSteps = steps.size();
            else if (screen == 2) totalSteps = searchSteps.size();
            else if (screen == 3) totalSteps = graphSteps.size();
            
            int oldStep = currentStep;

            // Keyboard input
            if (IsKeyPressed(KEY_SPACE)) playing = !playing;
            if (IsKeyPressed(KEY_RIGHT) && !playing && currentStep < totalSteps - 1) currentStep++;
            if (IsKeyPressed(KEY_LEFT) && !playing && currentStep > 0) currentStep--;
            if (IsKeyPressed(KEY_UP)) { speed -= 20; if (speed < 20) speed = 20; }
            if (IsKeyPressed(KEY_DOWN)) { speed += 20; if (speed > 1000) speed = 1000; }
            if (IsKeyPressed(KEY_R)) { screen = 0; playing = false; } 

            // Mouse Input for Dashboard
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (screen != 1) {
                    if (mPos.x > 10 && mPos.x < 110 && mPos.y > 10 && mPos.y < 50) {
                        screen = 0; playing = false;
                    }
                    int numAlgos = (category == 0) ? 3 : (category == 1 ? 1 : 2);
                    int startX = 130;
                    for (int i=0; i<numAlgos; i++) {
                        int ax = startX + i * 130;
                        if (mPos.x > ax && mPos.x < ax + 120 && mPos.y > 10 && mPos.y < 50) {
                            if (algo != i) {
                                algo = i;
                                if (category == 0) {}
                                else if (category == 1) { vector<int> arr = makeRandomArray(sliderValue); startSearch(arr, arr[rand() % arr.size()]); }
                                else if (category == 2) startGraphDFS_BFS(sliderValue);
                            }
                        }
                    }
                    int rightX = screenW - 10;
                    if (mPos.x > rightX - 50 && mPos.x < rightX - 10 && mPos.y > 10 && mPos.y < 50) playing = !playing;
                    rightX -= 140; // Past play and random
                    if (mPos.x > rightX && mPos.x < rightX + 80 && mPos.y > 10 && mPos.y < 50) { // Random
                        if (category == 0) {}
                        else if (category == 1) { vector<int> arr = makeRandomArray(sliderValue); startSearch(arr, arr[rand() % arr.size()]); }
                        else if (category == 2) startGraphDFS_BFS(sliderValue);
                    }
                    rightX -= 90; // Custom (stub)
                    rightX -= 140; // Speed Slider center
                    if (mPos.x > rightX && mPos.x < rightX + 120 && mPos.y > 20 && mPos.y < 50) isDraggingSpeed = true;
                    rightX -= 140; // Size Slider center
                    if (mPos.x > rightX && mPos.x < rightX + 120 && mPos.y > 20 && mPos.y < 50) isDraggingSize = true;
                }
            }
            
            if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
                isDraggingSpeed = false;
                isDraggingSize = false;
            }
            
            if (isDraggingSpeed) {
                if (screen == 1) {
                    float pct = (mPos.x - 30) / 200.0f;
                    if (pct < 0) pct = 0; if (pct > 1) pct = 1;
                    speed = 1000 - (int)(pct * 980);
                    if (speed < 20) speed = 20;
                } else {
                    int rightX = screenW - 10 - 50 - 90 - 90 - 140; 
                    float pct = (mPos.x - rightX) / 120.0f;
                    if (pct < 0) pct = 0; if (pct > 1) pct = 1;
                    speed = 1000 - (int)(pct * 980);
                    if (speed < 20) speed = 20;
                }
            }
            if (isDraggingSize) {
                int rightX = screenW - 10 - 50 - 90 - 90 - 140 - 140;
                float pct = (mPos.x - rightX) / 120.0f;
                if (pct < 0) pct = 0; if (pct > 1) pct = 1;
                int minV = MIN_ARRAY_SIZE, maxV = MAX_ARRAY_SIZE;
                if (category == 2) { minV = 2; maxV = 7; }
                sliderValue = minV + (int)(pct * (maxV - minV) + 0.5f);
            }

            // Screen 1 & 2 Left Panel Input & Typing
            if (screen == 1 || screen == 2) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Focus inputs
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 135 && mPos.y < 165) active_input_field = 1; // N
                    else if (mPos.x > 30 && mPos.x < 230 && mPos.y > 255 && mPos.y < 285) active_input_field = 2; // Manual
                    else if (screen == 2 && mPos.x > 30 && mPos.x < 230 && mPos.y > 400 && mPos.y < 430) active_input_field = 3; // Target
                    else active_input_field = 0;
                    
                    // Distribution toggle
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 195 && mPos.y < 225) {
                        current_distribution = (DistributionType)((current_distribution + 1) % 4);
                    }
                    
                    // CREATE button
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 300 && mPos.y < 340) {
                        vector<int> newArr;
                        if (!manual_array_buffer.empty()) {
                            newArr = parseManualInput(manual_array_buffer);
                        }
                        if (newArr.empty()) {
                            int N = 20;
                            try { N = stoi(num_elements_buffer); } catch(...) {}
                            if (N < 2) N = 2; if (N > 200) N = 200;
                            if (current_distribution == RANDOM_DIST) newArr = makeRandomArray(N);
                            else if (current_distribution == SORTED_NON_DECREASING) newArr = makeSortedArray(N, true);
                            else if (current_distribution == SORTED_NON_INCREASING) newArr = makeSortedArray(N, false);
                            else if (current_distribution == MANY_DUPLICATES) newArr = makeDuplicatesArray(N);
                        }
                        
                        if (screen == 2) {
                            sort(newArr.begin(), newArr.end()); // Enforce sorted array for binary search
                        }
                        
                        originalArr = newArr;
                        if (screen == 1) {
                            steps.clear();
                            currentStep = 0;
                            playing = false;
                        } else {
                            searchSteps.clear();
                            currentStep = 0;
                            playing = false;
                        }
                        timer = 0;
                        theoretical_complexity_str = "";
                        memset(realtime_status_msg, 0, sizeof(realtime_status_msg));
                    }
                    
                    if (screen == 1) {
                        // Algorithm toggle (Left Panel)
                        if (mPos.x > 30 && mPos.x < 230 && mPos.y > 380 && mPos.y < 410) {
                            algo = (algo + 1) % 3;
                        }
                        
                        // Order toggle (Left Panel)
                        if (mPos.x > 30 && mPos.x < 230 && mPos.y > 420 && mPos.y < 450) {
                            sort_ascending = !sort_ascending;
                        }
                        
                        // SORT button
                        if (mPos.x > 30 && mPos.x < 230 && mPos.y > 460 && mPos.y < 520) {
                            if (!originalArr.empty()) {
                                startSort(originalArr);
                            }
                        }
                    } else if (screen == 2) {
                        // SEARCH button
                        if (mPos.x > 30 && mPos.x < 230 && mPos.y > 460 && mPos.y < 520) {
                            if (!originalArr.empty() && !target_buffer.empty()) {
                                int t = 0;
                                try { t = stoi(target_buffer); } catch(...) {}
                                startSearch(originalArr, t);
                            }
                        }
                    }
                    
                    // Speed Slider (Left Panel)
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 560 && mPos.y < 610) {
                        isDraggingSpeed = true;
                    }
                    
                    // Bottom Playback Controls (Prev, Play, Next)
                    int pbY = screenH - 60;
                    int pbCX = screenW / 2;
                    // Prev: cx - 150, width 80
                    if (mPos.x > pbCX - 150 && mPos.x < pbCX - 70 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (screen == 1 && !steps.empty() && currentStep > 0) { currentStep--; playing = false; }
                        else if (screen == 2 && !searchSteps.empty() && currentStep > 0) { currentStep--; playing = false; }
                    }
                    // Play/Pause: cx - 60, width 120
                    if (mPos.x > pbCX - 60 && mPos.x < pbCX + 60 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (screen == 1 && !steps.empty()) {
                            if (currentStep >= totalSteps - 1) { currentStep = 0; playing = true; } else { playing = !playing; }
                        } else if (screen == 2 && !searchSteps.empty()) {
                            if (currentStep >= totalSteps - 1) { currentStep = 0; playing = true; } else { playing = !playing; }
                        }
                    }
                    // Next: cx + 70, width 80
                    if (mPos.x > pbCX + 70 && mPos.x < pbCX + 150 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (screen == 1 && !steps.empty() && currentStep < totalSteps - 1) { currentStep++; playing = false; }
                        else if (screen == 2 && !searchSteps.empty() && currentStep < totalSteps - 1) { currentStep++; playing = false; }
                    }
                    
                    // Top Left Menu Button
                    if (mPos.x > 10 && mPos.x < 110 && mPos.y > 10 && mPos.y < 50) {
                        screen = 0; playing = false;
                    }
                }
                
                // Typing
                if (active_input_field > 0) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if ((key >= 32) && (key <= 125)) {
                            if (active_input_field == 1 && num_elements_buffer.length() < 9 && key >= '0' && key <= '9') {
                                num_elements_buffer += (char)key;
                            } else if (active_input_field == 2 && manual_array_buffer.length() < 250) {
                                manual_array_buffer += (char)key;
                            } else if (active_input_field == 3 && target_buffer.length() < 9 && (key == '-' || (key >= '0' && key <= '9'))) {
                                target_buffer += (char)key;
                            }
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        if (active_input_field == 1 && !num_elements_buffer.empty()) num_elements_buffer.pop_back();
                        if (active_input_field == 2 && !manual_array_buffer.empty()) manual_array_buffer.pop_back();
                        if (active_input_field == 3 && !target_buffer.empty()) target_buffer.pop_back();
                    }
                }
            }

            // Auto-play
            if (playing && currentStep < totalSteps - 1) {
                timer += (int)(dt * 1000);
                if (timer >= speed) {
                    currentStep++; 
                    timer = 0; 
                }
            }
            
            if (currentStep != oldStep) {
                if (screen == 1) updateStatusMessage();
                else if (screen == 2) updateSearchStatusMessage();
            }
        }
    }
void startExecution() {
        if (category == 0) {
            if (customMode) {
                vector<int> arr;
                if (parseCustom(inputText, arr)) startSort(arr);
                else errorMsg = "Enter 2 to 50 numbers (1-999) separated by commas.";
            } else {
                startSort(makeRandomArray(sliderValue));
            }
        } else if (category == 1) {
            int target = -1;
            if (!searchTargetText.empty()) {
                target = stoi(searchTargetText);
            }
            if (customMode) {
                vector<int> arr;
                if (parseCustom(inputText, arr)) {
                    if (target == -1) target = arr[rand() % arr.size()];
                    startSearch(arr, target);
                }
                else errorMsg = "Enter 2 to 50 numbers (1-999) separated by commas.";
            } else {
                vector<int> arr = makeRandomArray(sliderValue);
                if (target == -1) target = arr[rand() % arr.size()];
                startSearch(arr, target);
            }
        } else if (category == 2) {
            startGraphDFS_BFS(sliderValue);
        }
    }

    void drawDashboardTopBar(int screenW, Vector2 mPos) {
        DrawRectangle(0, 0, screenW, 60, DASH_PANEL);
        DrawRectangle(0, 60, screenW, 1, DASH_BORDER);
        
        bool backHover = (mPos.x > 10 && mPos.x < 110 && mPos.y > 10 && mPos.y < 50);
        DrawRoundedRectLines(10, 10, 100, 40, 0.2f, 1.0f, backHover ? DASH_ACCENT : DASH_BORDER);
        DrawTextSmoothCentered("< MENU", 10, 20, 100, 20, backHover ? DASH_ACCENT : DASH_TEXT);
        
        int numAlgos = (category == 0) ? 3 : (category == 1 ? 1 : 2);
        string sortNames[] = {"Bubble", "Insertion", "Selection"};
        string searchNames[] = {"Binary Search", ""};
        string graphNames[] = {"DFS", "BFS"};
        string* names = (category == 0) ? sortNames : (category == 1 ? searchNames : graphNames);
        
        int startX = 130; 
        for (int i=0; i<numAlgos; i++) {
            int ax = startX + i * 130;
            bool hover = (mPos.x > ax && mPos.x < ax + 120 && mPos.y > 10 && mPos.y < 50);
            bool active = (algo == i);
            Color bg = active ? DASH_ACCENT : (hover ? DASH_BG : DASH_PANEL);
            Color textCol = active ? DASH_PANEL : DASH_TEXT;
            DrawRoundedRect(ax, 10, 120, 40, 0.2f, bg);
            if (!active) DrawRoundedRectLines(ax, 10, 120, 40, 0.2f, 1.0f, hover ? DASH_ACCENT : DASH_BORDER);
            DrawTextSmoothCentered(names[i].c_str(), ax, 20, 120, 18, textCol);
        }
        
        int rightX = screenW - 10;
        
        rightX -= 50;
        bool playHover = (mPos.x > rightX && mPos.x < rightX + 40 && mPos.y > 10 && mPos.y < 50);
        DrawRoundedRect(rightX, 10, 40, 40, 0.2f, playHover ? DASH_BG : DASH_PANEL);
        DrawRoundedRectLines(rightX, 10, 40, 40, 0.2f, 1.0f, DASH_BORDER);
        if (playing) {
            DrawRectangle(rightX + 13, 22, 5, 16, DASH_ACCENT);
            DrawRectangle(rightX + 22, 22, 5, 16, DASH_ACCENT);
        } else {
            DrawTriangle({(float)rightX + 15, 20}, {(float)rightX + 15, 40}, {(float)rightX + 30, 30}, DASH_ACCENT);
        }
        
        rightX -= 90;
        bool rndHover = (mPos.x > rightX && mPos.x < rightX + 80 && mPos.y > 10 && mPos.y < 50);
        DrawRoundedRect(rightX, 10, 80, 40, 0.2f, rndHover ? DASH_BG : DASH_PANEL);
        DrawRoundedRectLines(rightX, 10, 80, 40, 0.2f, 1.0f, DASH_BORDER);
        DrawTextSmoothCentered("RANDOM", rightX, 22, 80, 16, DASH_TEXT);
        
        rightX -= 90;
        bool cstHover = (mPos.x > rightX && mPos.x < rightX + 80 && mPos.y > 10 && mPos.y < 50);
        DrawRoundedRect(rightX, 10, 80, 40, 0.2f, cstHover ? DASH_BG : DASH_PANEL);
        DrawRoundedRectLines(rightX, 10, 80, 40, 0.2f, 1.0f, DASH_BORDER);
        DrawTextSmoothCentered("CUSTOM", rightX, 22, 80, 16, DASH_DIM);
        
        rightX -= 140;
        DrawTextSmoothCentered("SPEED", rightX, 12, 120, 14, DASH_DIM);
        DrawRoundedRect(rightX, 35, 120, 4, 1.0f, DASH_BORDER);
        float speedPct = 1.0f - ((float)(speed - 20) / 980.0f);
        DrawRoundedRect(rightX, 35, (int)(speedPct * 120), 4, 1.0f, DASH_ACCENT);
        DrawCircle(rightX + (int)(speedPct * 120), 37, 8, DASH_ACCENT);
        
        rightX -= 140;
        DrawTextSmoothCentered("SIZE", rightX, 12, 120, 14, DASH_DIM);
        DrawRoundedRect(rightX, 35, 120, 4, 1.0f, DASH_BORDER);
        int minV = MIN_ARRAY_SIZE, maxV = MAX_ARRAY_SIZE;
        if (category == 2) { minV = 2; maxV = 7; }
        float sizePct = (float)(sliderValue - minV) / (maxV - minV);
        DrawRoundedRect(rightX, 35, (int)(sizePct * 120), 4, 1.0f, DASH_ACCENT);
        DrawCircle(rightX + (int)(sizePct * 120), 37, 8, DASH_ACCENT);
    }
