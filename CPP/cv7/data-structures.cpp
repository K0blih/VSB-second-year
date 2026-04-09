#include <iostream>
#include <array>
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

using std::cout;
using std::endl;

int main() {
    // Sequence containers (can be accessed sequentially)
    std::array<int, 3> arr; // linear array with static size (wrapper over int arr[3])
    std::vector<int> vec;   // linear array with dynamic size (optimized for adding at the end)
    std::deque<int> deque;  // double-ended queue (optimized for adding from both sides)
    std::forward_list<int> flist; // singly-linked list
    std::list<int> list;        // doubly-linked list (optimized for modifying in the middle of the container)

    // Specialized wrappers over sequence containers
    std::stack<int> stack; // LIFO queue
    std::queue<int> queue; // FIFO queue
    std::priority_queue<int> priority_queue;    // Priority queue (heap). Queue that sorts its elements.

    // Associative containers (provide fast lookup) - trees, hash maps
    std::set<int> set;                      // ordered set of items
    std::unordered_set<int> unordered_set;  // unordered set of items
    std::map<int, bool> map;                // ordered key -> value mapping
    std::unordered_map<int, bool> unordered_map;  // unordered key -> value mapping (hash map)

    // There is also multiset, multimap, unordered_multiset, unordered_multimap -> multiple keys/values allowed

    // Tuples (fixed-size ordered list of values of potentially different types)
    auto tuple = std::make_tuple(1, -1);
    auto a = std::get<0>(tuple);    // get first element
    auto b = std::get<1>(tuple);    // get second element tuple.1

    auto [c, d] = tuple;            // destructure tuple into individual elements

    // Initialization from initializer list
    std::tuple<int, bool> tuple2 = {1, true};

    // If the tuple has exactly two elements, you can use std::pair
    std::pair<int, bool> pair{1, true};
    auto first = pair.first;
    auto second = pair.second;
    // or auto [first, second] = pair.second;

    auto pair2 = std::make_pair(first, second);

    return 0;
}
