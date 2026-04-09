#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <random>
#include <functional>

using std::cout;
using std::endl;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& items) {
    for (size_t i = 0; i < items.size(); i++) {
        cout << items[i];
        if (i != items.size() - 1) {
            os << ", ";
        }
    }
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& items) {
    int i = 0;
    for (auto it = items.begin(); it != items.end(); it++, i++) {
        cout << *it;
        if (i != items.size() - 1) {
            os << ", ";
        }
    }
    return os;
}

int main() {
    // Algorithms usually works with containers
    // begin() and end() provide a unifying interface for algorithms
    // The following algorithms work on vectors, lists, stacks, queues, maps etc.!
    // Will be useful later in templated code
    // Algorithms usually do not change the size of the iterated collection

    std::vector<int> v1{ 1, 2, 3, 4 };
    cout << std::accumulate(v1.begin(), v1.end(), 0) << endl; // sum
    cout << std::accumulate(v1.begin(), v1.end(), 1, [](int a, int b) { return a * b; }) << endl; // product

    cout << *std::max_element(v1.begin(), v1.end()) << endl;

    // algorithms can also work with raw pointers
    int* ptr = v1.data();
    cout << std::accumulate(ptr, ptr + v1.size(), 0) << endl;

    bool contains_even = std::any_of(v1.begin(), v1.end(), [](int x) { return x % 2 == 0; });
    bool all_larger_than_zero = std::all_of(v1.begin(), v1.end(), [](int x) { return x > 0; });

    // find (returns second parameter if nothing was found
    auto it = std::find(v1.begin(), v1.end(), 3);
    auto position = it - v1.begin();
    cout << position << endl;

    // find first even number
    cout << *std::find_if(v1.begin(), v1.end(), [](int x) { return x % 2 == 0; }) << endl;
    // find last even number
    cout << *std::find_if(
        std::make_reverse_iterator(v1.begin()),
        std::make_reverse_iterator(v1.end()),
        [](int x) { return x % 2 == 0; }) << endl;

    std::vector<int> v2{ 1, 2, 3, 4, 5 };
    std::fill(v2.begin() + 1, v2.begin() + 3, 5);
    cout << v2 << endl;

    std::sort(v1.begin(), v1.end()); // sort
    cout << v1 << endl;
    std::sort(v1.begin(), v1.end(), std::greater<int>()); // reverse sort
    cout << v1 << endl;
    // use std::stable_sort for stable sorting

    cout << std::is_sorted(v1.begin(), v1.end(), std::greater<>()) << endl;

    // Filter even values
    std::vector<int> v3;
    std::copy_if(v1.begin(), v1.end(), std::back_inserter(v3), [](int i) { return i % 2 == 0; });
    cout << v3 << endl;

    std::vector<int> v4{ 1, 2, 3, 5, 6 };
    // binary search (expects sorted array)
    cout << std::binary_search(v4.begin(), v4.end(), 5) << endl;

    // find where the element would be placed in a sorted array
    // useful for implementing binary search and other algorithms
    auto it2 = std::lower_bound(v4.begin(), v4.end(), 5);
    v4.insert(it2, 4);
    cout << v4 << endl;

    // functional map
    std::vector<int> v5{ 1, 2, 3, 4, 5 };
    std::transform(v5.begin(), v5.end(), v5.begin(), [](int x) { return x * 2; });
    cout << v5 << endl;

    // reverse
    std::reverse(v5.begin(), v5.end());
    cout << v5 << endl;

    // rotate
    std::rotate(v5.begin(), v5.begin() + 1, v5.end());
    cout << v5 << endl;

    // shuffle
    std::default_random_engine engine;
    std::shuffle(v5.begin(), v5.end(), engine);
    cout << v5 << endl;

    // Remove duplicates
    std::vector<int> v6{ 5, 3, 1, 5, 1, 2, 3, 4, 2, 2 };
    std::sort(v6.begin(), v6.end());
    auto it3 = std::unique(v6.begin(), v6.end());
    v6.erase(it3, v6.end());
    cout << v6 << endl;

    std::vector<int> v7{ -1, 3, -5, 6, 8, -3 };
    std::partition(v7.begin(), v7.end(), [](int x) { return x < 0; });
    cout << v7 << endl;

    std::vector<int> v8{ 1, 2, 3 };
    std::next_permutation(v8.begin(), v8.end());
    cout << v8 << endl;

    // set operations: set_union, set_intersection, set_difference
    // set_union
    {//with vector
        cout << "set operations on vector" << endl;
        std::vector<int> v9{ 1, 2, 3, 4, 5 };
        std::vector<int> v10{ 3, 4, 5, 6, 7 };
        std::vector<int> v_union_of_v9_and_v10{};
        std::set_union(v9.begin(), v9.end(), v10.begin(), v10.end(), std::back_inserter(v_union_of_v9_and_v10));
        cout << "v9               " << v9 << endl;
        cout << "v10                    " << v10 << endl;
        cout << "union            " << v_union_of_v9_and_v10 << endl;

        std::vector<int> v_intersection_of_v9_and_v10{};
        std::set_intersection(v9.begin(), v9.end(), v10.begin(), v10.end(), std::back_inserter(v_intersection_of_v9_and_v10));
        cout << "intersections          " << v_intersection_of_v9_and_v10 << endl;

        std::vector<int> v_difference_of_v9_and_v10{};
        std::set_difference(v9.begin(), v9.end(), v10.begin(), v10.end(), std::back_inserter(v_difference_of_v9_and_v10));
        cout << "difference       " << v_difference_of_v9_and_v10 << endl;
    }

    {//with set
        cout << "set operations on set" << endl;
        std::set<int> s1{ 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5 };
        std::set<int> s2{ 3, 4, 5, 6, 7, 3, 4, 5, 6, 7, 3, 4, 5, 6, 7 };
        std::set<int> s_union_of_s1_and_s2{};
        std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(s_union_of_s1_and_s2, s_union_of_s1_and_s2.begin()));
        cout << "s1               " << s1 << endl;
        cout << "s2                     " << s2 << endl;
        cout << "union            " << s_union_of_s1_and_s2 << endl;

        std::set<int> s_intersection_of_s1_and_s2{};
        std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(s_intersection_of_s1_and_s2, s_intersection_of_s1_and_s2.begin()));
        cout << "intersections          " << s_intersection_of_s1_and_s2 << endl;

        std::set<int> s_difference_of_s1_and_s2{};
        std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(s_difference_of_s1_and_s2, s_difference_of_s1_and_s2.begin()));
        cout << "difference       " << s_difference_of_s1_and_s2 << endl;
    }
    // heap operations: make_heap, push_heap, pop_heap
    {
        cout << "heap" << endl;
        {
            cout << "max heap" << endl;
            std::vector<int> v_heap{ 0,1, 2, 3, 4, 5, 6, 7, 8, 9 };
            cout << "initial:             " << v_heap << endl;
            std::make_heap(v_heap.begin(), v_heap.end());
            cout << "after make_heap:     " << v_heap << endl;
            v_heap.push_back(8);
            cout << "after push_back(8):  " << v_heap << endl;
            std::push_heap(v_heap.begin(), v_heap.end());
            cout << "after push_heap():   " << v_heap << endl;
            pop_heap(v_heap.begin(), v_heap.end());
            cout << "after pop_heap():    " << v_heap << endl;
            v_heap.pop_back();
            cout << "after pop_back():    " << v_heap << endl;
        }
        {
            cout << "min heap" << endl;
            std::vector<int> v_heap{ 0,1, 2, 3, 4, 5, 6, 7, 8, 9 };
            cout << "initial:             " << v_heap << endl;
            std::make_heap(v_heap.begin(), v_heap.end(), std::greater<>{});
            cout << "after make_heap:     " << v_heap << endl;
            v_heap.push_back(1);
            cout << "after push_back(1):  " << v_heap << endl;
            std::push_heap(v_heap.begin(), v_heap.end(), std::greater<>{});
            cout << "after push_heap():   " << v_heap << endl;
            pop_heap(v_heap.begin(), v_heap.end(), std::greater<>{});
            cout << "after pop_heap():    " << v_heap << endl;
            v_heap.pop_back();
            cout << "after pop_back():    " << v_heap << endl;
        }
    }

    // Composability of algorithms sucks :(
    // Ranges are better in this regard (see ranges.cpp)

    return 0;
}
