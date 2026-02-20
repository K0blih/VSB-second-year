// Things from C and C++ standard libraries live in the std namespace
#include <iostream>

// What does static mean here?
static void fun1() {

}

// What does static mean here?
static int x1 = 1;

// What does inline mean here? How does it relate to headers?
inline void fun2() {

}

inline int x2 = 1;

// Shortcut to "import" cout
using std::cout;

// Import all items from the target namespace
// DO NOT do this inside header files, it may cause name clashes
// Useful in source files
// using namespace std;

void call_cout() {
    std::cout << "Hello" << std::endl;

    // using std::cout; // Can be also used locally inside functions

    cout << "Hello again" << std::endl;
}


// Create our own namespace
// Things inside foo will be used through foo::<...>
namespace foo {
    void fun() {}

    // Namespace can be nested
    namespace bar {
        void fun() {}
    }
}

// anonymous namespaces have similar functionality to `static`
namespace {
    void this_function_is_tu_private() {}
}

int main() {
    foo::fun();
    foo::bar::fun();

    using namespace foo;
    using namespace foo::bar;
    // fun();  // Error: name clash

    return 0;
}
