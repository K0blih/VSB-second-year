#include "Graph.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include <iostream>
#include <algorithm>

std::vector<std::vector<int>> readEdgesFromFile(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> edges;
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return edges;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int u, v;
        if (ss >> u >> v) {
            edges.push_back({u, v});
        }
    }

    return edges;
}

std::vector<std::vector<int>> buildAdjacencyList(const std::vector<std::vector<int>>& edges, int &n) {
    int maxNode = 0;
    for (auto &e : edges) {
        maxNode = std::max({maxNode, e[0], e[1]});
    }
    n = maxNode + 1;

    std::vector<std::vector<int>> adj(n);
    for (auto &e : edges) {
        adj[e[0]].push_back(e[1]);
        adj[e[1]].push_back(e[0]);
    }

    return adj;
}

std::vector<int> bfs(int start, const std::vector<std::vector<int>> &adj) {
    std::vector<int> dist(adj.size(), std::numeric_limits<int>::max());
    std::queue<int> q;
    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : adj[u]) {
            if (dist[v] == std::numeric_limits<int>::max()) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }

    return dist;
}

std::pair<int, int> farthestVertex(int start, const std::vector<std::vector<int>> &adj) {
    std::vector<int> dist = bfs(start, adj);
    int maxDist = -1;
    int vertex = start;
    for (int i = 0; i < static_cast<int>(dist.size()); i++) {
        if (dist[i] != std::numeric_limits<int>::max() && dist[i] > maxDist) {
            maxDist = dist[i];
            vertex = i;
        }
    }

    return {vertex, maxDist};
}

std::pair<std::vector<int>, int> findGraphCenter(const std::vector<std::vector<int>> &adj) {
    // BFS from arbitrary vertex to estimate diameter
    auto [x, ignore1] = farthestVertex(0, adj);
    auto [y, ignore2] = farthestVertex(x, adj);

    // BFS from both endpoints
    std::vector<int> distX = bfs(x, adj);
    std::vector<int> distY = bfs(y, adj);

    // Compute lower bound for eccentricity
    int minEccLowerBound = std::numeric_limits<int>::max();
    for (int i = 0; i < static_cast<int>(adj.size()); i++) {
        int eccLB = std::max(distX[i], distY[i]);
        if (eccLB < minEccLowerBound) {
            minEccLowerBound = eccLB;
        }
    }

    // Candidate vertices
    std::vector<int> candidates;
    for (int i = 0; i < static_cast<int>(adj.size()); i++) {
        if (std::max(distX[i], distY[i]) == minEccLowerBound) {
            candidates.push_back(i);
        }
    }

    // Compute exact eccentricity only for candidates
    std::vector<int> centers;
    int exactMinEcc = std::numeric_limits<int>::max();
    for (int v : candidates) {
        std::vector<int> distV = bfs(v, adj);
        int ecc = *std::max_element(distV.begin(), distV.end());
        if (ecc < exactMinEcc) {
            exactMinEcc = ecc;
            centers.clear();
            centers.push_back(v);
        } else if (ecc == exactMinEcc) {
            centers.push_back(v);
        }
    }

    return {centers, exactMinEcc};
}

bool isConnected(const std::vector<std::vector<int>> &adj) {
    std::vector<int> dist = bfs(0, adj);
    for (int d : dist) {
        if (d == std::numeric_limits<int>::max()) {
            return false;
        }
    }

    return true;
}

std::string extractFileName(const std::string &path) {
    size_t pos = path.find_last_of("/\\");

    return (pos == std::string::npos) ? path : path.substr(pos + 1);
}