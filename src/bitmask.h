/*
 * bitmask.h — Bitwise operation step generators.
 * Each function records the mask creation and application as BitmaskSteps
 * so the visualizer can highlight individual bits during playback.
 */

#ifndef BITMASK_H
#define BITMASK_H

#include <string>
#include <vector>

using namespace std;

// One frame of a bitwise operation animation
struct BitmaskStep {
    int    S;                    // The original input number
    int    mask;                 // The mask used in this step
    int    result;               // The running result after this step
    int    active_bit_j;         // Bit position to highlight (-1 = none)
    string pseudocode_highlight; // Status text shown in the info panel
    string operation_symbol;     // Operator label: "+", "&", "|", "^"
    int    highlight_line_idx;   // Pseudocode line to highlight
};

// Increment (S + 1) — flips trailing 1-bits to 0 until the first 0-bit is set to 1
inline vector<BitmaskStep> incrementBits(int S) {
    vector<BitmaskStep> steps;
    int current = S, j = 0;
    steps.push_back({current, 0, current, -1, "Start Incrementing S.", "+", 1});

    while ((current & (1 << j)) != 0 && j < 16) {
        int mask = (1 << j);
        steps.push_back({current, mask, current, j, "Bit " + to_string(j) + " is 1. Flip it to 0.", "^", 1});
        current ^= mask;
        steps.push_back({current, mask, current, j, "Result after flipping bit " + to_string(j) + " to 0.", "^", 1});
        j++;
    }
    if (j < 16) {
        int mask = (1 << j);
        steps.push_back({current, mask, current, j, "Found first 0 at bit " + to_string(j) + ". Flip it to 1.", "^", 1});
        current ^= mask;
        steps.push_back({current, mask, current, j, "Increment complete. S = S + 1.", "^", 0});
    }
    steps.push_back({S, 1, current, -1, "Final Result.", "+", 0});
    return steps;
}

// Set Bit — forces bit j to 1 using S | (1 << j)
inline vector<BitmaskStep> setBit(int S, int j) {
    vector<BitmaskStep> steps;
    steps.push_back({S, 0, S, -1, "Goal: Set bit " + to_string(j) + " to 1.", "|", 0});
    int mask = (1 << j);
    steps.push_back({S, mask, S, j, "Step 1: Create mask = (1 << " + to_string(j) + ").", "|", 0});
    int res = S | mask;
    steps.push_back({S, mask, res, j, "Step 2: S | mask sets bit " + to_string(j) + " to 1.", "|", 1});
    return steps;
}

// Check Bit — tests whether bit j is 1 using S & (1 << j)
inline vector<BitmaskStep> checkBit(int S, int j) {
    vector<BitmaskStep> steps;
    steps.push_back({S, 0, S, -1, "Goal: Check if bit " + to_string(j) + " is 1.", "&", 0});
    int mask = (1 << j);
    steps.push_back({S, mask, S, j, "Step 1: Create mask = (1 << " + to_string(j) + ").", "&", 0});
    int res = S & mask;
    string resultText = (res != 0) ? "Non-zero result. Bit is 1." : "Zero result. Bit is 0.";
    steps.push_back({S, mask, res, j, "Step 2: S & mask isolates bit " + to_string(j) + ". " + resultText, "&", 1});
    steps.push_back({S, mask, res, j, resultText, "&", 2});
    return steps;
}

// Clear Bit — forces bit j to 0 using S & ~(1 << j)
inline vector<BitmaskStep> clearBit(int S, int j) {
    vector<BitmaskStep> steps;
    steps.push_back({S, 0, S, -1, "Goal: Clear bit " + to_string(j) + " to 0.", "&", 0});
    int base_mask = (1 << j);
    steps.push_back({S, base_mask, S, j, "Step 1: Create base mask = (1 << " + to_string(j) + ").", "&", 0});
    int mask = ~base_mask & 0xFFFF; // Truncated to 16 bits for display
    steps.push_back({S, mask, S, j, "Step 2: Invert mask = ~(1 << " + to_string(j) + ").", "&", 0});
    int res = S & mask;
    steps.push_back({S, mask, res, j, "Step 3: S & mask clears bit " + to_string(j) + " to 0.", "&", 1});
    return steps;
}

// Toggle Bit — flips bit j using S ^ (1 << j)
inline vector<BitmaskStep> toggleBit(int S, int j) {
    vector<BitmaskStep> steps;
    steps.push_back({S, 0, S, -1, "Goal: Toggle bit " + to_string(j) + ".", "^", 0});
    int mask = (1 << j);
    steps.push_back({S, mask, S, j, "Step 1: Create mask = (1 << " + to_string(j) + ").", "^", 0});
    int res = S ^ mask;
    steps.push_back({S, mask, res, j, "Step 2: S ^ mask flips bit " + to_string(j) + ".", "^", 1});
    return steps;
}

// Least Significant Bit — isolates the rightmost 1-bit using S & (-S)
inline vector<BitmaskStep> leastSignificantBit(int S) {
    vector<BitmaskStep> steps;
    steps.push_back({S, 0, S, -1, "Goal: Isolate Least Significant Bit.", "&", 0});
    int mask = -S & 0xFFFF; // Two's complement, truncated to 16 bits for display
    steps.push_back({S, mask, S, -1, "Step 1: Create two's complement mask = -S.", "&", 0});
    int res = S & (-S);
    int j = 0;
    if (res != 0) while ((res & (1 << j)) == 0) j++;
    else          j = -1;
    steps.push_back({S, mask, res, j, "Step 2: S & (-S) isolates the rightmost 1-bit.", "&", 1});
    return steps;
}

#endif // BITMASK_H
