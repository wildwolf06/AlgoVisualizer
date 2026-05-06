#ifndef APP_H
#define APP_H

#include "raylib.h"
#include "raymath.h"
#include "constants.h"
#include "sorting.h"
#include "searching.h"
#include "graph.h"
#include "bitmask.h"
#include "pathfinding.h"
#include "dp.h"
#include "backtracking.h"
#include "ui.h"
#include <string>
#include <sstream>
#include <random>

using namespace std;

// ── APPLICATION STATE ──
class App {
public:
    int screen; // 0=Select, 1=Sort, 2=Search, 3=Graph, 4=Bitmask
    int menuState; // 0=Category, 1=Algo, 2=Input
    int category; // 0=Sorting, 1=Searching, 2=Graphs, 3=Bitmask
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

    // Screen 4: Bitmask
    int bitmask_S;
    int bitmask_j;
    string bit_input_buffer;
    int active_bitmask_op; 
    vector<BitmaskStep> bitmask_steps;

    // Screen 5: Pathfinding
    int grid_rows;
    int grid_cols;
    vector<vector<bool>> walls;
    pair<int, int> start_node;
    pair<int, int> end_node;
    vector<GridStep> path_steps;

    // Screen 6: Dynamic Programming
    int dp_capacity;
    vector<Item> dp_items;
    vector<DPStep> dp_steps;
    string dp_capacity_buffer;

    // Screen 7: Backtracking (N-Queens)
    int n_queens_size;
    vector<QueenStep> queen_steps;
    float nqueens_anim_speed;
    float nqueens_timer;
    bool is_dragging_nqueens_speed;



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

        bitmask_S = 573;
        bitmask_j = 0;
        bit_input_buffer = "";
        active_bitmask_op = 0;
        
        grid_rows = 15;
        grid_cols = 25;
        walls = vector<vector<bool>>(grid_rows, vector<bool>(grid_cols, false));
        start_node = {7, 5};
        end_node = {7, 20};
        
        dp_capacity = 8;
        dp_items = {{2, 3}, {3, 4}, {4, 5}, {5, 6}};
        dp_capacity_buffer = "8";
        
        n_queens_size = 4;
        nqueens_anim_speed = 0.5f;
        nqueens_timer = 0.0f;
        is_dragging_nqueens_speed = false;
        

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

