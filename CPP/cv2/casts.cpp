#include <iostream>
#include <limits>

void basic_types() {
    // Basic types: char, short, int, long int, long long int
    // The following is guaranteed:
    // 1 == sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long)

    bool isValid = true; // C++ has built-in bool type and true/false literals

    // For fixed-size integers, use types from <cstdint>
    uint32_t this_type_has_exactly_4_bytes;

    // Use literal suffixes to explicitly specify type
    unsigned long long x1 = 5ull;
    float x2 = 1.0f;

    // Decimal/hexadecimal/octal/binary base literals
    int decimal = 10;
    int hexadecimal = 0x10;
    int octal = 010;
    int binary = 0b10;

    std::cout << decimal << " " << hexadecimal << " " << octal << " " << binary << std::endl;

    // Floating point literals
    double dval = 3e-5; // 3 * pow(10, -5)

    // Types can be cv-qualified (const, volatile or const volatile). See references.cpp
    const int constant = 1;

    std::cout << typeid(int).name() << std::endl;

    // constant2 has the same type as constant
    decltype(constant) constant2 = 0;
}

void casts() {
    int floored = 1.5; // floating point numbers will be converted by stripping the decimal part
    std::cout << floored << std::endl;

    // Be wary of overflows
    int maximum_value = std::numeric_limits<int>::max(); // maximum value of int
    maximum_value += 1; // *signed* int overflow is UB

    // C-style casts can be used: float* x = (int*) ...
    // But they are unpredictable (https://anteru.net/blog/2007/c-background-static-reinterpret-and-c-style-casts/).
    // Do not use them.

    // There are several way of C++-style type casts
    // static_cast is the "safest" cast, prefer it if possible. It is similar to C casts, but allows less conversions.
    int a = static_cast<int>(1.5f);

    const int constant = 1;
    // const_cast can be used to add or remove const (or volatile). Very dangerous, avoid if possible.
    int* b = const_cast<int*>(&constant);
    // Modifying a const value (after removing the const qualifier) is UB. DO NOT do this!

    int integer_number = 0;
    // reinterpret_cast can be used e.g. for casts between pointers of different types (signed int* -> unsigned int*). Maybe.
    // In most cases, it accidentally causes UB! Avoid if possible.
    float* c = reinterpret_cast<float*>(&integer_number);   // This is UB!

    // There is also dynamic_cast used for polymorphism. We will use it later when we talk about classes and structures.

    // There is also bit_cast in C++ 20, which is a fixed version of reinterpret_cast.
}

void conversions() {
    // C-like implicit conversions and integral promotions also apply in C++
    // See https://en.cppreference.com/w/cpp/language/implicit_conversion
    uint8_t x = 255;
    uint8_t y = 255;

    // promotion from uint8_t to uint64_t (same signedness)
    uint64_t xy = x + y;
    std::cout << xy << std::endl;

    // signed to unsigned promotion
    uint32_t a = 2;
    int b = 10;
    std::cout << a - b << std::endl;

    uint32_t size = 5;
    if (size > -1) {
        std::cout << "Larger than -1" << std::endl;
    }

    // short to int promotion
    unsigned short one = 1;
    unsigned short max = std::numeric_limits<unsigned short>::max();

    // here `one + max` is promoted to int, then narrowed back to short
    unsigned short sum = one + max;

    // but here there's no narrowing after the promotion!
    // both sum and one + max are promoted to int
    // 0 == 65536
    if (sum == one + max)
        std::cout << "sum = one + max, and sum == one + max\n";
    else
        std::cout << "sum = one + max, but sum != one + max\n";

    uint16_t x2 = std::numeric_limits<uint16_t>::max();
    uint16_t x3 = std::numeric_limits<uint16_t>::max();

    // here int * int is multiplied
    uint64_t result1 = x2 * x3;
    std::cout << result1 << std::endl;

    uint64_t x4 = std::numeric_limits<uint16_t>::max();
    uint64_t x5 = std::numeric_limits<uint16_t>::max();
    uint64_t result2 = x4 * x5;
    std::cout << result2 << std::endl;
}

int main() {
    basic_types();
//    casts();
//    integer_promotion();

    return 0;
}
