#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

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
        while (lineAsStream >> num) { 
            numbersVec.push_back(num);
        }
    }

    file.close();
    return numbersVec;
}

int countSubsets(const std::vector<int>& nums, int target, int index, int currentSum) {
    if (currentSum > target) {
        return 0;
    }

    if (index == static_cast<int>(nums.size())) {
        return (currentSum == target) ? 1 : 0;
    }

    // skip element
    int count = countSubsets(nums, target, index + 1, currentSum);

    // include element
    count += countSubsets(nums, target, index + 1, currentSum + nums[index]);

    return count;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    if (argc != 3) {
        std::cerr << "Expected exactly 2 arguments\n";
        return 1;
    }

    std::string filename = argv[1];
    int target = std::stoi(argv[2]);

    std::vector<int> numbers = readIntegersFromFile(filename);

    int result = countSubsets(numbers, target, 0, 0);
    std::cout << result << "\n";

    return 0;
}
