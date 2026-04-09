#include <iostream>
#include <ranges>
#include <cassert>
#include <vector>
#include <algorithm>

int main() {
    // More modern iteration method that provides (some) iterator composability in C++ 20
    // https://en.cppreference.com/w/cpp/ranges
    auto const ints = {0, 1, 2, 3, 4, 5};
    auto even = [](int it) { return 0 == it % 2; };
    auto square = [](int it) { return it * it; };

    // "pipe" syntax for composing individual views:
    for (int it : ints | std::views::filter(even) | std::views::transform(square)) {
        std::cout << it << ' ';
    }
    std::cout << std::endl;

    // Range adaptors are lazy
    int iteration = 0;
    auto view = ints | std::views::transform([&iteration](int v) {
        iteration++;
        return v + 1;
    });
    auto it = view.begin();
    auto value = *it;
    assert(iteration == 1);
    *it;
    assert(iteration == 2);

    std::vector<int> items{5, 3, 8, 2, 1, 0};

    std::cout << "---" << std::endl;
    // Reverse iteration
    for (auto item : items | std::views::reverse) {
        std::cout << item << std::endl;
    }

    std::cout << "---" << std::endl;
    // Range adaptors (views) are applied from the left side
    // First, the iteration is reversed, and then two items are dropped
    // Equivalent to: std::views::drop(std::views::reverse(items), 2))
    for (auto item : items | std::views::reverse | std::views::drop(2)) {
        std::cout << item << std::endl;
    }

    std::vector<std::string> words{"Hello", "world"};
    for (auto ch: words | std::views::join) {
        std::cout << ch;
    }
    std::cout << std::endl;

    return 0;
}