    void startBitmask() {
        if (active_bitmask_op == 0) bitmask_steps = incrementBits(bitmask_S);
        else if (active_bitmask_op == 1) { 
            bitmask_steps.clear();
            bitmask_steps.push_back({bitmask_S, 0, bitmask_S, -1, "Set S to " + to_string(bitmask_S) + ".", "+"});
        }
        else if (active_bitmask_op == 2) bitmask_steps = setBit(bitmask_S, bitmask_j);
        else if (active_bitmask_op == 3) bitmask_steps = checkBit(bitmask_S, bitmask_j);
        else if (active_bitmask_op == 4) bitmask_steps = clearBit(bitmask_S, bitmask_j);
        else if (active_bitmask_op == 5) bitmask_steps = toggleBit(bitmask_S, bitmask_j);
        else if (active_bitmask_op == 6) bitmask_steps = leastSignificantBit(bitmask_S);
        
        setupAnimation();
        speed = 400; 
        screen = 4;
        playing = false;
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
                int catW = 220, catH = 220, catGap = 30;
                for (int i=0; i<7; i++) {
                    int itemsInRow = 4;
                    int row = (i < 4) ? 0 : 1;
                    int col = (i < 4) ? i : (i - 4);
                    int rowW = itemsInRow * catW + (itemsInRow - 1) * catGap;
                    int startX = cx - rowW / 2;
                    int cx_cat = startX + col * (catW + catGap);
                    int cy_cat = 160 + row * (catH + catGap);
                    
                    if (mPos.y > cy_cat && mPos.y < cy_cat + catH && mPos.x > cx_cat && mPos.x < cx_cat + catW) {
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
                        else if (category == 3) startBitmask();
                        else if (category == 4) { screen = 5; playing = false; path_steps.clear(); }
                        else if (category == 5) { screen = 6; playing = false; dp_steps.clear(); }
                        else if (category == 6) { screen = 7; playing = false; queen_steps.clear(); }
                    }
                }
            }
        } else {
            // VISUALIZATION SCREENS INPUT 
            int totalSteps = 0;
            if (screen == 1) totalSteps = steps.size();
            else if (screen == 2) totalSteps = searchSteps.size();
            else if (screen == 3) totalSteps = graphSteps.size();
            else if (screen == 4) totalSteps = bitmask_steps.size();
            else if (screen == 5) totalSteps = path_steps.size();
            else if (screen == 6) totalSteps = dp_steps.size();
            else if (screen == 7) totalSteps = queen_steps.size();

            
            int oldStep = currentStep;

            // Keyboard input
            if (IsKeyPressed(KEY_SPACE)) playing = !playing;
            if (IsKeyPressed(KEY_RIGHT) && !playing && currentStep < totalSteps - 1) currentStep++;
            if (IsKeyPressed(KEY_LEFT) && !playing && currentStep > 0) currentStep--;
            if (IsKeyPressed(KEY_UP)) { speed -= 20; if (speed < 20) speed = 20; }
            if (IsKeyPressed(KEY_DOWN)) { speed += 20; if (speed > 1000) speed = 1000; }
            if (IsKeyPressed(KEY_R)) { screen = 0; playing = false; } 

            // Mouse Input for Dashboard
            Vector2 mPos = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (screen != 1 && screen != 4) {
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
            } else if (screen == 4) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Left Panel Input
                    for (int i=0; i<7; i++) {
                        int by = 135 + i * 50;
                        bool needsInput = (i == 1 || (i >= 2 && i <= 5));
                        bool isActive = (active_bitmask_op == i);
                        
                        int btnW = (isActive && needsInput) ? 120 : 200;
                        
                        if (mPos.x > 30 && mPos.x < 30 + btnW && mPos.y > by && mPos.y < by + 40) {
                            active_bitmask_op = i;
                            bit_input_buffer = "";
                            active_input_field = needsInput ? 4 : 0; 
                            if (!needsInput) startBitmask();
                        }
                        
                        if (isActive && needsInput) {
                            if (mPos.x > 180 && mPos.x < 215 && mPos.y > by && mPos.y < by + 40) {
                                active_input_field = 4; // Text box click
                            }
                            if (mPos.x > 220 && mPos.x < 250 && mPos.y > by && mPos.y < by + 40) {
                                if (!bit_input_buffer.empty()) { // Go click
                                    if (i == 1) bitmask_S = stoi(bit_input_buffer);
                                    else bitmask_j = stoi(bit_input_buffer);
                                    startBitmask();
                                    active_input_field = 0;
                                }
                            }
                        }
                    }
                    
                    // Bottom Playback Controls
                    int pbY = screenH - 60;
                    int pbCX = screenW / 2;
                    if (mPos.x > pbCX - 150 && mPos.x < pbCX - 70 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!bitmask_steps.empty() && currentStep > 0) { currentStep--; playing = false; }
                    }
                    if (mPos.x > pbCX - 60 && mPos.x < pbCX + 60 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!bitmask_steps.empty()) {
                            if (currentStep >= totalSteps - 1) { currentStep = 0; playing = true; } else { playing = !playing; }
                        }
                    }
                    if (mPos.x > pbCX + 70 && mPos.x < pbCX + 150 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!bitmask_steps.empty() && currentStep < totalSteps - 1) { currentStep++; playing = false; }
                    }
                    
                    if (mPos.x > 10 && mPos.x < 110 && mPos.y > 10 && mPos.y < 50) {
                        screen = 0; playing = false;
                    }
                }
            } else if (screen == 5) {
                int leftPanelW = 260;
                int rightPanelW = 260;
                int rightPanelX = screenW - rightPanelW;
                int centerW = rightPanelX - leftPanelW;
                
                int maxGridW = centerW - 40;
                int maxGridH = screenH - 120;
                
                int cellSizeW = maxGridW / grid_cols;
                int cellSizeH = maxGridH / grid_rows;
                int cellSize = (cellSizeW < cellSizeH) ? cellSizeW : cellSizeH;
                
                int gridW = grid_cols * cellSize;
                int gridH = grid_rows * cellSize;
                
                int gridX = leftPanelW + (centerW - gridW) / 2;
                int gridY = 40 + (maxGridH - gridH) / 2;
                
                if (!playing) {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        if (mPos.x > gridX && mPos.x < gridX + gridW && mPos.y > gridY && mPos.y < gridY + gridH) {
                            int c = (int)((mPos.x - gridX) / cellSize);
                            int r = (int)((mPos.y - gridY) / cellSize);
                            
                            if (r >= 0 && r < grid_rows && c >= 0 && c < grid_cols) {
                                if (make_pair(r, c) != start_node && make_pair(r, c) != end_node) {
                                    walls[r][c] = true;
                                }
                            }
                        }
                    }
                }
                
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Left Panel Buttons
                    // Clear Walls
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 150 && mPos.y < 190) {
                        for (int r=0; r<grid_rows; r++) for (int c=0; c<grid_cols; c++) walls[r][c] = false;
                        path_steps.clear(); currentStep = 0; playing = false;
                    }
                    
                    // START SEARCH
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 210 && mPos.y < 270) {
                        path_steps = runBFS(grid_rows, grid_cols, start_node, end_node, walls);
                        currentStep = 0; playing = true;
                    }
                    
                    // Bottom Playback Controls
                    int pbY = screenH - 60;
                    int pbCX = screenW / 2;
                    if (mPos.x > pbCX - 150 && mPos.x < pbCX - 70 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!path_steps.empty() && currentStep > 0) { currentStep--; playing = false; }
                    }
                    if (mPos.x > pbCX - 60 && mPos.x < pbCX + 60 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!path_steps.empty()) {
                            if (currentStep >= totalSteps - 1) { currentStep = 0; playing = true; } else { playing = !playing; }
                        }
                    }
                    if (mPos.x > pbCX + 70 && mPos.x < pbCX + 150 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!path_steps.empty() && currentStep < totalSteps - 1) { currentStep++; playing = false; }
                    }
                    
                    if (mPos.x > 10 && mPos.x < 110 && mPos.y > 10 && mPos.y < 50) {
                        screen = 0; playing = false;
                    }
                }
            } else if (screen == 6) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Left Panel Buttons
                    // Capacity Input
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 150 && mPos.y < 190) {
                        active_input_field = 5; // DP capacity
                    } else {
                        if (active_input_field == 5) active_input_field = 0;
                    }
                    
                    // Load Random Items
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 210 && mPos.y < 250) {
                        dp_items.clear();
                        int n = 4 + (rand() % 3); // 4 to 6 items
                        for (int i=0; i<n; i++) {
                            dp_items.push_back({1 + (rand() % 5), 1 + (rand() % 10)});
                        }
                        dp_steps.clear(); currentStep = 0; playing = false;
                    }
                    
                    // RUN TABULATION
                    if (mPos.x > 30 && mPos.x < 230 && mPos.y > 270 && mPos.y < 330) {
                        if (!dp_capacity_buffer.empty()) {
                            dp_capacity = stoi(dp_capacity_buffer);
                        }
                        dp_steps = runKnapsack(dp_capacity, dp_items);
                        currentStep = 0; playing = true;
                    }
                    
                    // Bottom Playback Controls
                    int pbY = screenH - 60;
                    int pbCX = screenW / 2;
                    if (mPos.x > pbCX - 150 && mPos.x < pbCX - 70 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!dp_steps.empty() && currentStep > 0) { currentStep--; playing = false; }
                    }
                    if (mPos.x > pbCX - 60 && mPos.x < pbCX + 60 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!dp_steps.empty()) {
                            if (currentStep >= totalSteps - 1) { currentStep = 0; playing = true; } else { playing = !playing; }
                        }
                    }
                    if (mPos.x > pbCX + 70 && mPos.x < pbCX + 150 && mPos.y > pbY && mPos.y < pbY + 40) {
                        if (!dp_steps.empty() && currentStep < totalSteps - 1) { currentStep++; playing = false; }
                    }
                    
                    if (mPos.x > 10 && mPos.x < 110 && mPos.y > 10 && mPos.y < 50) {
                        screen = 0; playing = false;
                    }
                }
            } else if (screen == 7) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // N = 4 button
                    if (mPos.x > 30 && mPos.x < 120 && mPos.y > 150 && mPos.y < 190) {
                        n_queens_size = 4;
                        queen_steps.clear(); currentStep = 0; playing = false;
                    }
                    
                    // N = 8 button
                    if (mPos.x > 140 && mPos.x < 230 && mPos.y > 150 && mPos.y < 190) {
                        n_queens_size = 8;
                        queen_steps.clear(); currentStep = 0; playing = false;
                    }
                    
                    // START SEARCH
                    Rectangle startBtn = {30, 210, 200, 60};
                    if (CheckCollisionPointRec(mPos, startBtn)) {
                        queen_steps = solveNQueens(n_queens_size);
                        currentStep = 0; playing = true;
                    }
                    
                    // Bottom Playback Controls
                    int pbY = screenH - 60;
                    int pbCX = screenW / 2;
                    Rectangle prevBtn = {(float)(pbCX - 150), (float)pbY, 80, 40};
                    Rectangle playBtn = {(float)(pbCX - 60), (float)pbY, 120, 40};
                    Rectangle nextBtn = {(float)(pbCX + 70), (float)pbY, 80, 40};
                    
                    if (CheckCollisionPointRec(mPos, prevBtn)) {
                        playing = false;
                        if (currentStep > 0) currentStep--;
                    }
                    if (CheckCollisionPointRec(mPos, playBtn)) {
                        if (!queen_steps.empty()) {
                            if (currentStep >= totalSteps - 1) { currentStep = 0; playing = true; } else { playing = !playing; }
                        }
                    }
                    if (CheckCollisionPointRec(mPos, nextBtn)) {
                        playing = false;
                        if (!queen_steps.empty() && currentStep < totalSteps - 1) currentStep++;
                    }
                    
                    // Menu button
                    Rectangle menuBtn = {10, 10, 100, 40};
                    if (CheckCollisionPointRec(mPos, menuBtn)) {
                        screen = 0; playing = false;
                    }
                }
                
                // Speed Slider Dragging
                Rectangle sliderHitbox = {30, 290, 200, 20};
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mPos, sliderHitbox)) {
                    is_dragging_nqueens_speed = true;
                }
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    is_dragging_nqueens_speed = false;
                }
                if (is_dragging_nqueens_speed) {
                    float pct = (mPos.x - 30) / 200.0f;
                    if (pct < 0.0f) pct = 0.0f;
                    if (pct > 1.0f) pct = 1.0f;
                    nqueens_anim_speed = 0.05f + pct * 1.95f;
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
                            } else if (active_input_field == 4 && bit_input_buffer.length() < 9 && key >= '0' && key <= '9') {
                                bit_input_buffer += (char)key;
                            } else if (active_input_field == 5 && dp_capacity_buffer.length() < 3 && key >= '0' && key <= '9') {
                                dp_capacity_buffer += (char)key;
                            }
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        if (active_input_field == 1 && !num_elements_buffer.empty()) num_elements_buffer.pop_back();
                        if (active_input_field == 2 && !manual_array_buffer.empty()) manual_array_buffer.pop_back();
                        if (active_input_field == 3 && !target_buffer.empty()) target_buffer.pop_back();
                        if (active_input_field == 4 && !bit_input_buffer.empty()) bit_input_buffer.pop_back();
                        if (active_input_field == 5 && !dp_capacity_buffer.empty()) dp_capacity_buffer.pop_back();
                    }
                }
                
            // Auto-play
            if (screen == 7) {
                if (playing && !queen_steps.empty() && currentStep < (int)queen_steps.size() - 1) {
                    nqueens_timer += dt;
                    if (nqueens_timer >= nqueens_anim_speed) {
                        currentStep++;
                        nqueens_timer = 0.0f;
                    }
                } else if (playing && currentStep >= (int)queen_steps.size() - 1) {
                    playing = false; // Stop at the end
                }
            } else {
                if (playing && currentStep < totalSteps - 1) {
                    timer += (int)(dt * 1000);
                    if (timer >= speed) {
                        currentStep++; 
                        timer = 0; 
                    }
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

    void draw(int screenW, int screenH) {
        ClearBackground(BG_COLOR);
        int cx = screenW / 2;

        if (screen == 0) {
            Vector2 mPos = GetMousePosition();
            
            // Stylized Header
            DrawTextSmoothCentered("ALGORITHM VISUALIZER", 0, 40, screenW, 56, TEXT_COLOR);
            
            // State A: Category Selection
            int catW = 220, catH = 220, catGap = 30;
            string names[] = {"SORTING", "SEARCHING", "GRAPHS", "BITMASK", "PATHFINDING", "DYNAMIC PROG", "BACKTRACKING"};
            Color catColors[] = {COLOR_SORTING, COLOR_SEARCHING, COLOR_GRAPHS, COLOR_BITMASK, COLOR_PATHFINDING, COLOR_DP, COLOR_BACKTRACK};
            
            for (int i=0; i<7; i++) {
                int itemsInRow = 4;
                int row = (i < 4) ? 0 : 1;
                int col = (i < 4) ? i : (i - 4);
                int rowW = itemsInRow * catW + (itemsInRow - 1) * catGap;
                int startX = cx - rowW / 2;
                int cx_cat = startX + col * (catW + catGap);
                int cy_cat = 160 + row * (catH + catGap);
                
                bool hover = (mPos.x > cx_cat && mPos.x < cx_cat + catW && mPos.y > cy_cat && mPos.y < cy_cat + catH);
                Color baseColor = catColors[i];
                Color bgColor = hover ? HOVER_COLOR : PANEL_COLOR;
                
                if (hover) {
                    DrawGlowRect(cx_cat, cy_cat, catW, catH, 0.1f, baseColor, 15);
                }
                
                DrawRoundedRect(cx_cat, cy_cat, catW, catH, 0.1f, bgColor);
                DrawRoundedRectLines(cx_cat, cy_cat, catW, catH, 0.1f, 3.0f, hover ? baseColor : BORDER_COLOR);
                
                // Simple Abstract Visual for Cards
                int iconY = cy_cat + 50;
                if (i == 0) { // Sorting icon
                    DrawRoundedRect(cx_cat + 50, iconY + 60, 30, 40, 0.0f, baseColor);
                    DrawRoundedRect(cx_cat + 90, iconY + 20, 30, 80, 0.0f, baseColor);
                    DrawRoundedRect(cx_cat + 130, iconY + 40, 30, 60, 0.0f, baseColor);
                    DrawRoundedRect(cx_cat + 170, iconY, 30, 100, 0.0f, baseColor);
                } else if (i == 1) { // Searching icon
                    DrawCircle(cx_cat + 130, iconY + 50, 40, baseColor);
                    DrawCircle(cx_cat + 130, iconY + 50, 30, bgColor); // inner hollow
                    DrawLineEx({(float)cx_cat + 155, (float)iconY + 75}, {(float)cx_cat + 185, (float)iconY + 105}, 10.0f, baseColor);
                } else if (i == 2) { // Graph icon
                    DrawCircle(cx_cat + 130, iconY + 20, 15, baseColor);
                    DrawCircle(cx_cat + 80, iconY + 80, 15, baseColor);
                    DrawCircle(cx_cat + 180, iconY + 80, 15, baseColor);
                    DrawLineEx({(float)cx_cat + 130, (float)iconY + 20}, {(float)cx_cat + 80, (float)iconY + 80}, 5.0f, baseColor);
                    DrawLineEx({(float)cx_cat + 130, (float)iconY + 20}, {(float)cx_cat + 180, (float)iconY + 80}, 5.0f, baseColor);
                    DrawLineEx({(float)cx_cat + 80, (float)iconY + 80}, {(float)cx_cat + 180, (float)iconY + 80}, 5.0f, baseColor);
                } else if (i == 3) { // Bitmask icon
                    DrawRoundedRect(cx_cat + 60, iconY + 40, 20, 20, 0.1f, baseColor);
                    DrawRoundedRect(cx_cat + 90, iconY + 40, 20, 20, 0.1f, bgColor);
                    DrawRoundedRectLines(cx_cat + 90, iconY + 40, 20, 20, 0.1f, 3.0f, baseColor);
                    DrawRoundedRect(cx_cat + 120, iconY + 40, 20, 20, 0.1f, baseColor);
                    DrawRoundedRect(cx_cat + 150, iconY + 40, 20, 20, 0.1f, bgColor);
                    DrawRoundedRectLines(cx_cat + 150, iconY + 40, 20, 20, 0.1f, 3.0f, baseColor);
                } else if (i == 4) { // Pathfinding icon
                    DrawRoundedRect(cx_cat + 60, iconY + 20, 30, 30, 0.0f, baseColor);
                    DrawRoundedRect(cx_cat + 95, iconY + 20, 30, 30, 0.0f, bgColor);
                    DrawRoundedRectLines(cx_cat + 95, iconY + 20, 30, 30, 0.0f, 2.0f, baseColor);
                    DrawRoundedRect(cx_cat + 130, iconY + 55, 30, 30, 0.0f, baseColor);
                    DrawRoundedRect(cx_cat + 95, iconY + 55, 30, 30, 0.0f, baseColor);
                } else if (i == 5) { // DP icon
                    DrawRoundedRect(cx_cat + 60, iconY + 20, 30, 30, 0.1f, baseColor);
                    DrawRoundedRect(cx_cat + 95, iconY + 20, 30, 30, 0.1f, bgColor);
                    DrawRoundedRectLines(cx_cat + 95, iconY + 20, 30, 30, 0.1f, 2.0f, baseColor);
                    DrawRoundedRect(cx_cat + 130, iconY + 20, 30, 30, 0.1f, baseColor);
                    
                    DrawRoundedRect(cx_cat + 60, iconY + 55, 30, 30, 0.1f, bgColor);
                    DrawRoundedRectLines(cx_cat + 60, iconY + 55, 30, 30, 0.1f, 2.0f, baseColor);
                    DrawRoundedRect(cx_cat + 95, iconY + 55, 30, 30, 0.1f, baseColor);
                    DrawRoundedRect(cx_cat + 130, iconY + 55, 30, 30, 0.1f, bgColor);
                    DrawRoundedRectLines(cx_cat + 130, iconY + 55, 30, 30, 0.1f, 2.0f, baseColor);
                } else if (i == 6) { // Backtracking icon
                    int bY = iconY + 20;
                    DrawRectangle(cx_cat + 60, bY, 100, 100, bgColor);
                    for(int r=0; r<4; r++) {
                        for(int c=0; c<4; c++) {
                            if((r+c)%2 != 0) DrawRectangle(cx_cat + 60 + c*25, bY + r*25, 25, 25, baseColor);
                        }
                    }
                    DrawCircle(cx_cat + 60 + 1*25 + 12, bY + 1*25 + 12, 8, DASH_PANEL); // A "Queen"
                }
                
                DrawTextSmoothCentered(names[i].c_str(), cx_cat, cy_cat + catH - 50, catW, 26, CARD_TEXT);
            }
        } else if (screen == 1 || screen == 2 || screen == 4 || screen == 5 || screen == 6 || screen == 7) {
            ClearBackground(DASH_BG);
            Vector2 mPos = GetMousePosition();
            
            // Draw Left Panel (Full Height)
            int leftPanelW = 260;
            DrawRectangle(0, 0, leftPanelW, screenH, DASH_PANEL);
            DrawLine(leftPanelW, 0, leftPanelW, screenH, DASH_BORDER);
            
            // Top Left Menu Button
            bool menuHover = (mPos.x > 10 && mPos.x < 110 && mPos.y > 10 && mPos.y < 50);
            DrawRoundedRectLines(10, 10, 100, 40, 0.2f, 1.0f, menuHover ? DASH_ACCENT : DASH_BORDER);
            DrawTextSmoothCentered("< MENU", 10, 20, 100, 20, menuHover ? DASH_ACCENT : DASH_TEXT);
            
            if (screen == 1 || screen == 2) {
                // "CREATE" Section
                DrawTextSmooth("CREATE", 30, 80, 24, DASH_TEXT);
            
            DrawTextSmooth("Size (N)", 30, 115, 16, DASH_DIM);
            DrawRoundedRect(30, 135, 200, 30, 0.2f, active_input_field == 1 ? DASH_BG : DASH_PANEL);
            DrawRoundedRectLines(30, 135, 200, 30, 0.2f, 1.0f, active_input_field == 1 ? DASH_ACCENT : DASH_BORDER);
            DrawTextSmooth(num_elements_buffer.c_str(), 40, 142, 16, DASH_TEXT);
            
            DrawTextSmooth("Distribution", 30, 175, 16, DASH_DIM);
            string distNames[] = {"Random", "Sorted (Asc)", "Sorted (Desc)", "Duplicates"};
            DrawRoundedRect(30, 195, 200, 30, 0.2f, DASH_PANEL);
            DrawRoundedRectLines(30, 195, 200, 30, 0.2f, 1.0f, DASH_BORDER);
            DrawTextSmoothCentered(distNames[current_distribution].c_str(), 30, 202, 200, 16, DASH_TEXT);
            DrawTriangle({215, 205}, {225, 205}, {220, 215}, DASH_DIM); 
            
            DrawTextSmooth("Manual Array", 30, 235, 16, DASH_DIM);
            DrawRoundedRect(30, 255, 200, 30, 0.2f, active_input_field == 2 ? DASH_BG : DASH_PANEL);
            DrawRoundedRectLines(30, 255, 200, 30, 0.2f, 1.0f, active_input_field == 2 ? DASH_ACCENT : DASH_BORDER);
            DrawTextSmooth(manual_array_buffer.c_str(), 40, 262, 16, DASH_TEXT);
            
            bool createHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 300 && mPos.y < 340);
            DrawRoundedRect(30, 300, 200, 40, 0.2f, createHover ? DASH_ACCENT : DASH_PANEL);
            DrawRoundedRectLines(30, 300, 200, 40, 0.2f, 1.0f, DASH_BORDER);
            DrawTextSmoothCentered("CREATE", 30, 312, 200, 18, createHover ? DASH_PANEL : DASH_ACCENT);
            
            // Actions Section
            DrawLine(20, 350, leftPanelW - 20, 350, DASH_BORDER);
            
            if (screen == 1) {
                // "SORT" Section
                DrawTextSmooth("Algorithm", 30, 360, 16, DASH_DIM);
                string algNames[] = {"Bubble Sort", "Insertion Sort", "Selection Sort"};
                DrawRoundedRect(30, 380, 200, 30, 0.2f, DASH_PANEL);
                DrawRoundedRectLines(30, 380, 200, 30, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered(algNames[algo].c_str(), 30, 386, 200, 16, DASH_TEXT);
                DrawTriangle({215, 390}, {225, 390}, {220, 400}, DASH_DIM); 
                
                DrawTextSmooth("Order", 30, 400, 16, DASH_DIM);
                DrawRoundedRect(30, 420, 200, 30, 0.2f, DASH_PANEL);
                DrawRoundedRectLines(30, 420, 200, 30, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered(sort_ascending ? "Ascending" : "Descending", 30, 426, 200, 16, DASH_TEXT);
                DrawTriangle({215, 430}, {225, 430}, {220, 440}, DASH_DIM); 
                
                bool sortHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 460 && mPos.y < 520);
                DrawRoundedRect(30, 460, 200, 60, 0.2f, sortHover ? BAR_NORMAL : DASH_PANEL);
                DrawRoundedRectLines(30, 460, 200, 60, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("SORT", 30, 480, 200, 24, sortHover ? DASH_PANEL : BAR_NORMAL);
            } else if (screen == 2) {
                // "SEARCH" Section
                DrawTextSmooth("Target Value", 30, 380, 16, DASH_DIM);
                DrawRoundedRect(30, 400, 200, 30, 0.2f, active_input_field == 3 ? DASH_BG : DASH_PANEL);
                DrawRoundedRectLines(30, 400, 200, 30, 0.2f, 1.0f, active_input_field == 3 ? DASH_ACCENT : DASH_BORDER);
                DrawTextSmooth(target_buffer.c_str(), 40, 407, 16, DASH_TEXT);
                
                bool searchHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 460 && mPos.y < 520);
                DrawRoundedRect(30, 460, 200, 60, 0.2f, searchHover ? BAR_NORMAL : DASH_PANEL);
                DrawRoundedRectLines(30, 460, 200, 60, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("SEARCH", 30, 480, 200, 24, searchHover ? DASH_PANEL : BAR_NORMAL);
            }
            
            // Speed Slider
            DrawLine(20, 540, leftPanelW - 20, 540, DASH_BORDER);
            DrawTextSmooth("Animation Speed", 30, 550, 16, DASH_DIM);
            float speedPct = (1000.0f - speed) / 980.0f; 
            if (speedPct < 0.0f) speedPct = 0.0f; if (speedPct > 1.0f) speedPct = 1.0f;
            DrawRoundedRect(30, 580, 200, 8, 0.5f, DASH_PANEL);
            DrawRoundedRect(30, 580, (int)(200 * speedPct), 8, 0.5f, DASH_ACCENT);
            DrawCircle(30 + (int)(200 * speedPct), 584, 8, DASH_TEXT);
            } else if (screen == 4) {
                DrawTextSmooth("OPERATIONS", 30, 80, 20, DASH_TEXT);
                DrawLine(20, 110, leftPanelW - 20, 110, DASH_BORDER);
                
                string ops[] = {"Increment", "Set S", "Set j-th Bit", "Check j-th Bit", "Clear j-th Bit", "Toggle j-th Bit", "Least Sig. Bit"};
                
                for (int i=0; i<7; i++) {
                    int by = 135 + i * 50;
                    bool needsInput = (i == 1 || (i >= 2 && i <= 5));
                    bool isActive = (active_bitmask_op == i);
                    
                    int btnW = (isActive && needsInput) ? 120 : 200;
                    
                    bool hover = (mPos.x > 30 && mPos.x < 30 + btnW && mPos.y > by && mPos.y < by + 40);
                    DrawRoundedRect(30, by, btnW, 40, 0.2f, isActive ? BAR_COMPARING : (hover ? DASH_BG : DASH_PANEL));
                    DrawRoundedRectLines(30, by, btnW, 40, 0.2f, 1.0f, DASH_BORDER);
                    DrawTextSmooth(ops[i].c_str(), 45, by + 12, 16, isActive ? DASH_PANEL : DASH_TEXT);
                    
                    if (isActive && needsInput) {
                        string lbl = (i == 1) ? "S:" : "j:";
                        DrawTextSmooth(lbl.c_str(), 155, by + 12, 16, DASH_DIM);
                        
                        // Input box
                        bool txtHover = (mPos.x > 180 && mPos.x < 215 && mPos.y > by && mPos.y < by + 40);
                        DrawRoundedRect(180, by, 35, 40, 0.2f, active_input_field == 4 ? DASH_BG : DASH_PANEL);
                        DrawRoundedRectLines(180, by, 35, 40, 0.2f, 1.0f, active_input_field == 4 ? DASH_ACCENT : DASH_BORDER);
                        DrawTextSmoothCentered(bit_input_buffer.c_str(), 180, by + 12, 35, 16, DASH_TEXT);
                        
                        // Go Button
                        bool goHover = (mPos.x > 220 && mPos.x < 250 && mPos.y > by && mPos.y < by + 40);
                        DrawRoundedRect(220, by, 30, 40, 0.2f, goHover ? BAR_NORMAL : DASH_PANEL);
                        DrawRoundedRectLines(220, by, 30, 40, 0.2f, 1.0f, DASH_BORDER);
                        DrawTextSmoothCentered("Go", 220, by + 12, 30, 16, goHover ? DASH_PANEL : DASH_TEXT);
                    }
                }
            } else if (screen == 5) {
                DrawTextSmooth("CONTROLS", 30, 80, 20, DASH_TEXT);
                DrawLine(20, 110, leftPanelW - 20, 110, DASH_BORDER);
                
                // Clear Walls
                bool clearHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 150 && mPos.y < 190);
                DrawRoundedRect(30, 150, 200, 40, 0.2f, clearHover ? DASH_BG : DASH_PANEL);
                DrawRoundedRectLines(30, 150, 200, 40, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("Clear Walls", 30, 162, 200, 16, DASH_TEXT);
                
                // START SEARCH
                bool searchHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 210 && mPos.y < 270);
                DrawRoundedRect(30, 210, 200, 60, 0.2f, searchHover ? BAR_NORMAL : DASH_PANEL);
                DrawRoundedRectLines(30, 210, 200, 60, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("START SEARCH", 30, 230, 200, 20, searchHover ? DASH_PANEL : BAR_NORMAL);
            } else if (screen == 6) {
                DrawTextSmooth("CONTROLS", 30, 80, 20, DASH_TEXT);
                DrawLine(20, 110, leftPanelW - 20, 110, DASH_BORDER);
                
                // Capacity Input
                DrawTextSmooth("Knapsack Capacity (W)", 30, 130, 16, DASH_DIM);
                DrawRoundedRect(30, 150, 200, 40, 0.2f, active_input_field == 5 ? DASH_BG : DASH_PANEL);
                DrawRoundedRectLines(30, 150, 200, 40, 0.2f, 1.0f, active_input_field == 5 ? DASH_ACCENT : DASH_BORDER);
                DrawTextSmooth(dp_capacity_buffer.c_str(), 40, 162, 16, DASH_TEXT);
                
                // Load Random Items
                bool loadHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 210 && mPos.y < 250);
                DrawRoundedRect(30, 210, 200, 40, 0.2f, loadHover ? DASH_BG : DASH_PANEL);
                DrawRoundedRectLines(30, 210, 200, 40, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("Load Random Items", 30, 222, 200, 16, DASH_TEXT);
                
                // START SEARCH
                bool searchHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 270 && mPos.y < 330);
                DrawRoundedRect(30, 270, 200, 60, 0.2f, searchHover ? BAR_NORMAL : DASH_PANEL);
                DrawRoundedRectLines(30, 270, 200, 60, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("RUN TABULATION", 30, 290, 200, 20, searchHover ? DASH_PANEL : BAR_NORMAL);
            } else if (screen == 7) {
                DrawTextSmooth("CONTROLS", 30, 80, 20, DASH_TEXT);
                DrawLine(20, 110, leftPanelW - 20, 110, DASH_BORDER);
                
                // N = 4 Button
                bool n4Hover = (mPos.x > 30 && mPos.x < 120 && mPos.y > 150 && mPos.y < 190);
                DrawRoundedRect(30, 150, 90, 40, 0.2f, n4Hover ? DASH_BG : (n_queens_size == 4 ? DASH_ACCENT : DASH_PANEL));
                DrawRoundedRectLines(30, 150, 90, 40, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("N = 4", 30, 162, 90, 16, DASH_TEXT);
                
                // N = 8 Button
                bool n8Hover = (mPos.x > 140 && mPos.x < 230 && mPos.y > 150 && mPos.y < 190);
                DrawRoundedRect(140, 150, 90, 40, 0.2f, n8Hover ? DASH_BG : (n_queens_size == 8 ? DASH_ACCENT : DASH_PANEL));
                DrawRoundedRectLines(140, 150, 90, 40, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("N = 8", 140, 162, 90, 16, DASH_TEXT);
                
                // START SEARCH
                bool searchHover = (mPos.x > 30 && mPos.x < 230 && mPos.y > 210 && mPos.y < 270);
                DrawRoundedRect(30, 210, 200, 60, 0.2f, searchHover ? BAR_NORMAL : DASH_PANEL);
                DrawRoundedRectLines(30, 210, 200, 60, 0.2f, 1.0f, DASH_BORDER);
                DrawTextSmoothCentered("START SEARCH", 30, 230, 200, 20, searchHover ? DASH_PANEL : BAR_NORMAL);
                
                // Animation Speed Slider
                DrawTextSmooth("Animation Speed", 30, 280, 14, DASH_DIM);
                DrawRoundedRect(30, 300, 200, 4, 1.0f, DASH_BORDER);
                float speed_pct = (nqueens_anim_speed - 0.05f) / 1.95f;
                if (speed_pct < 0) speed_pct = 0;
                if (speed_pct > 1) speed_pct = 1;
                DrawRoundedRect(30, 300, (int)(speed_pct * 200), 4, 1.0f, DASH_ACCENT);
                DrawCircle(30 + (int)(speed_pct * 200), 302, 8, is_dragging_nqueens_speed ? DASH_TEXT : DASH_ACCENT);
            }
            
            // Right Panel (Metrics) - Full Height
            int rightPanelW = 200;
            int rightPanelX = screenW - rightPanelW;
            DrawRectangle(rightPanelX, 0, rightPanelW, screenH, DASH_PANEL);
            DrawLine(rightPanelX, 0, rightPanelX, screenH, DASH_BORDER);
            
            int cCount = 0;
            int sCount = 0;
            if (screen == 1) {
                cCount = steps.empty() ? 0 : countComparisons(steps, currentStep);
                sCount = steps.empty() ? 0 : countSwaps(steps, currentStep);
            } else if (screen == 2) {
                cCount = searchSteps.empty() ? 0 : currentStep; // 1 comparison per step in binary search
            }
            
            if (screen == 1 || screen == 2) {
                // Comparisons Box
                DrawRoundedRect(rightPanelX + 20, 100, 160, 100, 0.1f, DASH_PANEL);
                DrawRoundedRectLines(rightPanelX + 20, 100, 160, 100, 0.1f, 3.0f, BAR_COMPARING);
                DrawTextSmoothCentered("Comparisons", rightPanelX + 20, 120, 160, 18, DASH_DIM);
                DrawTextSmoothCentered(to_string(cCount).c_str(), rightPanelX + 20, 150, 160, 32, BAR_COMPARING);
            }
            
            if (screen == 1) {
                // Swaps Box
                DrawRoundedRect(rightPanelX + 20, 220, 160, 100, 0.1f, DASH_PANEL);
                DrawRoundedRectLines(rightPanelX + 20, 220, 160, 100, 0.1f, 3.0f, BAR_SWAPPING);
                DrawTextSmoothCentered("Swaps", rightPanelX + 20, 240, 160, 18, DASH_DIM);
                DrawTextSmoothCentered(to_string(sCount).c_str(), rightPanelX + 20, 270, 160, 32, BAR_SWAPPING);
                
                // Complexity Text
                if (!theoretical_complexity_str.empty()) {
                    DrawTextSmoothCentered("Complexity", rightPanelX + 20, 350, 160, 18, DASH_DIM);
                    DrawTextSmoothCentered(theoretical_complexity_str.c_str(), rightPanelX + 20, 380, 160, 24, DASH_TEXT);
                }
            } else if (screen == 2) {
                // Complexity Text for Search
                DrawTextSmoothCentered("Time Complexity", rightPanelX + 20, 220, 160, 18, DASH_DIM);
                DrawTextSmoothCentered("O(log N)", rightPanelX + 20, 250, 160, 24, DASH_TEXT);
            } else if (screen == 4) {
                DrawTextSmooth("PSEUDOCODE", rightPanelX + 20, 80, 20, DASH_TEXT);
                DrawLine(rightPanelX + 20, 110, screenW - 20, 110, DASH_BORDER);
                
                vector<string> lines;
                if (active_bitmask_op == 0) {
                    lines = {"result = S + 1;", "// Implemented via", "// Half Adders"};
                } else if (active_bitmask_op == 1) {
                    lines = {"// Set S value"};
                } else if (active_bitmask_op == 2) {
                    lines = {"mask = 1 << j;", "result = S | mask;"};
                } else if (active_bitmask_op == 3) {
                    lines = {"mask = 1 << j;", "result = S & mask;", "// if result > 0,", "// bit is 1"};
                } else if (active_bitmask_op == 4) {
                    lines = {"mask = ~(1 << j);", "result = S & mask;"};
                } else if (active_bitmask_op == 5) {
                    lines = {"mask = 1 << j;", "result = S ^ mask;"};
                } else if (active_bitmask_op == 6) {
                    lines = {"mask = -S;", "result = S & mask;"};
                }

                int highlightIdx = 0;
                if (!bitmask_steps.empty() && currentStep < bitmask_steps.size()) {
                    highlightIdx = bitmask_steps[currentStep].highlight_line_idx;
                }
                
                int y = 140;
                for (int i=0; i<lines.size(); i++) {
                    if (i == highlightIdx) {
                        DrawRoundedRect(rightPanelX + 10, y - 5, rightPanelW - 20, 26, 0.2f, BAR_COMPARING);
                    }
                    Color textC = (i == highlightIdx) ? DASH_TEXT : DASH_DIM;
                    DrawTextSmooth(lines[i].c_str(), rightPanelX + 20, y, 14, textC);
                    y += 30;
                }
                
                if (!bitmask_steps.empty() && currentStep < bitmask_steps.size()) {
                    DrawLine(rightPanelX + 20, y + 20, screenW - 20, y + 20, DASH_BORDER);
                    DrawTextSmooth("Log:", rightPanelX + 20, y + 40, 14, DASH_DIM);
                    string msg = bitmask_steps[currentStep].pseudocode_highlight;
                    
                    int msgY = y + 70;
                    string word;
                    string lineStr = "";
                    stringstream ss(msg);
                    while (ss >> word) {
                        if (MeasureTextSmooth((lineStr + word).c_str(), 14) > rightPanelW - 40) {
                            DrawTextSmooth(lineStr.c_str(), rightPanelX + 20, msgY, 14, DASH_TEXT);
                            msgY += 20;
                            lineStr = word + " ";
                        } else {
                            lineStr += word + " ";
                        }
                    }
                    if (!lineStr.empty()) {
                        DrawTextSmooth(lineStr.c_str(), rightPanelX + 20, msgY, 14, DASH_TEXT);
                    }
                }
            } else if (screen == 5) {
                DrawTextSmooth("LEGEND", rightPanelX + 20, 80, 20, DASH_TEXT);
                DrawLine(rightPanelX + 20, 110, screenW - 20, 110, DASH_BORDER);
                
                int ly = 130;
                DrawRectangle(rightPanelX + 20, ly, 20, 20, GRID_START);
                DrawTextSmooth("Start Node", rightPanelX + 50, ly + 2, 16, DASH_TEXT);
                ly += 30;
                DrawRectangle(rightPanelX + 20, ly, 20, 20, GRID_END);
                DrawTextSmooth("End Node", rightPanelX + 50, ly + 2, 16, DASH_TEXT);
                ly += 30;
                DrawRectangle(rightPanelX + 20, ly, 20, 20, GRID_WALL);
                DrawTextSmooth("Wall", rightPanelX + 50, ly + 2, 16, DASH_TEXT);
                ly += 30;
                DrawRectangle(rightPanelX + 20, ly, 20, 20, GRID_VISITED);
                DrawTextSmooth("Visited", rightPanelX + 50, ly + 2, 16, DASH_TEXT);
                ly += 30;
                DrawRectangle(rightPanelX + 20, ly, 20, 20, GRID_FRONTIER);
                DrawTextSmooth("Frontier", rightPanelX + 50, ly + 2, 16, DASH_TEXT);
                ly += 30;
                DrawRectangle(rightPanelX + 20, ly, 20, 20, GRID_PATH);
                DrawTextSmooth("Path", rightPanelX + 50, ly + 2, 16, DASH_TEXT);
                
                DrawLine(rightPanelX + 20, ly + 40, screenW - 20, ly + 40, DASH_BORDER);
                DrawTextSmooth("METRICS", rightPanelX + 20, ly + 60, 20, DASH_TEXT);
                
                int nodesExplored = 0;
                if (!path_steps.empty() && currentStep < (int)path_steps.size()) {
                    for (int r=0; r<grid_rows; r++) {
                        for (int c=0; c<grid_cols; c++) {
                            CellState st = path_steps[currentStep].grid[r][c];
                            if (st == VISITED || st == FRONTIER || st == PATH) nodesExplored++;
                        }
                    }
                }
                DrawTextSmooth("Nodes Explored:", rightPanelX + 20, ly + 90, 16, DASH_DIM);
                DrawTextSmooth(to_string(nodesExplored).c_str(), rightPanelX + 20, ly + 115, 24, DASH_ACCENT);
            } else if (screen == 6) {
                DrawTextSmooth("BELLMAN EQUATION", rightPanelX + 20, 80, 18, DASH_TEXT);
                DrawLine(rightPanelX + 20, 110, screenW - 20, 110, DASH_BORDER);
                
                DrawTextSmooth("DP[i][w] = max(", rightPanelX + 20, 130, 16, DASH_TEXT);
                DrawTextSmooth("    DP[i-1][w],", rightPanelX + 20, 160, 16, DASH_TEXT);
                DrawTextSmooth("    DP[i-1][w-wt[i-1]] + val[i-1]", rightPanelX + 20, 190, 16, DASH_TEXT);
                DrawTextSmooth(")", rightPanelX + 20, 220, 16, DASH_TEXT);
                
                if (!dp_steps.empty() && currentStep < (int)dp_steps.size()) {
                    DPStep& cur = dp_steps[currentStep];
                    if (cur.current_i > 0) {
                        DrawLine(rightPanelX + 20, 260, screenW - 20, 260, DASH_BORDER);
                        DrawTextSmooth("CURRENT STEP:", rightPanelX + 20, 280, 16, DASH_DIM);
                        
                        string eq1 = "DP[" + to_string(cur.current_i) + "][" + to_string(cur.current_w) + "] = max(";
                        DrawTextSmooth(eq1.c_str(), rightPanelX + 20, 310, 14, DASH_TEXT);
                        
                        string eq2 = "    DP[" + to_string(cur.check_row_1) + "][" + to_string(cur.check_col_1) + "],";
                        DrawTextSmooth(eq2.c_str(), rightPanelX + 20, 340, 14, DP_DEPENDENCY);
                        
                        string eq3 = "    -1";
                        if (cur.check_row_2 != -1) {
                            eq3 = "    DP[" + to_string(cur.check_row_2) + "][" + to_string(cur.check_col_2) + "] + " + to_string(dp_items[cur.current_i - 1].value);
                        }
                        DrawTextSmooth(eq3.c_str(), rightPanelX + 20, 370, 14, DP_DEPENDENCY);
                        
                        string eq4 = ") = " + to_string(cur.chosen_value);
                        DrawTextSmooth(eq4.c_str(), rightPanelX + 20, 400, 14, DP_ACTIVE_CELL);
                    }
                }
            } else if (screen == 7) {
                DrawTextSmooth("ALGORITHM", rightPanelX + 20, 80, 18, DASH_TEXT);
                DrawLine(rightPanelX + 20, 110, screenW - 20, 110, DASH_BORDER);
                
                string pseudo_active = "";
                if (!queen_steps.empty() && currentStep < (int)queen_steps.size()) {
                    pseudo_active = queen_steps[currentStep].pseudocode_highlight;
                }
                
                int py = 130;
                auto drawPsLine = [&](string text, string id, int indent) {
                    Color c = DASH_TEXT;
                    if (id == pseudo_active) c = DP_ACTIVE_CELL;
                    DrawTextSmooth(text.c_str(), rightPanelX + 20 + indent, py, 14, c);
                    py += 24;
                };
                
                drawPsLine("if (row == N) {", "return_true", 0);
                drawPsLine("  return true;", "return_true", 0);
                drawPsLine("}", "return_true", 0);
                drawPsLine("for (col = 0 to N-1) {", "for_loop", 0);
                drawPsLine("  if (isSafe(row, col)) {", "if_isSafe", 0);
                drawPsLine("    placeQueen();", "place", 0);
                drawPsLine("    solve(row + 1);", "solve", 0);
                drawPsLine("    removeQueen();", "remove", 0);
                drawPsLine("  }", "if_isSafe", 0);
                drawPsLine("}", "for_loop", 0);
            }
            
            // Playback Controls (Bottom Center)
            int pbY = screenH - 60;
            int pbCX = screenW / 2;
            bool prevHover = (mPos.x > pbCX - 150 && mPos.x < pbCX - 70 && mPos.y > pbY && mPos.y < pbY + 40);
            bool playHover = (mPos.x > pbCX - 60 && mPos.x < pbCX + 60 && mPos.y > pbY && mPos.y < pbY + 40);
            bool nextHover = (mPos.x > pbCX + 70 && mPos.x < pbCX + 150 && mPos.y > pbY && mPos.y < pbY + 40);
            
            // Draw Prev
            DrawRoundedRect(pbCX - 150, pbY, 80, 40, 0.2f, prevHover ? DASH_ACCENT : DASH_PANEL);
            DrawRoundedRectLines(pbCX - 150, pbY, 80, 40, 0.2f, 1.0f, DASH_BORDER);
            DrawTextSmoothCentered("< PREV", pbCX - 150, pbY + 12, 80, 16, prevHover ? DASH_PANEL : DASH_TEXT);
            
            // Draw Play/Pause
            DrawRoundedRect(pbCX - 60, pbY, 120, 40, 0.2f, playHover ? DASH_ACCENT : DASH_PANEL);
            DrawRoundedRectLines(pbCX - 60, pbY, 120, 40, 0.2f, 1.0f, DASH_BORDER);
            DrawTextSmoothCentered(playing ? "PAUSE" : "PLAY", pbCX - 60, pbY + 12, 120, 16, playHover ? DASH_PANEL : DASH_ACCENT);
            
            // Draw Next
            DrawRoundedRect(pbCX + 70, pbY, 80, 40, 0.2f, nextHover ? DASH_ACCENT : DASH_PANEL);
            DrawRoundedRectLines(pbCX + 70, pbY, 80, 40, 0.2f, 1.0f, DASH_BORDER);
            DrawTextSmoothCentered("NEXT >", pbCX + 70, pbY + 12, 80, 16, nextHover ? DASH_PANEL : DASH_TEXT);
            
            // Visual Area Config
            int areaLeft = leftPanelW + 40;
            int areaRight = rightPanelX - 40;
            int areaTop = 160;
            int areaBottom = screenH - 120;
            int areaW = areaRight - areaLeft;
            int areaH = areaBottom - areaTop;
            
            // Draw Box around visual area
            DrawRoundedRectLines(areaLeft - 10, areaTop - 10, areaW + 20, areaH + 20, 0.05f, 2.0f, DASH_BORDER);

            // Draw Real-time Status Log centrally above visual area
            if (strlen(realtime_status_msg) > 0) {
                // Determine highlight color based on message content
                Color msgColor = DASH_DIM;
                if (screen == 1) {
                    if (strstr(realtime_status_msg, "Swap") || strstr(realtime_status_msg, "swap") || strstr(realtime_status_msg, "right") || strstr(realtime_status_msg, "left")) msgColor = BAR_SWAPPING;
                    else if (strstr(realtime_status_msg, "Comparing") || strstr(realtime_status_msg, "comparing") || strstr(realtime_status_msg, "Target") || strstr(realtime_status_msg, "Match")) msgColor = BAR_COMPARING;
                    if (strstr(realtime_status_msg, "found") || strstr(realtime_status_msg, "Found")) msgColor = BAR_DONE;
                } else if (screen == 2) {
                    if (strstr(realtime_status_msg, "Discarding") || strstr(realtime_status_msg, "Moving")) msgColor = SEARCH_BOUNDS;
                    else if (strstr(realtime_status_msg, "Target") || strstr(realtime_status_msg, "Match")) msgColor = SEARCH_MID;
                    if (strstr(realtime_status_msg, "found at index")) msgColor = SEARCH_FOUND;
                    if (strstr(realtime_status_msg, "exhausted")) msgColor = SEARCH_DIM;
                }
                
                int tw = MeasureTextSmooth(realtime_status_msg, 20);
                DrawRoundedRect((screenW / 2) - (tw / 2) - 20, areaTop - 60, tw + 40, 40, 0.2f, msgColor);
                DrawTextSmooth(realtime_status_msg, (screenW / 2) - (tw / 2), areaTop - 50, 20, DASH_PANEL);
            }

            if (screen == 1) {
                if (steps.empty()) return;
                Step& cur = steps[currentStep];

            int n = originalArr.size();
            int maxVal = *max_element(originalArr.begin(), originalArr.end());
            if (maxVal < 1) maxVal = 1;
            
            float exactBarW = (float)areaW / n;
            float pad = exactBarW * 0.15f; 
            float t = (playing && currentStep < (int)steps.size() - 1) ? (float)timer / speed : 1.0f;
            
            vector<int> currPos(n);
            for(int p=0; p<n; p++) currPos[cur.indices[p]] = p;
            
            vector<int> nextPos = currPos;
            bool hasNext = (currentStep < (int)steps.size() - 1);
            if (hasNext) {
                for(int p=0; p<n; p++) nextPos[steps[currentStep+1].indices[p]] = p;
            }

            for (int k = 0; k < n; k++) {
                int val = originalArr[k];
                float barH = ((float)val / maxVal) * areaH; if (barH < 5) barH = 5;
                float p_interp = (float)currPos[k] + t * ((float)nextPos[k] - (float)currPos[k]);
                float x = areaLeft + p_interp * exactBarW, y = areaBottom - barH;
                
                int pos = (t > 0.5f && hasNext) ? nextPos[k] : currPos[k];
                Step& colorStep = (t > 0.5f && hasNext) ? steps[currentStep+1] : cur;
                
                bool stepDone = (colorStep.compared == -1);
                Color c = stepDone ? BAR_DONE : BAR_NORMAL;
                if (!stepDone) {
                    bool isActive = (colorStep.compared_sec == -1) ? (pos == colorStep.compared || pos == colorStep.compared + 1) : (pos == colorStep.compared || pos == colorStep.compared_sec);
                    if (isActive) c = colorStep.swapped ? BAR_SWAPPING : BAR_COMPARING;
                }
                
                // Draw bar with solid color
                DrawRoundedRect((int)(x + pad), (int)y, (int)(exactBarW - pad * 2.0f), (int)barH, 0.25f, c);
                // Draw a slightly darker outline
                Color outlineColor = { (unsigned char)(c.r * 0.8f), (unsigned char)(c.g * 0.8f), (unsigned char)(c.b * 0.8f), c.a };
                DrawRoundedRectLines((int)(x + pad), (int)y, (int)(exactBarW - pad * 2.0f), (int)barH, 0.25f, 2.0f, outlineColor);

                string numStr = to_string(val);
                string idxStr = to_string(k);
                
                int tw = MeasureTextSmooth(numStr.c_str(), 16);
                float tx = (x + pad) + ((exactBarW - pad*2) / 2.0f) - (tw / 2.0f);
                float ty = y - 24; // Above the bar
                if (ty < areaTop + 10) ty = y + 10; // If bar is too tall, put text inside
                
                // Draw array numerical value above the bar
                DrawTextSmooth(numStr.c_str(), (int)tx, (int)ty, 16, DASH_TEXT);
            }
            
            // Draw static physical indices below the area
            for (int i = 0; i < n; i++) {
                string idxStr = to_string(i);
                float fixed_x = areaLeft + i * exactBarW;
                int iw = MeasureTextSmooth(idxStr.c_str(), 14);
                float ix = (fixed_x + pad) + ((exactBarW - pad*2) / 2.0f) - (iw / 2.0f);
                DrawTextSmooth(idxStr.c_str(), (int)ix, areaBottom + 8, 14, DASH_DIM);
            }
            } else if (screen == 2) {
            if (searchSteps.empty()) return;
            SearchStep& cur = searchSteps[currentStep];

            int n = cur.arr.size();
            if (n == 0) return;
            
            float maxBoxW = 60.0f;
            float exactBoxW = (float)areaW / n;
            if (exactBoxW > maxBoxW) exactBoxW = maxBoxW;
            
            float pad = exactBoxW * 0.1f; 
            float totalW = n * exactBoxW;
            float startX = areaLeft + (areaW - totalW) / 2.0f; // Centered
            float centerY = areaTop + areaH / 2.0f;

            for (int k = 0; k < n; k++) {
                int val = cur.arr[k];
                float x = startX + k * exactBoxW;
                float y = centerY - exactBoxW / 2.0f; // Centered vertically
                
                Color c = SEARCH_NORMAL; 
                
                if (cur.done && cur.found && k == cur.mid) {
                    c = SEARCH_FOUND;
                } else if (!cur.done && k >= cur.lo && k <= cur.hi) {
                    c = SEARCH_NORMAL;
                } else if (!cur.done && (k < cur.lo || k > cur.hi)) {
                    c = SEARCH_DIM; // Dim out-of-bounds
                } else if (cur.done) {
                    c = SEARCH_DIM; // Dim all if not found
                }
                
                if (!cur.done) {
                    if (k == cur.lo) c = SEARCH_BOUNDS;
                    if (k == cur.hi) c = SEARCH_BOUNDS;
                    if (k == cur.mid) c = SEARCH_MID;
                }
                
                // Draw box
                DrawRoundedRect((int)(x + pad), (int)(y + pad), (int)(exactBoxW - pad * 2.0f), (int)(exactBoxW - pad * 2.0f), 0.15f, c);
                // Outline
                Color outlineColor = { (unsigned char)(c.r * 0.8f), (unsigned char)(c.g * 0.8f), (unsigned char)(c.b * 0.8f), c.a };
                DrawRoundedRectLines((int)(x + pad), (int)(y + pad), (int)(exactBoxW - pad * 2.0f), (int)(exactBoxW - pad * 2.0f), 0.15f, 2.0f, outlineColor);

                // Value inside the box
                string numStr = to_string(val);
                int fs = (int)(exactBoxW * 0.4f); if (fs < 10) fs = 10;
                int tw = MeasureTextSmooth(numStr.c_str(), fs);
                float tx = (x + exactBoxW / 2.0f) - (tw / 2.0f);
                float ty = y + exactBoxW / 2.0f - fs / 2.0f;
                DrawTextSmooth(numStr.c_str(), (int)tx, (int)ty, fs, DASH_TEXT);
                
                // Static physical index ABOVE the box
                string idxStr = to_string(k);
                int iw = MeasureTextSmooth(idxStr.c_str(), 14);
                float ix = (x + exactBoxW / 2.0f) - (iw / 2.0f);
                DrawTextSmooth(idxStr.c_str(), (int)ix, (int)(y - 25), 14, DASH_DIM);
                
                // Pointers BELOW the box
                if (!cur.done) {
                    int labelY_bot = y + exactBoxW + 10; 
                    if (k == cur.lo) DrawTextSmoothCentered("lo", (int)x, labelY_bot, (int)exactBoxW, 16, SEARCH_BOUNDS);
                    if (k == cur.hi) DrawTextSmoothCentered("hi", (int)x, labelY_bot + 20, (int)exactBoxW, 16, SEARCH_BOUNDS);
                    if (k == cur.mid) DrawTextSmoothCentered("mid", (int)x, labelY_bot, (int)exactBoxW, 16, SEARCH_MID);
                }
            }
            } else if (screen == 4) {
                if (bitmask_steps.empty()) return;
                BitmaskStep& cur = bitmask_steps[currentStep];
                
                int bits = 16; 
                float exactBoxW = 40.0f;
                float pad = 4.0f;
                float totalW = bits * exactBoxW;
                float startX = areaLeft + (areaW - totalW) / 2.0f;
                float startY = areaTop + 40.0f;
                
                // Active column highlight
                if (cur.active_bit_j >= 0 && cur.active_bit_j < bits) {
                    int visualIdx = (bits - 1) - cur.active_bit_j;
                    float hx = startX + visualIdx * exactBoxW;
                    DrawRoundedRect((int)hx, (int)startY - 20, (int)exactBoxW, 280, 0.1f, DASH_PANEL);
                    DrawRoundedRectLines((int)hx, (int)startY - 20, (int)exactBoxW, 280, 0.1f, 1.0f, BAR_COMPARING);
                }
                
                auto drawBinaryRow = [&](int value, float yPos, string rowLabel, string prefixLabel) {
                    for (int i = 0; i < bits; i++) {
                        int bitIndex = (bits - 1) - i; // MSB to LSB left to right
                        int bitVal = (value & (1 << bitIndex)) ? 1 : 0;
                        
                        float x = startX + i * exactBoxW;
                        Color c = (bitVal == 1) ? BIT_ONE : BIT_ZERO;
                        Color textC = (bitVal == 1) ? DASH_PANEL : DASH_TEXT;
                        
                        DrawRoundedRect((int)(x + pad), (int)(yPos + pad), (int)(exactBoxW - pad * 2), (int)(exactBoxW - pad * 2), 0.1f, c);
                        DrawRoundedRectLines((int)(x + pad), (int)(yPos + pad), (int)(exactBoxW - pad * 2), (int)(exactBoxW - pad * 2), 0.1f, 1.0f, BORDER_COLOR);
                        
                        DrawTextSmoothCentered(to_string(bitVal).c_str(), (int)x, (int)(yPos + exactBoxW/2 - 8), (int)exactBoxW, 18, textC);
                    }
                    
                    DrawTextSmooth(rowLabel.c_str(), startX + totalW + 20, yPos + exactBoxW/2 - 8, 18, DASH_TEXT);
                    if (!prefixLabel.empty()) {
                        DrawTextSmoothCentered(prefixLabel.c_str(), startX - 50, yPos + exactBoxW/2 - 12, 40, 24, DASH_TEXT);
                    }
                };
                
                // Row 1: Indices
                for (int i = 0; i < bits; i++) {
                    int bitIndex = (bits - 1) - i;
                    float x = startX + i * exactBoxW;
                    DrawTextSmoothCentered(to_string(bitIndex).c_str(), (int)x, startY, (int)exactBoxW, 14, DASH_DIM);
                }
                DrawTextSmooth("= index", startX + totalW + 20, startY, 14, DASH_DIM);
                
                // Row 2: S
                drawBinaryRow(cur.S, startY + 40, "= S (" + to_string(cur.S) + ")", "");
                
                // Row 3: Mask
                drawBinaryRow(cur.mask, startY + 100, "= mask (" + to_string(cur.mask) + ")", cur.operation_symbol);
                
                // Line
                DrawLineEx({startX, startY + 160}, {startX + totalW, startY + 160}, 3.0f, DASH_BORDER);
                
                // Row 4: Result
                drawBinaryRow(cur.result, startY + 180, "= result (" + to_string(cur.result) + ")", "=");
            } else if (screen == 5) {
                int leftPanelW = 260;
                int rightPanelW = 260;
                int rightPanelX = screenW - rightPanelW;
                int centerW = rightPanelX - leftPanelW;
                
                int maxGridW = centerW - 40;
                int maxGridH = screenH - 120;
                
                int cellSizeW = maxGridW / grid_cols;
                int cellSizeH = maxGridH / grid_rows;
                int cellSize = (cellSizeW < cellSizeH) ? cellSizeW : cellSizeH;
                
                int gridW = grid_cols * cellSize;
                int gridH = grid_rows * cellSize;
                
                int gridX = leftPanelW + (centerW - gridW) / 2;
                int gridY = 40 + (maxGridH - gridH) / 2;
                
                for (int r=0; r<grid_rows; r++) {
                    for (int c=0; c<grid_cols; c++) {
                        int x = gridX + c * cellSize;
                        int y = gridY + r * cellSize;
                        
                        CellState st = EMPTY;
                        if (!path_steps.empty() && currentStep < (int)path_steps.size()) {
                            st = path_steps[currentStep].grid[r][c];
                        } else {
                            if (make_pair(r, c) == start_node) st = START;
                            else if (make_pair(r, c) == end_node) st = END;
                            else if (walls[r][c]) st = WALL;
                        }
                        
                        Color cellC = GRID_EMPTY;
                        if (st == START) cellC = GRID_START;
                        else if (st == END) cellC = GRID_END;
                        else if (st == WALL) cellC = GRID_WALL;
                        else if (st == VISITED) cellC = GRID_VISITED;
                        else if (st == FRONTIER) cellC = GRID_FRONTIER;
                        else if (st == PATH) cellC = GRID_PATH;
                        
                        DrawRectangle(x, y, cellSize, cellSize, cellC);
                        DrawRectangleLines(x, y, cellSize, cellSize, DASH_BORDER);
                    }
                }
            } else if (screen == 6) {
                int leftPanelW = 260;
                int rightPanelW = 260;
                int rightPanelX = screenW - rightPanelW;
                int centerW = rightPanelX - leftPanelW;
                
                int maxGridW = centerW - 60;
                int maxGridH = screenH - 140;
                
                int rows = dp_items.size() + 1;
                int cols = dp_capacity + 1;
                
                int cellSizeW = maxGridW / cols;
                int cellSizeH = maxGridH / rows;
                int cellSize = (cellSizeW < cellSizeH) ? cellSizeW : cellSizeH;
                
                int gridW = cols * cellSize;
                int gridH = rows * cellSize;
                
                int gridX = leftPanelW + (centerW - gridW) / 2;
                int gridY = 60 + (maxGridH - gridH) / 2;
                
                for (int c = 0; c < cols; c++) {
                    DrawTextSmoothCentered(to_string(c).c_str(), gridX + c * cellSize, gridY - 25, cellSize, 16, DASH_DIM);
                }
                
                for (int r = 0; r < rows; r++) {
                    string rowLbl = (r == 0) ? "0" : "wt:" + to_string(dp_items[r-1].weight) + " v:" + to_string(dp_items[r-1].value);
                    int lblW = MeasureTextSmooth(rowLbl.c_str(), 14);
                    DrawTextSmooth(rowLbl.c_str(), gridX - lblW - 10, gridY + r * cellSize + (cellSize/2) - 7, 14, DASH_DIM);
                    
                    for (int c = 0; c < cols; c++) {
                        int x = gridX + c * cellSize;
                        int y = gridY + r * cellSize;
                        
                        Color cellC = DP_EMPTY;
                        int val = 0;
                        bool isActive = false;
                        bool isEmpty = true;
                        
                        if (!dp_steps.empty() && currentStep < (int)dp_steps.size()) {
                            DPStep& cur = dp_steps[currentStep];
                            val = cur.current_table_state[r][c];
                            
                            if (r == cur.current_i && c == cur.current_w) { cellC = DP_ACTIVE_CELL; isActive = true; isEmpty = false; }
                            else if (r == cur.check_row_1 && c == cur.check_col_1) { cellC = DP_DEPENDENCY; isEmpty = false; }
                            else if (r == cur.check_row_2 && c == cur.check_col_2) { cellC = DP_DEPENDENCY; isEmpty = false; }
                            else if (val > 0 || r < cur.current_i || (r == cur.current_i && c < cur.current_w)) { cellC = DP_FILLED; isEmpty = false; }
                        }
                        
                        DrawRectangle(x, y, cellSize, cellSize, cellC);
                        DrawRectangleLines(x, y, cellSize, cellSize, DASH_BORDER);
                        
                        if (val > 0) {
                            DrawTextSmoothCentered(to_string(val).c_str(), x, y + cellSize/2 - 8, cellSize, 16, isActive ? DASH_PANEL : DASH_TEXT);
                        } else if (!isEmpty) {
                            DrawTextSmoothCentered("0", x, y + cellSize/2 - 8, cellSize, 16, isActive ? DASH_PANEL : DASH_TEXT);
                        }
                    }
                }
            } else if (screen == 7) {
                int leftPanelW = 260;
                int rightPanelW = 260;
                int rightPanelX = screenW - rightPanelW;
                int centerW = rightPanelX - leftPanelW;
                
                int maxGridW = centerW - 60;
                int maxGridH = screenH - 180; // slightly more space for text
                
                // Keep the board perfectly square
                int N = n_queens_size;
                int cellSize = min(maxGridW / N, maxGridH / N);
                int boardW = cellSize * N;
                
                int boardX = leftPanelW + (centerW - boardW) / 2;
                int boardY = 100;
                
                QueenStep current_q_step;
                bool has_step = false;
                if (!queen_steps.empty() && currentStep < (int)queen_steps.size()) {
                    current_q_step = queen_steps[currentStep];
                    has_step = true;
                }
                
                // Draw action text
                if (has_step) {
                    int tw = MeasureTextSmooth(current_q_step.action_text.c_str(), 20);
                    DrawRoundedRect(leftPanelW + centerW/2 - tw/2 - 20, 30, tw + 40, 40, 0.2f, DASH_PANEL);
                    DrawRoundedRectLines(leftPanelW + centerW/2 - tw/2 - 20, 30, tw + 40, 40, 0.2f, 1.0f, DASH_BORDER);
                    DrawTextSmooth(current_q_step.action_text.c_str(), leftPanelW + centerW/2 - tw/2, 40, 20, current_q_step.is_conflict ? CONFLICT_COLOR : DASH_TEXT);
                }
                
                // Draw Board
                for (int r = 0; r < N; r++) {
                    for (int c = 0; c < N; c++) {
                        Color sqColor = ((r + c) % 2 == 0) ? BOARD_LIGHT : BOARD_DARK;
                        DrawRectangle(boardX + c*cellSize, boardY + r*cellSize, cellSize, cellSize, sqColor);
                        DrawRectangleLines(boardX + c*cellSize, boardY + r*cellSize, cellSize, cellSize, DASH_BORDER);
                    }
                }
                
                // Draw conflict highlights
                if (has_step && current_q_step.is_conflict) {
                    int r = current_q_step.current_row;
                    int c = current_q_step.current_col;
                    
                    // Highlight the row and col
                    DrawRectangle(boardX, boardY + r*cellSize, boardW, cellSize, CONFLICT_COLOR);
                    DrawRectangle(boardX + c*cellSize, boardY, cellSize, boardW, CONFLICT_COLOR);
                    
                    // Highlight diagonals safely
                    for(int i=0; i<N; i++) {
                        for(int j=0; j<N; j++) {
                            if (abs(i - r) == abs(j - c)) {
                                DrawRectangle(boardX + j*cellSize, boardY + i*cellSize, cellSize, cellSize, CONFLICT_COLOR);
                            }
                        }
                    }
                }
                
                // Draw Queens
                vector<int> current_board = has_step ? current_q_step.board : vector<int>(N, -1);
                for (int r = 0; r < N; r++) {
                    if (current_board[r] != -1) {
                        int c = current_board[r];
                        int cx = boardX + c*cellSize + cellSize/2;
                        int cy = boardY + r*cellSize + cellSize/2;
                        int radius = (int)(cellSize * 0.35f);
                        DrawCircle(cx, cy, radius, QUEEN_COLOR);
                        DrawCircleLines(cx, cy, radius, DASH_BORDER);
                        DrawTextSmoothCentered("Q", cx - radius, cy - 10, radius*2, 20, DASH_PANEL);
                    }
                }
                
                // Optional: Outline the currently active cell being tested
                if (has_step && current_q_step.current_row != -1 && current_q_step.current_col != -1) {
                    DrawRectangleLinesEx({(float)boardX + current_q_step.current_col*cellSize, (float)boardY + current_q_step.current_row*cellSize, (float)cellSize, (float)cellSize}, 4.0f, DASH_ACCENT);
                }
                
            }
        } else if (screen == 3) {
            ClearBackground(DASH_BG);
            Vector2 mPos = GetMousePosition();
            drawDashboardTopBar(screenW, mPos);
            
            if (graphSteps.empty()) return;
            GraphStep& cur = graphSteps[currentStep];
            
            if (cur.done) {
                DrawTextSmooth("COMPLETE", screenW - 200, 80, 26, BAR_DONE);
            }

            int areaTop = 140;
            int areaBottom = screenH - 40;
            int availH = areaBottom - areaTop;
            int availW = screenW - 40;
            float offsetX = 20.0f;
            float offsetY = areaTop;

            auto getNodePos = [&](int idx) -> Vector2 {
                return { offsetX + graphNodes[idx].x * availW, offsetY + graphNodes[idx].y * availH };
            };

            for (size_t i = 0; i < graphNodes.size(); i++) {
                Vector2 p1 = getNodePos(i);
                if (graphNodes[i].leftId != -1) {
                    Vector2 p2 = getNodePos(graphNodes[i].leftId);
                    DrawLineEx(p1, p2, 3.0f, DASH_BORDER);
                }
                if (graphNodes[i].rightId != -1) {
                    Vector2 p2 = getNodePos(graphNodes[i].rightId);
                    DrawLineEx(p1, p2, 3.0f, DASH_BORDER);
                }
            }

            float nodeRadius = 24.0f;
            if (graphNodes.size() > 30) nodeRadius = 18.0f;
            if (graphNodes.size() > 60) nodeRadius = 14.0f;

            for (size_t i = 0; i < graphNodes.size(); i++) {
                Vector2 pos = getNodePos(i);
                Color c = DASH_PANEL;
                Color borderC = DASH_BORDER;
                Color textC = DASH_TEXT;
                
                if (find(cur.visitedSequence.begin(), cur.visitedSequence.end(), i) != cur.visitedSequence.end()) {
                    c = BAR_DONE; borderC = BAR_DONE; textC = DASH_PANEL;
                } else if (find(cur.activeStack.begin(), cur.activeStack.end(), i) != cur.activeStack.end()) {
                    c = BAR_SWAPPING; borderC = BAR_SWAPPING; textC = DASH_PANEL;
                }

                if (cur.currentId == i && !cur.done) {
                    c = BAR_COMPARING; borderC = BAR_COMPARING; textC = DASH_TEXT;
                }

                DrawCircleV(pos, nodeRadius + 3.0f, borderC); 
                DrawCircleV(pos, nodeRadius, c); 

                int fontSize = (int)(nodeRadius * 1.2f);
                string valStr = to_string(graphNodes[i].value);
                DrawTextSmoothCentered(valStr.c_str(), (int)(pos.x - nodeRadius), (int)(pos.y - fontSize*0.4f), (int)(nodeRadius*2), fontSize, textC);
            }
        }
    }
};

#endif // APP_H
