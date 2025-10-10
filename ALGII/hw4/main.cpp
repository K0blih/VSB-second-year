#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

class Heap
{
    private:
    std::vector<int> data;
    size_t n;

    size_t parent(const size_t index) {
        if (index == 0) { return 0; }
        return (index -1) / 2;
    }

    size_t child(const size_t index, const size_t k) {
        return n * index + k + 1;
    }

    void heapify(size_t index) {
        size_t largest = index;

        for (size_t k = 0; k < n; ++k) {
            size_t c = child(index, k);
            if (c < data.size() && data[c] > data[largest]) {
                largest = c;
            }
        }

        if (index != largest) {
            std::swap(data[index], data[largest]);
            heapify(largest);
        }
    }   

    void makeHeap() {
        for (int i = data.size() / n; i >= 0; i--) {
            heapify(i);
        }
    }

    public:
    Heap() = default;

    Heap(std::vector<int>&& input, size_t size) {
        data = std::move(input);
        n = size;
        makeHeap();
    }

    void insert(const int key) {
        data.push_back(key);
        size_t currentIndex = data.size();
        size_t parentIndex = parent(currentIndex);

        while (currentIndex != 0 && data[currentIndex] > data[parentIndex]) {
            std::swap(data[currentIndex], data[parentIndex]);
            currentIndex = parentIndex;
            parentIndex = parent(currentIndex);
        }
    }

    int getMax() {
        if (data.empty()) {
            throw std::out_of_range("Trying to extract item from empty heap");
        }

        int maximum = data[0];

        data[0] = data.back();
        data.pop_back();
        
        if (!data.empty()) {
            heapify(0);
        }

        return maximum;
    }

    void print() {
        for (const int item : data) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }

    void clear() {
        data.clear();
    }
};

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

int main(__attribute_maybe_unused__ int argc, __attribute_maybe_unused__ char** argv)
{
    if (argc < 3) {
        std::cerr << "Not enough arguments\n";
        return 1;
    }

    std::stringstream stream(argv[1]);
    size_t n;
    stream >> n;
    std::vector<int> data = readIntegersFromFile(argv[2]);

    Heap heap(std::move(data), n);
    heap.print(); // 1
    heap.getMax();
    heap.print(); // 2
    heap.insert(42);
    heap.print(); // 3
    heap.insert(-5);
    heap.print(); // 4
    heap.getMax();
    heap.print(); // 5
    
    return 0;
}