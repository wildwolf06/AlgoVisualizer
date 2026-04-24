#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <algorithm>

using namespace std;

// ── SNAPSHOT OF THE ARRAY ──
struct Step {
    vector<int> arr;
    vector<int> indices;
    int compared;
    int compared_sec = -1;
    bool swapped;
    bool isComparison = false;
    bool sortAscending = true;
};

// ── CORE ALGORITHM (Bubble Sort) ──
// Returns a list of all steps taken during the sort.
inline vector<Step> bubbleSort(vector<int> arr, bool sortAscending = true) {
    vector<Step> steps;
    int n = arr.size();
    vector<int> idx(n);
    for (int i = 0; i < n; i++) idx[i] = i;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            bool didSwap = false;
            
            // Compare and swap
            bool condition = sortAscending ? (arr[j] > arr[j + 1]) : (arr[j] < arr[j + 1]);
            if (condition) {
                swap(arr[j], arr[j + 1]);
                swap(idx[j], idx[j + 1]);
                didSwap = true;
            }

            // Save the state after this comparison/swap
            Step s;
            s.arr      = arr;
            s.indices  = idx;
            s.compared = j;
            s.swapped  = didSwap;
            s.isComparison = true;
            s.sortAscending = sortAscending;
            steps.push_back(s);
        }
    }
    
    // Final state (meaning done)
    Step done;
    done.arr      = arr;
    done.indices  = idx;
    done.compared = -1;
    done.swapped  = false;
    done.sortAscending = sortAscending;
    steps.push_back(done);

    return steps;
}

// ── CORE ALGORITHM (Insertion Sort) ──
inline vector<Step> insertionSort(vector<int> arr, bool sortAscending = true) {
    vector<Step> steps;
    int n = arr.size();
    vector<int> idx(n);
    for (int i = 0; i < n; i++) idx[i] = i;

    for (int i = 1; i < n; i++) {
        int j = i;
        while (j > 0) {
            // Compare arr[j-1] and arr[j]
            Step s;
            s.arr = arr;
            s.indices = idx;
            s.compared = j - 1;
            s.swapped = false;
            s.isComparison = true;
            s.sortAscending = sortAscending;
            steps.push_back(s);

            bool condition = sortAscending ? (arr[j - 1] > arr[j]) : (arr[j - 1] < arr[j]);
            if (condition) {
                swap(arr[j - 1], arr[j]);
                swap(idx[j - 1], idx[j]);
                Step sSwap;
                sSwap.arr = arr;
                sSwap.indices = idx;
                sSwap.compared = j - 1;
                sSwap.swapped = true;
                sSwap.isComparison = false;
                sSwap.sortAscending = sortAscending;
                steps.push_back(sSwap);
                j--;
            } else {
                break; // No need to compare further, it's sorted
            }
        }
    }
    
    Step done;
    done.arr      = arr;
    done.indices  = idx;
    done.compared = -1;
    done.swapped  = false;
    done.sortAscending = sortAscending;
    steps.push_back(done);

    return steps;
}

// ── CORE ALGORITHM (Selection Sort) ──
inline vector<Step> selectionSort(vector<int> arr, bool sortAscending = true) {
    vector<Step> steps;
    int n = arr.size();
    vector<int> idx(n);
    for (int i = 0; i < n; i++) idx[i] = i;

    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            Step s;
            s.arr = arr;
            s.indices = idx;
            s.compared = min_idx;
            s.compared_sec = j;
            s.swapped = false;
            s.isComparison = true;
            s.sortAscending = sortAscending;
            steps.push_back(s);

            bool condition = sortAscending ? (arr[j] < arr[min_idx]) : (arr[j] > arr[min_idx]);
            if (condition) {
                min_idx = j;
            }
        }
        
        if (min_idx != i) {
            swap(arr[i], arr[min_idx]);
            swap(idx[i], idx[min_idx]);
            Step sSwap;
            sSwap.arr = arr;
            sSwap.indices = idx;
            sSwap.compared = i;
            sSwap.compared_sec = min_idx;
            sSwap.swapped = true;
            sSwap.isComparison = false;
            sSwap.sortAscending = sortAscending;
            steps.push_back(sSwap);
        }
    }
    
    Step done;
    done.arr      = arr;
    done.indices  = idx;
    done.compared = -1;
    done.swapped  = false;
    done.sortAscending = sortAscending;
    steps.push_back(done);

    return steps;
}

// ── HELPER: Count number of swaps up to current step ──
inline int countSwaps(const vector<Step>& steps, int upTo) {
    int count = 0;
    for (int i = 0; i <= upTo; i++) {
        if (steps[i].swapped) count++;
    }
    return count;
}

// ── HELPER: Count number of comparisons up to current step ──
inline int countComparisons(const vector<Step>& steps, int upTo) {
    int count = 0;
    for (int i = 0; i <= upTo; i++) {
        if (steps[i].isComparison) count++;
    }
    return count;
}

// ── HELPER: Generate Random Array ──
inline vector<int> makeRandomArray(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) arr[i] = i + 1;
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(arr[i], arr[j]);
    }
    return arr;
}

// ── HELPER: Generate Sorted Array ──
inline vector<int> makeSortedArray(int n, bool nonDecreasing) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) arr[i] = i + 1;
    if (!nonDecreasing) {
        reverse(arr.begin(), arr.end());
    }
    return arr;
}

// ── HELPER: Generate Many Duplicates ──
inline vector<int> makeDuplicatesArray(int n) {
    vector<int> arr(n);
    int numGroups = max(2, n / 4);
    for (int i = 0; i < n; i++) {
        arr[i] = (rand() % numGroups) + 1;
    }
    return arr;
}

#endif // SORTING_H
