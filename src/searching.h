#ifndef SEARCHING_H
#define SEARCHING_H

#include <vector>
#include <algorithm>

using namespace std;

// ── SNAPSHOT OF THE ARRAY FOR SEARCHING ──
struct SearchStep {
    vector<int> arr;
    int lo;
    int hi;
    int mid;
    int target;
    bool found;
    bool done;
};

// ── CORE ALGORITHM (Binary Search) ──
inline vector<SearchStep> binarySearch(vector<int> arr, int target) {
    vector<SearchStep> steps;
    sort(arr.begin(), arr.end());

    int lo = 0;
    int hi = arr.size() - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        SearchStep s; s.arr = arr; s.lo = lo; s.hi = hi; s.mid = mid;
        s.target = target; s.found = false; s.done = false;
        steps.push_back(s); 

        if (arr[mid] == target) {
            SearchStep foundStep = s; foundStep.found = true; foundStep.done = true;
            steps.push_back(foundStep);
            return steps;
        }

        if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }

    SearchStep notFound; notFound.arr = arr; notFound.lo = lo; notFound.hi = hi;
    notFound.mid = -1; notFound.target = target; notFound.found = false; notFound.done = true;
    steps.push_back(notFound);

    return steps;
}

#endif // SEARCHING_H
