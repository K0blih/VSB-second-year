#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>

/**
 * @brief Reads edges from a file.
 * @param filename Path to the file
 * @return List of edges
 */
std::vector<std::vector<int>> readEdgesFromFile(const std::string &filename);

/**
 * @brief Builds adjacency list from edge list.
 * @param edges Vector of edges
 * @param n Output: number of vertices
 * @return Adjacency list
 */
std::vector<std::vector<int>> buildAdjacencyList(const std::vector<std::vector<int>>& edges, int &n);

/**
 * @brief Runs BFS from a start vertex.
 * @param start Start vertex
 * @param adj Adjacency list
 * @return Distances from start to all vertices, unreachable vertices have max int
 */
std::vector<int> bfs(int start, const std::vector<std::vector<int>> &adj);

/**
 * @brief Finds the farthest vertex from a start vertex and its distance.
 * 
 * This function assumes the graph is connected
 * 
 * @param start Start vertex
 * @param adj Adjacency list
 * @return Pair of (farthest vertex, distance from start)
 */
std::pair<int, int> farthestVertex(int start, const std::vector<std::vector<int>> &adj);

/**
 * @brief Finds the graph center and its eccentricity.
 * 
 * This function:
 * 1. Estimates diameter endpoints using double-sweep BFS.
 * 2. Computes a lower bound for eccentricity using distances from endpoints.
 * 3. Selects candidate vertices that may be centers.
 * 4. Computes exact eccentricity only for candidate vertices to determine the center(s).
 * 
 * @param adj Adjacency list
 * @return Pair of (vector of center vertices, minimum eccentricity)
 */
std::pair<std::vector<int>, int> findGraphCenter(const std::vector<std::vector<int>> &adj);

/**
 * @brief Checks if the graph is connected.
 * @param adj Adjacency list
 * @return true if connected, false otherwise
 */
bool isConnected(const std::vector<std::vector<int>> &adj);

/**
 * @brief Extracts file name from a full path.
 * @param path Full path
 * @return File name
 */
std::string extractFileName(const std::string &path);

#endif // GRAPH_H