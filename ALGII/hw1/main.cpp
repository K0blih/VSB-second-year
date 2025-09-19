#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

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

char binarySearch(const std::vector<int>& vec, int T)
{
    int L = 0;
    int R = vec.size() - 1;

    if (T < vec.front() || T > vec.back()) {
        return 'F';
    }

    while (L <= R) {
        int m = L + (R - L) / 2;
        if (vec[m] < T) {
            L = m + 1;
        }
        else if (vec[m] > T) {
            R = m - 1;
        }
        else {
            return 'T';
        }
    }

    return 'F';
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Not enough arguments\n";
        return 1;
    }
    std::string data_file = argv[1];
    std::string numbers_file = argv[2];

    auto data = readIntegersFromFile(data_file);
    std::sort(data.begin(), data.end());
    auto numbers = readIntegersFromFile(numbers_file);

    for (const auto& number : numbers) {
        std::cout << number << ": " << binarySearch(data, number) << "\n";
    }

    return 0;
}