#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>

void countingSortByDigit(std::vector<int>& data, int exp, int base = 10) {
    std::vector<int> output(data.size());
    std::vector<int> count(base, 0);

    for (int num : data) {
        int digit = (num / exp) % base;
        count[digit]++;
    }

    for (int i = 1; i < base; i++) {
        count[i] += count[i - 1];
    }

    for (int i = data.size() - 1; i >= 0; i--) {
        int digit = (data[i] / exp) % base;
        output[count[digit] - 1] = data[i];
        count[digit]--;
    }

    data = output;
}

void radixSort(std::vector<int>& data, int base = 10) {
    if (data.empty()) {
        return;
    }

    int maxVal = *std::max_element(data.begin(), data.end());

    for (int exp = 1; maxVal / exp > 0; exp *= base) {
        countingSortByDigit(data, exp, base);
    }
}

// reads integers from the first line in the file
std::vector<int> readIntegersFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<int> numbersVec;

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return numbersVec;
    }

    std::string line;

    // change if to while to read the whole file
    if (std::getline(file, line)) {
        std::stringstream lineAsStream(line);
        int num;
        while (lineAsStream >> num) {  // NOTE: this assumes that the input file has the data we want
            numbersVec.push_back(num);
        }
    }

    file.close();
    return numbersVec;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Exactly 1 argument expected.\n";
    }

    std::vector<int> data = readIntegersFromFile(argv[1]);
    radixSort(data);

    for (size_t i = 0; i < data.size(); i++) {
        std::cout << data[i] << " ";
    }
    std::cout << "\n";

    return 0;
}