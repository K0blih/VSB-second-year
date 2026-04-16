#include "template-lib.h"

#include <iostream>

int foo1(int x) {
    return x + 1;
}

template <typename T>
int foo2(T t) {
    std::cout << t << std::endl;
    return 0;
}

// Explicit instantiation for T=int
template int foo2<int>(int);

// Or you can instantiate it yourself
void foo3() {
//    foo2<int>(1);
}
