#include<set>
#include<map>
#include<vector>
#include<queue>
#include<string>
#include<iostream>

using std::set, std::vector;

class State {
    private:
    std::map<std::string, bool> positions;

    bool isValid(std::map<std::string, bool> situation) const {
        return (situation["P"] == situation["K"]) || (situation["K"] ^ situation["V"] && situation["K"] ^ situation["Z"]);
    }

    public:
    State() {
        // na zacatku jsou vsichni vlevo
        positions["K"] = false;
        positions["V"] = false;
        positions["Z"] = false;
        positions["P"] = false;
    };

    State(std::map<std::string, bool>& inPositions) : positions(inPositions) {};

    vector<State> generateChildren() const {
        vector<State> res;
        
        // P jede sam
        auto proposedPosition = positions;
        proposedPosition["P"] = !proposedPosition["P"];
        if ( isValid(proposedPosition) ) {
            res.emplace_back(proposedPosition);
        }

        // P jede s nekym
        for (const auto& [item, value] : positions) {
            if (item == "P" || positions.at("P") != value) {
                continue;
            }
            
            auto proposedPosition = positions;
            proposedPosition["P"] = !proposedPosition["P"];
            proposedPosition[item] = !proposedPosition[item];
            if ( isValid(proposedPosition) ) {
                res.emplace_back(proposedPosition);
            }
        }
        return res;
    }

    bool isFinal() const {
        for (const auto& [name, position] : positions) {
            if(!position) { return false; }
        }
        return true;
    }

    bool operator==(const State& other) const {
        return positions == other.positions;
    }

    bool operator<(const State& other) const {
        return positions < other.positions;
    }

    void print() const {
        std::string left, right;
        for (const auto& [key, value] : positions) {
            if (value) {
                left += key;
            }
            else {
                right += key;
            }
        }
        std::cout << left << " " << right << "\n";
    }
};


vector<State> solveKVZ(){
    State start;

    std::queue<State> bfsQ;
    bfsQ.push(start);

    set<State> visited;
    std::map<State, State> child2parent;
    
    visited.insert(start);
    child2parent[start] = start;
    vector<State> pathToVictory;
    bool solutionFound = false;
    
    while (!bfsQ.empty() && !solutionFound) {
        int n = bfsQ.size();
        for (int i = 0; i < n && !solutionFound; i++) {
            State current = bfsQ.front();
            bfsQ.pop();

            const auto children = current.generateChildren();
            for (const auto& child : children) {
                if (visited.find(child) == visited.end()) {
                    child2parent[child] = current;
                    bfsQ.push(child);
                    visited.insert(child);
                }
                if (child.isFinal()) {
                    solutionFound = true;
                    State parent = child2parent.at(child);
                    State tmp = child;
                    pathToVictory.push_back(child);

                    while (!(tmp == parent) ) {
                        pathToVictory.push_back(parent);
                        tmp = parent;
                        parent = child2parent.at(tmp);
                    }
                    break;
                }
            }
        }
    }
    return pathToVictory;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    const auto res = solveKVZ();

    for (const auto& line : res) {
        line.print();
    }

    return 0;
}