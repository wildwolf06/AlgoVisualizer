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
