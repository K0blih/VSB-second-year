#include <iostream>
#include <vector>
#include <array>

using std::cout;
using std::endl;

// Pre-C++ 20 and concepts it was quite annoying to make sure that some type adheres to a specific
// interface. It can be done using type-traits (https://en.cppreference.com/w/cpp/header/type_traits).
// Type traits leverage SFINAE (https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error).

// Check that type is in a set of explicitly allowed types
template <typename>
struct is_my_type {
    const static bool value = false;
};

template <>
struct is_my_type<int> {
    const static bool value = true;
};

template <typename T>
void foo() {
    static_assert(is_my_type<T>::value);
}

// Check that type can be compared with itself
template<typename T, typename = void>
struct is_comparable: std::false_type {};

template<typename T>
struct is_comparable<T, decltype(std::declval<T>() < std::declval<T>(), void())>: std::true_type {};

struct S1 {};
struct S2 {
    bool operator<(S2 b) { return true; }
};

int main() {
    foo<int>();
//    foo<bool>();

    static_assert(!is_comparable<S1>::value);
    static_assert(is_comparable<S2>::value);

    return 0;
}
