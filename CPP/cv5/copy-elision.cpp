#include <iostream>

using std::cout;
using std::endl;

struct Foo {
    Foo() {
        cout << "empty constructor" << endl;
    }
    Foo(int x): x(x) {
        cout << "int constructor" << endl;
    }
    // No functions called in main require any copy!
    Foo(const Foo& other) = delete;
    Foo& operator=(const Foo& other) = delete;

    Foo(Foo&& other) noexcept {
        this->x = other.x;
        cout << "move constructor" << endl;
    }
//    Foo(Foo&&) = delete;
    Foo& operator=(Foo&& other) {
        cout << "move assignment constructor" << endl;
        this->x = other.x;
        return *this;
    }
//    Foo& operator=(Foo&& other) = delete;
    ~Foo() {
        cout << "destructor: " << this->x << endl;
    }

    int x;
};

// https://en.cppreference.com/w/cpp/language/copy_elision
// Copy elision can break the AS-IF rule!
Foo return_local_by_val_0() {
    return Foo{1}; // RVO
}
Foo return_local_by_val_1() {
    Foo foo{3};
    return foo; // NRVO
}
// RVO, elided
Foo return_local_by_val_2(bool first=true) {
    if (first) {
        return Foo(1);
    } else {
        return Foo(2);
    }
}
// NRVO, elided
Foo return_local_by_val_3(bool first=true) {
    Foo foo{5};

    if (first) {
        std::cout << "do something" << std::endl;
        return foo;
    }
    return foo;
}
// NRVO, not elided
Foo return_local_by_val_4() {
    Foo foo1{3};
    Foo foo2{4};

    if (true) {
        return foo1;
    } else {
        return foo2;
    }
}

// Not elided, but moved
Foo return_param_by_val(Foo foo) {
    return foo;
}
// Elided
void take_by_value(Foo foo) {}

int main() {
    Foo f0 = return_local_by_val_0();
//    Foo f1 = return_local_by_val_1();
//    Foo f2 = return_local_by_val_2();
//    Foo f3 = return_local_by_val_3();
//    Foo f4 = return_local_by_val_4();
//    Foo f_val = return_param_by_val(Foo{6});
//    take_by_value(Foo{1});

    return 0;
}
