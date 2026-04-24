#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <queue>

using namespace std;

struct GraphNode {
    int id; int value; int leftId; int rightId; float x; float y;
};

struct GraphStep {
    int currentId;
    vector<int> visitedSequence; 
    vector<int> activeStack;     
    bool done;
};

inline void layoutGraph(vector<GraphNode>& nodes, int rootId, int depth) {
    if (nodes.empty() || rootId == -1) return;
    struct LayoutEngine {
        vector<GraphNode>& tNodes; int dCounter;
        void inOrderLayout(int nodeIdx, int currentDepth, int maxDepth) {
            if (nodeIdx == -1) return;
            inOrderLayout(tNodes[nodeIdx].leftId, currentDepth + 1, maxDepth);
            tNodes[nodeIdx].x = dCounter; 
            tNodes[nodeIdx].y = (float)currentDepth / max(1, maxDepth);
            dCounter++;
            inOrderLayout(tNodes[nodeIdx].rightId, currentDepth + 1, maxDepth);
        }
    };
    LayoutEngine engine = {nodes, 0};
    engine.inOrderLayout(rootId, 0, depth);
    
    int totalNodes = engine.dCounter;
    for(size_t i = 0; i < nodes.size(); i++) {
        nodes[i].x = totalNodes > 1 ? (nodes[i].x + 0.5f) / totalNodes : 0.5f;
        nodes[i].y = 0.1f + nodes[i].y * 0.8f; 
    }
}

inline vector<GraphNode> generateGraph(int maxDepth) {
    vector<GraphNode> nodes;
    if (maxDepth < 0) return nodes;

    GraphNode root = {0, (rand() % 99) + 1, -1, -1, 0, 0};
    nodes.push_back(root);

    struct NodeTask { int id; int depth; };
    queue<NodeTask> q; q.push({0, 0});

    int nextId = 1;
    while (!q.empty()) {
        NodeTask curr = q.front(); q.pop();
        if (curr.depth < maxDepth) {
            if ((rand() % 100) < 85 || curr.depth < 2) { 
                GraphNode leftNode = {nextId++, (rand() % 99) + 1, -1, -1, 0, 0};
                nodes.push_back(leftNode); nodes[curr.id].leftId = leftNode.id;
                q.push({leftNode.id, curr.depth + 1});
            }
            if ((rand() % 100) < 85 || curr.depth < 2) {
                GraphNode rightNode = {nextId++, (rand() % 99) + 1, -1, -1, 0, 0};
                nodes.push_back(rightNode); nodes[curr.id].rightId = rightNode.id;
                q.push({rightNode.id, curr.depth + 1});
            }
        }
    }
    layoutGraph(nodes, 0, maxDepth);
    return nodes;
}
