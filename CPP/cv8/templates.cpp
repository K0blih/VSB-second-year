#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

using std::cout;
using std::endl;

/*
 * Templates provide several functionalities:
 * - They allow us to write generic code, which works for any (user-supplied) type that supports
 * a required set of operations. This allows us to reduce code duplication without any runtime
 * overhead.
 * - They allow us to use compile-time polymorphism, by selecting different implementations based
 * on the type of a parameter.
 * - They allow us to perform compile-time programming.
 * - They allow us to have functions with arbitrary number of parameters (using variadic templates).
 */

// This is not a function, only a template
template <typename T> // or class T
T add(const T& a, const T& b) {
    // Compiles as long as T + T is valid
    return a + b;
}

// You can use multiple template parameters
template <typename T, typename R>
auto add2(const T& a, const R& b) -> decltype(a + b) { // Return type is deducted from the type of expression `a + b`
    // Compiles as long as T + R is valid
    return a + b;
}

template <typename T>
void foo() {}

template <typename Key, typename Value=int>
std::vector<std::pair<Key, Value>> create_pairs(const Key& key, int count) {
    std::vector<std::pair<Key, Value>> pairs;
    std::generate_n(std::back_inserter(pairs), count, [&key]() {
        return std::make_pair(key, Value{});
    });
    return pairs;
}

template <typename Function>
void apply_function(std::vector<int>& items, Function fn) {
    for (auto& item: items) {
        fn(item);
    }
}

int main() {
    // Parametrize function by a type
    float r1 = add<float>(3.0f, 4.0f); // Here the `add<float>` function is instantiated
    int r2 = add<int>(3, 5); // Here the `add<int>` function is instantiated

    add<float>(5, 6);   // The previous instantiation is used here

    // Here the type is inferred to be T=int
    add(8, 9);

//    add(8.0, 4); // inference fails, cannot unify double with int
    add<float>(8.0, 4); // ok, parameters will be casted to T=float
//    foo(); // no inference possible

    float result = add2(1, 4.0f); // T=int, R=float

    auto map1 = create_pairs(std::string("Key"), 5);    // std::vector<std::pair<std::string, int>>
    auto map2 = create_pairs<int, double>(1, 10);        // std::vector<std::pair<int, double>>

    std::vector<int> items{1, 2, 3, 4, 5};

    // The passed lambda will be probably inlined -> possibly better performance
    apply_function(items, [](const auto& item) {
        std::cout << item << std::endl;
    });

    return 0;
}
