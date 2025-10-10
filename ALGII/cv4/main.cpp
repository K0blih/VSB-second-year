#include <iostream>
#include <vector>

class Heap
{
    private:
    std::vector<int> data;

    size_t parent(const size_t index) {
        if (index == 0) { return 0; }
        return (index -1) / 2;
    }

    void heapify(size_t index) {
        size_t children[2] = {2*index + 1, 2*index + 2};
        size_t largest = index;

        for (const size_t child : children) {
            if (child >= data.size()) { break; }
            if (data[child] > data[largest]) {
                largest = child;
            }
        }

        if (index != largest) {
            std::swap(data[index], data[largest]);
            heapify(largest);
        }
    }

    void makeHeap() {
        for (int i = data.size() / 2; i >= 0; i --) {
            heapify(i);
        }
    }

    public:
    Heap() = default;

    Heap(std::vector<int>&& input) {
        data = std::move(input);
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
        
        if (data.size() > 1) {
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

void test(){
    std::vector<int> data = {10, 11, 1, 2, 3, 100, 200};
    Heap heap;
    for(const int item : data){
        heap.insert(item);
    }
    Heap heap2(std::move(data));
    std::cout << "Note that these constructions do not lead to same internal vector.\n";
    heap2.print();
    heap.print();

}

int main(__attribute_maybe_unused__ int argc, __attribute_maybe_unused__ char** argv)
{
    std::vector<int> data = {4, 5, 6, 7, 8, 2, 1, 0};

    Heap heap(std::move(data));
    heap.print();
    heap.insert(-1);
    heap.getMax();
    heap.print();
    
    return 0;
}