/*
 * backtracking.h — N-Queens solver using recursive backtracking.
 * Records every placement attempt, conflict detection, and removal as a
 * QueenStep so the visualizer can animate the full search including backtracks.
 */

#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include <vector>
#include <string>
#include <cmath>

using namespace std;

// One frame of the N-Queens animation
struct QueenStep {
    vector<int> board;           // board[row] = column of placed queen, -1 if empty
    int         current_row;     // Row currently being examined (-1 if N/A)
    int         current_col;     // Column currently being tested (-1 if N/A)
    bool        is_conflict;     // True if a conflict was detected at (row, col)
    string      action_text;     // Description shown in the visualizer status bar
    string      pseudocode_highlight; // Tag for highlighting the pseudocode panel
};

// Checks if placing a queen at (row, col) conflicts with any already-placed queen.
// Checks column and both diagonals; appends a conflict step if a clash is found.
inline bool isSafe(const vector<int>& board, int row, int col, vector<QueenStep>& steps) {
    for (int i = 0; i < row; i++) {
        if (board[i] == col || abs(board[i] - col) == abs(i - row)) {
            steps.push_back({board, row, col, true,
                "Conflict detected at Row " + to_string(row) + ", Col " + to_string(col),
                "if_isSafe"});
            return false;
        }
    }
    return true;
}

// Recursive backtracking — tries every column in the current row, recurses on success,
// removes the queen and tries the next column when stuck (backtrack).
inline void solveNQueensRec(int N, int row, vector<int>& board,
                            vector<QueenStep>& steps, bool& solved) {
    if (row == N) {
        // All rows filled — a valid solution has been found
        steps.push_back({board, -1, -1, false, "Solution Found!", "return_true"});
        return;
    }

    for (int col = 0; col < N; col++) {
        steps.push_back({board, row, col, false,
            "Checking Row " + to_string(row) + ", Col " + to_string(col), "for_loop"});

        if (isSafe(board, row, col, steps)) {
            board[row] = col;
            steps.push_back({board, row, col, false,
                "Placed Queen at Row " + to_string(row) + ", Col " + to_string(col), "place"});

            solveNQueensRec(N, row + 1, board, steps, solved);

            // Backtrack: remove the queen and try the next column
            board[row] = -1;
            steps.push_back({board, row, col, false,
                "Backtracking from Row " + to_string(row) + ", Col " + to_string(col), "remove"});
        }
    }
}

// Entry point — initializes the board and runs the solver
inline vector<QueenStep> solveNQueens(int N) {
    vector<QueenStep> steps;
    vector<int> board(N, -1); // All rows start empty
    bool solved = false;

    steps.push_back({board, -1, -1, false, "Starting N-Queens for N=" + to_string(N), "start"});
    solveNQueensRec(N, 0, board, steps, solved);
    steps.push_back({board, -1, -1, false, "Search Complete", "end"});

    return steps;
}

#endif // BACKTRACKING_H
