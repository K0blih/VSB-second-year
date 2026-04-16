#include <iostream>
#include <vector>

using std::cout;
using std::endl;

/*
 * We can "specialize" the implementation of a templated function/method for a specific type -
 * e.g. to provide a more optimized implementation.
 *
 * We should make sure that only the implementation changes, and not the interface in any way -
 * the cautionary tale is std::vector<bool>, which cannot be used as other vector types in all cases
 * (https://stackoverflow.com/a/17794965/1107768).
 */

// Default template function
template <typename T>
void foo(const T& t) {
    std::cout << "Value: " << t << std::endl;
}

// Specialization for T=bool (the template argument T was removed from the template list)
template <>
void foo(const bool& t) {
    std::cout << "Value: " << (t ? "true" : "false") << std::endl;
}

template <class T>
void foo2(std::vector<T>& v)
{
    for (auto& t : v) {
        foo(t);
    }
}

int main() {
    bool x = true;
    foo(x);
    foo(1);

    // e.g. std::vector<bool> is specialized - watch out!
    std::vector<bool> vbool;
//    bool& b = vbool[0]; // invalid

    std::vector<int> v1{1, 2, 3};
    foo2(v1);
//    std::vector<bool> v2{true, false};
//    foo2(v2); // invalid

    return 0;
}
