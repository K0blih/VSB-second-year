#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <algorithm>

class State {
private:
    int n_pairs;
    
    std::vector<bool> positions;

    // 0=H0, 1=W0, 2=H1, 3=W1...
    bool isBoatValid(int person_idx_1, int person_idx_2) const {
        int p1_id = person_idx_1 - 1;
        int p2_id = person_idx_2 - 1;

        int p1_pair_idx = p1_id / 2;
        bool p1_is_wife = (p1_id % 2) == 1;
        int p2_pair_idx = p2_id / 2;
        bool p2_is_wife = (p2_id % 2) == 1;

        // manzel H_i (p1) s cizi zenou W_j (p2)
        if (!p1_is_wife && p2_is_wife && p1_pair_idx != p2_pair_idx) return false;
        
        // manzelka W_i (p1) s cizim muzem H_j (p2)
        if (p1_is_wife && !p2_is_wife && p1_pair_idx != p2_pair_idx) return false;

        //(H+H, W+W, H_i+W_i)
        return true; 
    }

    bool isValid() const {
        for (int i = 0; i < n_pairs; i++) {
            bool husbandPos = positions[2 * i + 1]; // H_i
            bool wifePos = positions[2 * i + 2];    // W_i
            if (husbandPos == wifePos) continue;

            bool dangerBank = wifePos; 
            for (int j = 0; j < n_pairs; j++) {
                if (i == j) continue;
                
                bool otherHusbandPos = positions[2 * j + 1]; // H_j
                if (otherHusbandPos == dangerBank) {
                    bool otherWifePos = positions[2 * j + 2]; // W_j
                    if (otherWifePos != dangerBank) {
                        return false; 
                    }
                }
            }
        }

        return true;
    }

public:
    State(int n) : n_pairs(n) {
        positions.resize(2 * n + 1, false);
    }

    State(int n, const std::vector<bool>& newPositions) : n_pairs(n), positions(newPositions) {}

    bool getBoatPos() const {
        return positions[0];
    }

    std::vector<State> generateChildren() const {
        std::vector<State> res;
        bool currentBank = getBoatPos();
        bool targetBank = !currentBank;

        for (int i = 1; i <= 2 * n_pairs; i++) {
            if (positions[i] == currentBank) {
                auto proposedPositions = positions;
                proposedPositions[0] = targetBank;
                proposedPositions[i] = targetBank;
                
                State child(n_pairs, proposedPositions);
                if (child.isValid()) {
                    res.push_back(child);
                }
            }
        }

        for (int i = 1; i <= 2 * n_pairs; i++) {
            if (positions[i] != currentBank) continue;

            for (int j = i + 1; j <= 2 * n_pairs; ++j) {
                if (positions[j] != currentBank) continue;

                if (isBoatValid(i, j)) {
                    auto proposedPositions = positions;
                    proposedPositions[0] = targetBank;
                    proposedPositions[i] = targetBank;
                    proposedPositions[j] = targetBank;

                    State child(n_pairs, proposedPositions);
                    if (child.isValid()) {
                        res.push_back(child);
                    }
                }
            }
        }
        return res;
    }

    bool isFinal() const {
        for (bool pos : positions) {
            if (!pos) return false;
        }
        return true;
    }
    
    bool operator==(const State& other) const {
        return positions == other.positions;
    }

    bool operator<(const State& other) const {
        return positions < other.positions;
    }
};

int solve(int n) {
    State start(n);
    std::queue<State> bfsQ;
    bfsQ.push(start);

    std::set<State> visited;
    visited.insert(start);

    std::map<State, int> distance;
    distance[start] = 0;

    while (!bfsQ.empty()) {
        State current = bfsQ.front();
        bfsQ.pop();

        int current_dist = distance[current];

        if (current.isFinal()) {
            return current_dist;
        }

        const auto children = current.generateChildren();
        for (const auto& child : children) {
            if (visited.find(child) == visited.end()) { 
                visited.insert(child);
                distance[child] = current_dist + 1;
                bfsQ.push(child);
            }
        }
    }

    return -1;
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    std::cout << "2 pairs - " << solve(2) << " transports" << "\n";
    std::cout << "3 pairs - " << solve(3) << " transports" << "\n";
    std::cout << "4 pairs - " << solve(4) << " transports" << "\n";
    
    return 0;
}