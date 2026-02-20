#include <iostream>
#include <limits>

// You can use the C standard library, but with different paths
// Instead of `stdint.h` use `cstdint`, instead of `stdio.h` use `cstdio`, etc.
#include <cstdint>

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

void type_alises() {
    // C-style macro-based type alias
    #define my_int1 int
    my_int1 a1;

    // C-style typedef type alias
    typedef int my_int2;
    my_int2 a2;

    // C++-style using type alias
    using my_int3 = int;
    my_int3 a3;
}

void type_deduction() {
    // auto can be used to infer the type (type deduction)
    auto inferred1 = 1; // int
    auto inferred2 = 1ul; // unsigned long int

    const auto inferred3 = 1;   // const int
    decltype(auto) inferred4 = inferred1;   // Might (?) be useful for templates later
}

// Functions can have a trailing return type
// This will be useful later with templates + auto
auto trailing_return() -> int {
    return 5;
}

// Return type determined by first return statement
auto auto_return() {
    if (true)
    {
        return 42;
    }
    return 1;   // return type here must match the previous return
}

/*
 * Functions can be overloaded (multiple functions with the same name may exist)
 * They must have different parameter types/order/count.
 * Function names are mangled to allow calling from other translation units (https://demangler.com/).
 */
int fun1() { return 0; }
float fun1(float x) { return x; }           // Valid overload, differs by parameter count
float fun1(double x) { return x + 1; }      // Valid overload, differs by parameter type
// void fun1() {}                           // Invalid overload, differs only by return type

// Overloading also allows function parameters to have default values
int fun2(int x = 1, int y = 2) { return x + y; }

void overloads() {
    std::cout << fun1() << std::endl;

    // What will be printed here? 1 or 2?
    std::cout << fun1(1.0) << std::endl;

    // And here?
    std::cout << fun1(1.0f) << std::endl;

    // What about this?
    // std::cout << fun1(1) << std::endl;

    std::cout << fun2() << std::endl;
    std::cout << fun2(5) << std::endl;
    std::cout << fun2(5, 6) << std::endl;
}

void casts() {
    // C-like implicit conversions and integral promotions also apply in C++
    // See https://en.cppreference.com/w/cpp/language/implicit_conversion
    unsigned char num = 200;
    int x = num + num; // c is promoted to int, no overflow happens

    std::cout << x << std::endl;

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

int main() {
    basic_types();
//    overloads();
//    casts();

    return 0;
}
