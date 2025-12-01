#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct SimplePairHash {
    size_t operator()(const std::pair<int,int> &p) const {
        return p.first + 10000 * p.second; // we expect and promise less than 10000 keys in stringToCode
    }
};

std::string readTextFromFile(const std::string &fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
        std::exit(1);
    }
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return text;
}

std::string escapeSpecialChars(const std::string &s) {
    std::string result;
    for (char c : s) {
        if (c == '\n') {
            result += "\\n";
        } else if (c == '\t') {
            result += "\\t";
        } else {
            result += c;
        }
    }
    return result;
}

std::pair<int,int> findMostFrequentPair(const std::vector<int> &textAsNumbers) {
    std::unordered_map<std::pair<int,int>, int, SimplePairHash> freq;

    for (std::size_t i = 0; i + 1 < textAsNumbers.size(); i++) {
        freq[{textAsNumbers[i], textAsNumbers[i+1]}]++;
    }

    if (freq.empty()) {
        return {-1, -1};
    }

    std::pair<int,int> bestPair;
    int bestCount = -1;
    bool first = true;

    for (const auto &p : freq) {
        const auto &pairVal = p.first;
        int count = p.second;

        if (first || count > bestCount || (count == bestCount && pairVal > bestPair))
        {
            first = false;
            bestPair = pairVal;
            bestCount = count;
        }
    }

    return bestPair;
}

void runBPE(int targetSize, std::unordered_map<std::string,int> &stringToCode,
    std::unordered_map<int,std::string> &codeToString, std::vector<int> &textAsNumbers, int &nextCode) {
    while ((int)stringToCode.size() < targetSize) {

        if (textAsNumbers.size() < 2) {
            break;
        }

        const auto bestPair = findMostFrequentPair(textAsNumbers);
        if (bestPair.first == -1) {
            break;
        }

        const std::string merged = codeToString[bestPair.first] + codeToString[bestPair.second];

        stringToCode[merged] = nextCode;
        codeToString[nextCode] = merged;
        int newCode = nextCode++;
        
        std::vector<int> newText;
        newText.reserve(textAsNumbers.size());

        for (std::size_t i = 0; i < textAsNumbers.size();) {
            if (i + 1 < textAsNumbers.size() && textAsNumbers[i] == bestPair.first && textAsNumbers[i+1] == bestPair.second)
            {
                newText.push_back(newCode);
                i += 2;
            }
            else {
                newText.push_back(textAsNumbers[i]);
                i++;
            }
        }

        textAsNumbers = std::move(newText);
    }
}

void printCodeMap(const std::unordered_map<int,std::string> &codeToString) {
    std::vector<int> keys;
    keys.reserve(codeToString.size());

    for (const auto &p : codeToString) {
        keys.push_back(p.first);
    }

    std::sort(keys.begin(), keys.end());

    for (int k : keys) {
        std::cout << k << " -> '" << escapeSpecialChars(codeToString.at(k)) << "'\n";
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    if (argc != 3) {
        std::cerr << "Exactly 2 arguments expected.\n";
        return 1;
    }

    std::string fileName = argv[1];
    int targetSize = std::stoi(argv[2]);

    std::string text = readTextFromFile(fileName);

    std::unordered_map<std::string,int> stringToCode;
    std::unordered_map<int,std::string> codeToString;
    std::vector<int> textAsNumbers;

    int nextCode = 0;
    for (char c : text) {
        std::string s(1, c);

        if (!stringToCode.count(s)) {
            stringToCode[s] = nextCode;
            codeToString[nextCode] = s;
            nextCode++;
        }

        textAsNumbers.push_back(stringToCode[s]);
    }

    runBPE(targetSize, stringToCode, codeToString, textAsNumbers, nextCode);
    printCodeMap(codeToString);

    return 0;
}
