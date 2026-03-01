#include <iostream>

using std::cout;
using std::endl;

struct Bar {
    int x;
};

struct Foo {
    Foo() {
        cout << "empty constructor" << endl;
    }
    Foo(int x) {
        cout << "(int x) constructor" << endl;
    }
//    explicit Foo(int x) {
//        cout << "explicit (int x) constructor" << endl;
//    }
    Foo(std::initializer_list<int>) {
        cout << "initializer list" << endl;
    }
    Foo(const Foo&) {
        cout << "copy constructor" << endl;
    }
    Foo& operator=(const Foo&) {
        cout << "copy assignment constructor" << endl;
        return *this;
    }
};

struct InitList1 {
    InitList1(int a, int b) {
        cout << "constructor" << endl;
    }
};

struct InitList2 {
    InitList2(int a, int b) {
        cout << "constructor" << endl;
    }

    InitList2(std::initializer_list<int> l) {
        cout << "initializer list constructor" << endl;
    }
};

void fun1(Foo f) {
    cout << "fun1: Foo" << endl;
}

struct FooOwner {
    FooOwner(int x): x(x) {}  // single init
//    FooOwner(int x) { this->x = x; }  // triple init

    Foo x = 1;
};

// No default constructor, reference must be initialized
struct OwnsRef {
    int& a;
};

struct CtorDelegation {
    CtorDelegation(): CtorDelegation(1) {}
    explicit CtorDelegation(int a, int b = 1): x(a), y(b) {}

    int x;
    int y;
};

int main() {
    Bar b1;
    std::cout << b1.x << std::endl; // UB: read from uninitialized memory

    Foo f1;
//    Foo f2();
    Foo f2(1);

    Foo f3 = 1;
    Foo f4 = Foo(1);

    Foo f5;
    f5 = Foo(1);

    Foo f6 = {1};

    // Preferred way
    // Does not allow narrowing
    // Calls constructor if found, otherwise initializes members
    Foo f7{1};

    Bar b2 = {1};
    std::cout << b2.x << std::endl;
    Bar b3{1};
    std::cout << b3.x << std::endl;

    InitList1 il1{1, 2};
    InitList1 il2 = {1, 2};

    InitList2 il3{1, 2};
    InitList2 il4 = {1, 2};

    fun1(1);

    // Multiple member initialization
    FooOwner s{1};

//    OwnsRef ownsRef;

    return 0;
}
