#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

void printVector(const std::vector<int>& vec)
{
    for (const int item : vec) {
        std::cout << item << "\n";
    }
}

bool containsOnlyUniqueValues(const std::vector<int>& vec)
{
    for (int i = 0; i < vec.size() - 1; i ++) {
        if (vec.at(i) == vec.at(i + 1)) {
            return false;
        }
    }
    return true;
}

int ModusFromSorted(const std::vector<int>& vec)
{
    int modus, runLength, runValue;
    int i = 0;
    int freq = 0;

    while (i < vec.size()) {
        runValue = vec.at(i);
        runLength = 1;
        
        while (i + runLength < vec.size() && vec.at(i + runLength) == runValue) {
            runLength += 1;
        }


        if (runLength > freq) {
            freq = runLength;
            modus = runValue;
        }

        i += runLength;
    }

    return modus;
}

int ModusByDict(const std::vector<int>& vec)
{
    std::map<int, int> freqMap;

    for (const int item : vec) {
        freqMap[item] += 1;
    }

    int modus;
    int modusFreq = 0;
    for (const auto& [number, freq] : freqMap) {
        if (freq > modusFreq) {
            modus = number;
            modusFreq = freq;
        }
    }

    return modus;
}

int main ()
{

    std::vector<int> data = {1,2,3,4,5,6,7,8,9,9,9,4,5,6};
    // std::sort(data.begin(), data.end());
    // printVector(data);

    int modus = ModusByDict(data);

    int modus2 = ModusFromSorted(data);

    std::cout << modus << " " << modus2 << "\n";
    return 0;
}