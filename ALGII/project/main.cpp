#include "Graph.h"
#include <iostream>

/**
 * @brief Entry point of the program.
 *
 * Reads a graph from a file, builds its adjacency list, checks connectivity,
 * computes the graph center and prints results.
 *
 * @param argc Argument count
 * @param argv Argument vector
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Exactly 1 argument expected.\n";
        return 1;
    }

    std::string filename = argv[1];
    std::vector<std::vector<int>> edges = readEdgesFromFile(filename);
    
    int n;
    std::vector<std::vector<int>> adj = buildAdjacencyList(edges, n);
    auto [centers, centerEcc] = findGraphCenter(adj);

    std::cout << extractFileName(filename) << "\n";
    std::cout << "Number of vertices: " << n << "\n";
    std::cout << "Number of edges: " << edges.size() << "\n";
    std::cout << "Is connected: " << (isConnected(adj) ? "True" : "False") << "\n";

    std::cout << "Graph center: ";
    for (int c : centers) {
        std::cout << c << " ";
    }
    std::cout << "\n";

    std::cout << "Graph center eccentricity: " << centerEcc << "\n";

    return 0;
}