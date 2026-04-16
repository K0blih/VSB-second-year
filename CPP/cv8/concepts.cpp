#include <concepts>
#include <type_traits>
#include <string>
#include <iostream>

/*
 * Concepts are a C++20 feature that allows us to specify operations (and in general, an interface)
 * that a generic type has to support in order to be valid/usable for some templated function.
 *
 * It provides us with better documentation (which type can I use with this function) and, in theory,
 * also with better error messages (or at least a better location of error messages).
 */

// No type constraint
template <typename T>
T foo(T a, T b) {
    return a + b;
}

// Type trait solution
template <class T>
T foo2(T a, T b)
{
    static_assert(std::is_integral<T>::value, "Integral required.");
    return a + b;
}

// C++ 20 concept solution
template<typename T>
concept Add = requires(T a) {
    { a + a } -> std::convertible_to<T>;
};

template <Add T>
T foo3(T a, T b)
{
    return a + b;
}

template<typename T>
concept AddNumeric = requires(T a) {
    { a + a } -> std::convertible_to<T>;
} && std::is_integral<T>::value;

template <AddNumeric T>
T foo4(T a, T b)
{
    return a + b;
}

struct S {};

// SFINAE
template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
void foo5(T t) {
    std::cout << "foo 1" << std::endl;
}
template <typename T, std::enable_if_t<!std::is_arithmetic_v<T>, bool> = true>
void foo5(T t) {
    std::cout << "foo 2" << std::endl;
}

int main() {
    foo(1, 2);
    foo2(1, 2);
    foo3(1, 2);
    foo4(1, 2);

    // Compare compiler errors
//    foo(S{}, S{});
//    foo2(S{}, S{});
    foo3(S{}, S{});

//    foo(std::string{"a"}, std::string{"b"});
//    foo2(std::string{"a"}, std::string{"b"});
//    foo3(std::string{"a"}, std::string{"b"});
//    foo4(std::string{"a"}, std::string{"b"});

    foo5(1);
    foo5("1");
    return 0;
}
