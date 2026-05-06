/*
 * pathfinding.h — BFS grid pathfinding step generator.
 * Records each cell state change (VISITED, FRONTIER, PATH) as a GridStep
 * so the visualizer can animate the search wave expanding across the grid.
 */

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <vector>
#include <utility>
#include <queue>
#include <algorithm>

using namespace std;

// Visual/logical state of each grid cell
enum CellState { EMPTY, WALL, START, END, VISITED, FRONTIER, PATH };

// One frame of the pathfinding animation (full grid snapshot)
struct GridStep {
    vector<vector<CellState>> grid;
};

// Runs BFS on the grid and returns all animation steps.
// Records FRONTIER expansion, VISITED marking, and final PATH reveal.
inline vector<GridStep> runBFS(int rows, int cols,
                               pair<int,int> start, pair<int,int> end,
                               vector<vector<bool>>& walls) {
    vector<GridStep> steps;

    // Build the initial grid: walls, start, and end cells
    vector<vector<CellState>> currentGrid(rows, vector<CellState>(cols, EMPTY));
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (walls[r][c]) currentGrid[r][c] = WALL;
    currentGrid[start.first][start.second] = START;
    currentGrid[end.first][end.second]     = END;
    steps.push_back({currentGrid}); // Frame 0: initial state

    queue<pair<int,int>> q;
    vector<vector<bool>>          visited(rows, vector<bool>(cols, false));
    // parent[r][c] stores where we came from; used to trace the final path
    vector<vector<pair<int,int>>> parent(rows, vector<pair<int,int>>(cols, {-1, -1}));

    q.push(start);
    visited[start.first][start.second] = true;

    int dr[] = {-1, 1, 0, 0}; // Up, Down, Left, Right
    int dc[] = { 0, 0,-1, 1};
    bool found = false;

    while (!q.empty()) {
        auto curr = q.front(); q.pop();

        // Mark cell as fully visited (don't overwrite START or END colors)
        if (curr != start && curr != end) {
            currentGrid[curr.first][curr.second] = VISITED;
            steps.push_back({currentGrid});
        }

        if (curr == end) { found = true; break; }

        // Expand to all 4 neighbours
        for (int i = 0; i < 4; i++) {
            int nr = curr.first + dr[i];
            int nc = curr.second + dc[i];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !visited[nr][nc] && !walls[nr][nc]) {
                visited[nr][nc] = true;
                parent[nr][nc]  = curr;
                q.push({nr, nc});
                if (make_pair(nr, nc) != end) {
                    currentGrid[nr][nc] = FRONTIER;
                    steps.push_back({currentGrid});
                }
            }
        }
    }

    // Trace back from end → start using parent[] and animate each PATH cell
    if (found) {
        pair<int,int> curr = parent[end.first][end.second];
        vector<pair<int,int>> path;
        while (curr != start && curr.first != -1) {
            path.push_back(curr);
            curr = parent[curr.first][curr.second];
        }
        reverse(path.begin(), path.end());
        for (auto p : path) {
            currentGrid[p.first][p.second] = PATH;
            steps.push_back({currentGrid});
        }
    }

    return steps;
}

#endif // PATHFINDING_H
