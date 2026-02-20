#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include <limits>
#include <vector>

namespace math
{
    int fibonacci(int n);
}

namespace strutils
{
    std::string camel_to_snake(const std::string &input);
    bool parse_uint(const std::string &input, std::uint32_t &result);
    bool validate_utf8(const std::vector<std::uint8_t> &bytes, std::size_t &codepoints);
}