/*
 * dp.h — 0/1 Knapsack dynamic programming step generator.
 * Fills the dp table bottom-up and records one DPStep per cell so the
 * visualizer can highlight the active cell and its dependencies.
 */

#ifndef DP_H
#define DP_H

#include <vector>
#include <algorithm>

using namespace std;

// One item available for the knapsack
struct Item {
    int weight; // Weight units consumed
    int value;  // Profit gained by including this item
};

// One frame of the DP table animation
struct DPStep {
    int current_i;   // Row being filled (item index 1..N)
    int current_w;   // Column being filled (capacity 1..W)
    int check_row_1; // Dependency: dp[i-1][w] — value when item is skipped
    int check_col_1;
    int check_row_2; // Dependency: dp[i-1][w-weight] — value when item is included (-1 if too heavy)
    int check_col_2;
    int chosen_value;                    // Value written into dp[i][w]
    vector<vector<int>> current_table_state; // Full table snapshot after this cell is set
};

// Runs 0/1 Knapsack and returns one step per table cell filled
inline vector<DPStep> runKnapsack(int capacity, const vector<Item>& items) {
    vector<DPStep> steps;
    int n = items.size();
    // dp[i][w] = max value using first i items with capacity w
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));

    steps.push_back({0, 0, -1, -1, -1, -1, 0, dp}); // Initial empty table

    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= capacity; w++) {
            DPStep step;
            step.current_i = i;
            step.current_w = w;
            step.check_row_1 = i - 1; // Skip item i → read from row above, same column
            step.check_col_1 = w;
            step.check_row_2 = -1;    // Assume item is too heavy; set below if it fits
            step.check_col_2 = -1;

            int val_exclude = dp[i - 1][w];
            int val_include = -1;

            if (items[i - 1].weight <= w) {
                // Item fits — the include dependency is dp[i-1][w - weight]
                step.check_row_2 = i - 1;
                step.check_col_2 = w - items[i - 1].weight;
                val_include = dp[step.check_row_2][step.check_col_2] + items[i - 1].value;
            }

            // Choose the better of excluding or including item i
            int chosen = max(val_exclude, val_include > -1 ? val_include : -1);
            dp[i][w] = chosen;

            step.chosen_value        = chosen;
            step.current_table_state = dp;
            steps.push_back(step);
        }
    }

    return steps;
}

#endif // DP_H
