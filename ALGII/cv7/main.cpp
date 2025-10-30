#include <iostream>
#include <vector>
#include <algorithm>

void distributionCountingSort(std::vector<int>& data) {
    if (data.size() < 2) {
        return;
    }
    std::vector<int> sortedOutput;

    const auto [minP, maxP] = std::minmax_element(data.begin(), data.end());
    int min, max;
    min = *minP; max = *maxP;

    int range = max - min + 1;
    std::vector<int> frequencies(range, 0);

    for (int number : data) {
        frequencies[number - min] += 1;
    }

    for (int i = 1; i < range; i++) {
        frequencies[i] += frequencies[i - 1];
    }

    sortedOutput.resize(data.size(), 0);
    for (int i = data.size() - 1; i >= 0; i--) {
        int current = data.at(i);
        int index = current - min;
        int indexInSorted = frequencies[index] - 1;
        sortedOutput[indexInSorted] = current;
        frequencies[index] -= 1;
    }
    data = sortedOutput;
}

void test(){
    std::vector<int> data = {64, 33, 50, 25, 76, 38, 2, 15, 36, 6, 22, 43, 54, 29, 1, 7, 75, 80, 0, 6};
    auto data2 = data;
    distributionCountingSort(data);
    std::sort(data2.begin(), data2.end());

    for (size_t i = 0; i < data.size(); i++) {
        if (data[i] != data2[i]) {
            std::cout << "nay!\n";
            return;
        }
    }

    std::cout << "yay!\n";
    return;
}

int main() {
    test();
    return 0;
}