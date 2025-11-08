#include <iostream>
#include <vector>
#include <optional>
#include <string>

class CuckooHashTable {
private:
    std::vector<std::optional<std::string>> T1;
    std::vector<std::optional<std::string>> T2;
    size_t numberOfItems;
    double maxOccupancy;
    double resizeFactor;
    size_t maxLoops;

    size_t h1(const std::string& key) const {
        return std::hash<std::string>{}(key) % T1.size();
    }

    size_t h2(const std::string& key) const {
        return std::hash<std::string>{}(key) % T2.size();
    }

    bool needsResize() const {
        double load = numberOfItems / static_cast<double>(T1.size() + T2.size());
        return load > maxOccupancy;
    }

    void resize() {
        size_t newSize1 = static_cast<size_t>(T1.size() * resizeFactor);
        size_t newSize2 = static_cast<size_t>(T2.size() * resizeFactor);

        std::vector<std::optional<std::string>> oldT1 = std::move(T1);
        std::vector<std::optional<std::string>> oldT2 = std::move(T2);

        T1.assign(newSize1, std::nullopt);
        T2.assign(newSize2, std::nullopt);
        numberOfItems = 0;

        for (const auto& item : oldT1) {
            if (item) {
                insert(*item);
            }
        }
        for (const auto& item : oldT2) {
            if (item) {
                insert(*item);
            }
        }
    }

public:
    CuckooHashTable(size_t size1, size_t size2, double maxOcc, double resizeF, size_t maxL)
        : T1(size1), T2(size2), numberOfItems(0),
          maxOccupancy(maxOcc), resizeFactor(resizeF), maxLoops(maxL) {}

    bool lookup(const std::string& key) const {
        size_t i1 = h1(key);
        size_t i2 = h2(key);
        return (T1[i1] && *T1[i1] == key) || (T2[i2] && *T2[i2] == key);
    }

    void insert(const std::string& key) {
        if (lookup(key))
        {
            return;
        }

        if (needsResize()) {
            resize();
        }

        std::string x = key;
        for (size_t i = 0; i < maxLoops; ++i) {
            size_t i1 = h1(x);
            if (!T1[i1]) {
                T1[i1] = x;
                numberOfItems++;
                return;
            }
            std::swap(x, *T1[i1]);

            size_t i2 = h2(x);
            if (!T2[i2]) {
                T2[i2] = x;
                numberOfItems++;
                return;
            }
            std::swap(x, *T2[i2]);
        }

        resize();
        insert(x);
    }

    bool remove(const std::string& key) {
        size_t i1 = h1(key);
        size_t i2 = h2(key);
        if (T1[i1] && *T1[i1] == key) {
            T1[i1].reset();
            numberOfItems--;
            return true;
        }
        if (T2[i2] && *T2[i2] == key) {
            T2[i2].reset();
            numberOfItems--;
            return true;
        }
        return false;
    }

    void print() const {
        std::cout << "Table1:\n";
        for (size_t i = 0; i < T1.size(); ++i) {
            if (T1[i])
                std::cout << i << ": " << *T1[i] << " ";
        }
        std::cout << "\nTable2:\n";
        for (size_t i = 0; i < T2.size(); ++i) {
            if (T2[i])
                std::cout << i << ": " << *T2[i] << " ";
        }
        std::cout << "\n\n";
    }
};

int main() {
    CuckooHashTable cuckoo(10, 11, 0.5, 1.5, 5);

    const std::vector<std::string> inputs = {
        "zero", "one", "two", "three", "four", "five", "six",
        "seven", "eight", "nine"
    };

    for (const auto& s : inputs) cuckoo.insert(s);
    cuckoo.print();

    const std::vector<std::string> inputs2 = {
        "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen",
        "seventeen", "eighteen", "nineteen", "twenty",
        "twenty-one", "twenty-two", "twenty-three", "twenty-four", "twenty-five",
        "twenty-six", "twenty-seven", "twenty-eight", "twenty-nine"
    };

    for (const auto& s : inputs2) cuckoo.insert(s);
    cuckoo.print();

    const std::vector<std::string> removals = {
        "twenty-one", "twenty-two", "twenty-three", "twenty-four", "twenty-five",
        "twenty-six", "twenty-seven", "twenty-eight", "twenty-nine",
        "aardvark", "Bene Gesserit", "crane", "dog", "element"
    };

    for (const auto& s : removals) cuckoo.remove(s);
    cuckoo.print();

    return 0;
}
