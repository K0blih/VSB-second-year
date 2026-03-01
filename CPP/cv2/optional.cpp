#include <optional>
#include <vector>
#include <functional>
#include <iostream>

/**
 * Use std::optional to let the user of your API know that some value might not be present, e.g.
 * something was not found in a collection or in a database.
 */

std::optional<int> find(const std::vector<int>& items, std::function<bool(int)> filter_fn) {
    for (const auto& item: items) {
        if (filter_fn(item)) return item;
    }

    return std::nullopt;
}

int main() {
    auto result = find({1, 2, 3}, [](auto value) { return value % 2 == 0; });
    if (result.has_value()) {
        // runtime check
        std::cout << result.value() << std::endl;
    }
    if (result) {
        // without check (preferably do not use, UB if value is missing)
        std::cout << *result << std::endl;
    }

    std::optional<bool> value = false;
    if (value) {
        std::cout << "Has value!" << std::endl;
        if (*value) {
            std::cout << "Value is true" << std::endl;
        }
    }
    bool default_value = value.value_or(true);

    // std::optional cannot currently work with references :(
    // https://thephd.dev/to-bind-and-loose-a-reference-optional

    return 0;
}
