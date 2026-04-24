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
inline vector<Step> bubbleSort(vector<int> arr, bool sortAscending = true) {
    vector<Step> steps;
    int n = arr.size();
    vector<int> idx(n);
    for (int i = 0; i < n; i++) idx[i] = i;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            bool didSwap = false;
            bool condition = sortAscending ? (arr[j] > arr[j + 1]) : (arr[j] < arr[j + 1]);
            if (condition) {
                swap(arr[j], arr[j + 1]);
                swap(idx[j], idx[j + 1]);
                didSwap = true;
            }
            Step s;
            s.arr = arr; s.indices = idx; s.compared = j; s.swapped = didSwap;
            s.isComparison = true; s.sortAscending = sortAscending;
            steps.push_back(s);
        }
    }
    Step done; done.arr = arr; done.indices = idx; done.compared = -1;
    done.swapped = false; done.sortAscending = sortAscending; steps.push_back(done);
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
            Step s; s.arr = arr; s.indices = idx; s.compared = j - 1; s.swapped = false;
            s.isComparison = true; s.sortAscending = sortAscending; steps.push_back(s);

            bool condition = sortAscending ? (arr[j - 1] > arr[j]) : (arr[j - 1] < arr[j]);
            if (condition) {
                swap(arr[j - 1], arr[j]);
                swap(idx[j - 1], idx[j]);
                Step sSwap; sSwap.arr = arr; sSwap.indices = idx; sSwap.compared = j - 1;
                sSwap.swapped = true; sSwap.isComparison = false; sSwap.sortAscending = sortAscending;
                steps.push_back(sSwap);
                j--;
            } else break; 
        }
    }
    Step done; done.arr = arr; done.indices = idx; done.compared = -1;
    done.swapped = false; done.sortAscending = sortAscending; steps.push_back(done);
    return steps;
}

#endif // SORTING_H
