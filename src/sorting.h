#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <algorithm>

using namespace std;

// ── SNAPSHOT OF THE ARRAY ──
struct Step {
    vector<int> arr;
    int compared;
    int compared_sec = -1;
    bool swapped;
};

// ── CORE ALGORITHM (Bubble Sort) ──
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
//  (Insertion Sort) 
inline vector<Step> insertionSort(vector<int> arr) {
    vector<Step> steps;
    int n = arr.size();

    for (int i = 1; i < n; i++) {
        int j = i;
        while (j > 0) {
            // Compare arr[j-1] and arr[j]
            Step s;
            s.arr = arr;
            s.compared = j - 1;
            s.swapped = false;
            steps.push_back(s);

            if (arr[j - 1] > arr[j]) {
                swap(arr[j - 1], arr[j]);
                Step sSwap;
                sSwap.arr = arr;
                sSwap.compared = j - 1;
                sSwap.swapped = true;
                steps.push_back(sSwap);
                j--;
            } else {
                break; // No need to compare further, it's sorted
            }
        }
    }
    
    Step done;
    done.arr      = arr;
    done.compared = -1;
    done.swapped  = false;
    steps.push_back(done);

    return steps;
}

inline int countSwaps(const vector<Step>& steps, int upTo) {
    int count = 0;
    for (int i = 0; i <= upTo; i++) {
        if (steps[i].swapped) count++;
    }
    return count;
}

inline vector<int> makeRandomArray(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) arr[i] = i + 1;
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(arr[i], arr[j]);
    }
    return arr;
}


#endif 