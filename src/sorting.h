#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <algorithm>

using namespace std;

// ── SNAPSHOT OF THE ARRAY ──
struct Step {
    vector<int> arr;
    int compared;
    bool swapped;
};

// ── CORE ALGORITHM (Bubble Sort) ──
// Returns a list of all steps taken during the sort.
inline vector<Step> bubbleSort(vector<int> arr) {
    vector<Step> steps;
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            bool didSwap = false;
            
            // Compare and swap
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                didSwap = true;
            }

            // Save the state after this comparison/swap
            Step s;
            s.arr      = arr;
            s.compared = j;
            s.swapped  = didSwap;
            steps.push_back(s);
        }
    }
    
    // Final state (meaning done)
    Step done;
    done.arr      = arr;
    done.compared = -1;
    done.swapped  = false;
    steps.push_back(done);

    return steps;
}

#endif 