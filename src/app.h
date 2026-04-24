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
