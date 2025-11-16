#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

std::vector<std::vector<int>> readBoardFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << "\n";
    }

    std::vector<std::vector<int>> board;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<int> row;
        int val;
        while (ss >> val) {
            row.push_back(val);
        }
        if (!row.empty()) {
            board.push_back(row);
        }
    }

    return board;
}

int robotCoinCollection(const std::vector<std::vector<int>> &board) {
    if (board.empty()) {
        return 0;
    }

    int n = board.size();
    int m = board[0].size();

    std::vector<std::vector<int>> F(n, std::vector<int>(m, 0));

    F[0][0] = board[0][0];

    for (int j = 1; j < m; j++) {
        F[0][j] = F[0][j-1] + board[0][j];
    }
        
    for (int i = 1; i < n; i++) {
        F[i][0] = F[i-1][0] + board[i][0];
    }

    for (int i = 1; i < n; i++) {
        for (int j = 1; j < m; j++) {
            F[i][j] = std::max(F[i-1][j], F[i][j-1]) + board[i][j];
        }
    }

    return F[n-1][m-1];
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    if (argc != 2) {
        std::cerr << "Exactly 1 argument expected.\n";
        return 1;
    }

    std::vector<std::vector<int>> board = readBoardFromFile(argv[1]);
    int maxCoins = robotCoinCollection(board);
    std::cout << maxCoins << "\n";

    return 0;
}