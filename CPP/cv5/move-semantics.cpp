#include <iostream>

using std::cout;
using std::endl;

// Notice: in modern C++, implementing these manually is almost never needed
struct Foo {
    Foo() {
        cout << "empty constructor" << endl;
    }
    Foo(int x): x(x) {
        cout << "int constructor" << endl;
    }
    Foo(const Foo& other) {
        this->x = other.x;
        cout << "copy constructor" << endl;
    }
    Foo& operator=(const Foo& other) {
        this->x = other.x;
        cout << "copy assignment constructor" << endl;
        return *this;
    }
    Foo(Foo&& other) {
        // Foo f = other; // other is treated like an L-value! Because it has a name

        this->x = other.x;
        cout << "move constructor" << endl;
    }
//    Foo(Foo&&) = delete;
    Foo& operator=(Foo&& other) {
        cout << "move assignment constructor" << endl;
        this->x = other.x;
        return *this;
    }
    ~Foo() {
        cout << "destructor: " << this->x << endl;
    }

    int x;
};

void take_rref(Foo&& f) {
    f.x = 5;
    std::cout << f.x << std::endl;
}
void rvalue_as_lvalue(Foo&& foo) {
    foo = Foo(1);
    cout << "test" << endl;
}
int&& return_rref() {
    return 0;   // UB!
//    return std::move(0); // UB!
}
int&& return_rref2(int&& ref) {
//    return ref; // cannot return, ref is now l-value
    return std::move(ref);
}

int returns_value() {
    return 0;
}

// Understand what L/R/PR/GL/X-values are (https://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues)
// L-value vs R-value
// What is int&& foo(int&& x) { return std::move(x); }? -> need for X-values

int main() {
//    Foo f{};
//    Foo f2;
//    f = f2;
//    f = Foo(4);

    int x = 0;
    // std::move casts the argument to r-value ref (hides the name of the argument)
    int&& rx = std::move(x); // basically the same as static_cast<int&&>(x)

//    take_rref(Foo());
//    take_rref(std::move(f));
//     f shouldn't be touched here
//    rvalue_as_lvalue(Foo());

//    int x = return_rref();  // UB
//    int&& rx = return_rref();  // UB

    int a;
    int&& ra = return_rref2(std::move(a));
    // What is ra? Is it an r-value? Is it an l-value?
    ra = 5;

    // Extend the lifetime of a temporary value
//    int&& ref = returns_value();
//    ref = 5;
//    cout << ref << endl;

    return 0;
}
