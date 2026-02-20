#include <iostream>

void change_by_ref(int& item) {
    item = 5; // What does this do?
}
void take_const_ref(const int& item) {
    // item = 5;
    std::cout << item << std::endl;
}
void take_value(int item) {
    item = 5;
    std::cout << item << std::endl;
}

// This has the same problems as if pointers were used - UB! DO NOT do this.
int& return_ref() {
    int a;
    return a;
}

int global_variable = 0;
// You can however refer to things that live long enough
int& return_global_ref() {
    return global_variable;
}

void test_return_global_ref() {
    std::cout << global_variable << std::endl;
    return_global_ref() = 5;
    std::cout << global_variable << std::endl;
}

// Use const to state intent
// a will be probably modified, b will be read-only
// More useful for larger data types, integers should be passed by value
void do_something(int& a, const int& b) {}

void overloaded(int x) {
    std::cout << "By value" << std::endl;
}
void overloaded(int& x) {
    std::cout << "By reference" << std::endl;
}
void overloaded(const int& x) {
    std::cout << "By const reference" << std::endl;
}

int main() {
    // Pointers
    int a = 5;
    int b = 6;
    int* pointer = &a;

    // nullptr vs NULL
    int* empty = nullptr;

    // What happens if you reassign a pointer?

    // Const pointers
    const int* pc1 = &a;
    int * const pc2 = &a;
    const int * const pc3 = &a;

    // (l-value) References
    int& ref = a;

    // What happens if you reassign ref?

    // What happens if you create a reference to a constant literal?
    // Does it work if the reference is constant?

    // Is there a difference between the two references below? (east-const vs west-const)
    const int& refx1 = a;
    int const & refx2 = a;

    // Can you create a reference to "nothing"?

    int x = 8;
    change_by_ref(x);
    // What is the value of `x` here?

    // Can you pass a literal (e.g. 5) to change_by_ref?
    // What about take_const_ref? What about take_value?

    // Which of the following calls are valid?
    // Which overload will be called?
//     overloaded(5 + 5);
//     overloaded(a);

    // Which overload can we remove to make it work?

    // References are a bit "special", cannot be used everywhere
    // int& a[3]; // error
    // int&* p;   // error
    // int& &r;   // error

    // Reference collapsing
    int var1 = 1;
    int& ref1 = var1;
    int& ref2 = ref1;
    ref2 = 6;
    std::cout << var1 << " " << ref2 << std::endl;

    // Reference decay to value copy
    int var2 = ref1;
    ref1 = 8;
    var2 = 9;
    std::cout << ref1 << " " << var2 << std::endl;

    auto& rvar2 = var2;  // ref to var2
    const auto& crvar2 = var2;  // const ref to var2

//    test_return_global_ref();

    // There are also r-value references! Wait for later :)
    int&& rref = 5;

    return 0;
}
