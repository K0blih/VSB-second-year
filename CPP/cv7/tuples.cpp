#include <tuple>
#include <string>
#include <iostream>

int main() {
    // anonymous product type
    std::tuple<int, bool, std::string> data = std::make_tuple(1, true, "foo");

    // accessing members by index
    std::cout << std::get<0>(data) << std::endl;

    // accessing members by type
    std::cout << std::get<int>(data) << std::endl;

    // accessing members by destructuring
    auto [a, b, c] = data;
    std::cout << a << " " << b << " " << c << std::endl;

    // special case for tuple of size 2
    std::pair<int, bool> data2{1, true};
    std::cout << data2.first << " " << data2.second << std::endl;

    return 0;
}
