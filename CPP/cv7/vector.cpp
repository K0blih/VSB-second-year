#include <iostream>
#include <vector>
#include <algorithm>

using std::cout;
using std::endl;

struct Foo {
    Foo(int x, int y): x(x), y(y) {}
    Foo(Foo&& other) noexcept : x(other.x), y(other.y) {
        cout << "move" << endl;
    }

    int x;
    int y;
};

int main() {
    // Dynamic ordered sequence of items with random access
    // Contains 3 attributes - pointer to heap data (T*), size (size_t) and capacity (size_t)
    // Capacity - how many items can the vector hold before reallocation
    // Size - how many items does the vector currently hold
    std::vector<int> v1;

    // Initialization
    std::vector<int> v2{1, 2, 3, 4, 5};
    std::vector<int> v3(3, 5); // Same as {5, 5, 5}

    // Add items
    std::vector<Foo> v4;
    v4.push_back(Foo(1, 2)); // uses move if possible
    v4.emplace_back(1, 2);  // constructs object directly in place
    v4.push_back(Foo(3, 4));

    cout << v4.size() << " " << v4.capacity() << endl;
    v4.shrink_to_fit(); // (Maybe) shrink capacity
    cout << v4.size() << " " << v4.capacity() << endl;

    // Preallocation - if you know the number of inserted items beforehand, preallocate
    std::vector<int> v5;
    v5.reserve(60); // Set capacity to (at least) 60
    for (int i = 0; i < 60; i++) {
        v5.push_back(i * 2); // No reallocation should happen
    }

    cout << v5.capacity() << endl;
    // Set size to the specified number. Remove or add new elements using the default constructor as necessary.
    v5.resize(100);
//    v4.resize(10); // Not possible - no default constructor!
    v4.clear(); // Remove all items

    // Access items
    std::vector<int> v6{1, 2, 3, 4};
    auto i1 = v6[0];    // no bounds check
//    auto i2 = v6.at(5); // with bounds check
    v6[0] = 5;

    int* ptr = v6.data(); // get raw data pointer
    ptr[3] = 8;

    // Remove item at second index (all items to its right will be moved one spot to the left in memory)
    v6.erase(v6.begin() + 2);

    // Iterator invalidation
    std::vector<int> v7{1, 2};
    // Pointers into the vector may no longer be safe after any modification to its size/capacity happen.
    for (auto item : v7) {
        cout << item << endl;
//        v7.push_back(8);    // BOOM
    }

    // Vector of polymorphic types
//    std::vector<std::unique_ptr<Polymorphic>> objects;

    // std::vector can be used as a building block for other data structures
    // stack
    std::vector<int> stack;
    stack.push_back(1);
    stack.push_back(2);
    stack.push_back(3);
    stack.pop_back();   // 3
    stack.pop_back();   // 2
    stack.pop_back();   // 1

    // set
    std::vector<int> set;
    set.push_back(1);
    set.push_back(2);
    set.push_back(3);
    set.erase(std::remove(set.begin(), set.end(), 2), set.end()); // erase-find idiom

    // queue, sorted set, etc.

    return 0;
}
