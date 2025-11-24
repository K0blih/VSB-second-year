#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <fstream>

using label = int;

class UnionFindStructure {
    private:
    std::unordered_map<label, label> parent; // element -> parent/representant of element
    std::unordered_map<label, size_t> sizes; // element -> size of its partition if element is representant, rubish otherwise

    public:
    UnionFindStructure(const std::vector<label>& universe) {
        for (const auto& el : universe) {
            parent[el] = el;
            sizes[el] = 1;
        }
    }

    UnionFindStructure(const int n) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
            sizes[i] = 1;
        }
    }

    label findRepresentative(label x) {
        if (parent[x] != x) {
            parent[x] = findRepresentative(parent[x]); // path compression
        }
        return parent[x];
    }

    void setUnion(label x, label y) {
        label rootX = findRepresentative(x);
        label rootY = findRepresentative(y);

        if (rootX == rootY) {
            return;
        }

        if (sizes[rootX] < sizes[rootY]) {
            std::swap(rootX, rootY);
        }

        parent[rootY] = rootX;
        sizes[rootX] += sizes[rootY];
    }

    bool inTheSameSubset(label x, label y) {
        return findRepresentative(x) == findRepresentative(y);
    }
};

// weighted edges, possible simple representations, std::sort compatible
using Edge = std::pair<label, label>; // from, to
using WeightedEdge1 = std::tuple<int, label, label>; // weight, from, to
using WeightedEdge2 = std::pair<int, Edge>; // weight, (from, to)

struct WeightedEdge3 {
    int weight;
    label from;
    label to;

    // comparison operator for std::sort, does not work with std::set or std::map
    bool operator<(const WeightedEdge3& other) const {
        return weight < other.weight;
    }
};

struct WeightedEdge4 {
    int weight;
    label from;
    label to;

    WeightedEdge4(int w, label f, label t) {
        weight = w;
        if (f > t) {
            std::swap(f, t);
        }
        from = f;
        to = t;
    }

    // comparison operator that will work with std::set or std::map in expected way
    bool operator<(const WeightedEdge4& other) const {
        if (weight != other.weight) {
            return weight < other.weight;
        }
        if (from != other.from) {
            return from < other.from;
        }
        return to < other.to;
    }
};

std::vector<std::vector<int>> inputMatrix(const std::string& filename, int& n) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> matrix;

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << "\n";
        return matrix;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::vector<int> row;
        int x;

        while (ss >> x) {
            row.push_back(x);
        }

        if (!row.empty()) {
            matrix.push_back(row);
        }
    }

    n = matrix.size();

    return matrix;
}

int kruskalMST(int n, std::vector<WeightedEdge3>& edges, std::vector<WeightedEdge3>& MST_edges) {
    std::sort(edges.begin(), edges.end());

    UnionFindStructure uf(n);

    int edgesUsed = 0;
    int totalWeight = 0;

    for (auto& e : edges) {
        if (edgesUsed == n - 1) {
            break;
        }
            
        if (!uf.inTheSameSubset(e.from, e.to)) {
            uf.setUnion(e.from, e.to);
            edgesUsed++;
            totalWeight += e.weight;

            MST_edges.push_back(e);
        }
    }

    return totalWeight;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    if (argc != 2) {
        std::cerr << "Exactly 1 argument expected.\n";
        return 1;
    }

    int n;
    std::vector<std::vector<int>> matrix = inputMatrix(argv[1], n);

    std::vector<WeightedEdge3> edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int w = matrix[i][j];
            if (w != 0) {
                edges.push_back({w, i, j});
            }
        }
    } 

    std::vector<WeightedEdge3> MST_edges;
    std::cout << kruskalMST(n, edges, MST_edges) << "\n";
}