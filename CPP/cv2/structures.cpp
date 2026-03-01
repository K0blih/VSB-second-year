#include <iostream>

#include "structures.h"

using std::cout;
using std::endl;

int Bar::fun2()
{
    return 2;
}

struct Struct {
    // default: public
private:
    int f1;

public:
    int f2;

protected:
    int f3;

private:
    int f4;
};
class Class {
    // default: private
};

// Order of initialization matters!
struct FieldInit {
    FieldInit(int x, int y): x(x), y(y + 1) {}

    int x;
    int y;
    int z = 123;
};

struct StaticMembers {
    StaticMembers(int x): x(x) {}

    int x;
    static int static_field;
};

int StaticMembers::static_field = 3;

struct Methods {
    int fun() {
        // Implicit `this` pointer pointing to the current instance
        return this->x;

        // Prefer explicit this
        // return x;
    }

    void does_it_crash() {
        std::cout << "does_it_crash called" << std::endl;
    }

    void const_method() const {
//        this->x = 5;  // invalid
        this->cache++;  // valid
        cout << "const method called" << endl;
    }

    void overload_by_ref() & {
        cout << "overload_by_ref called on l-value ref" << endl;
    }
    void overload_by_ref() && {
        cout << "overload_by_ref called on r-value ref" << endl;
    }

    int x = 0;
    mutable int cache = 0;
};

// Rule of three/five
struct Foo {
//    Foo() {
//        cout << "empty constructor" << endl;
//    }
    Foo() = default;
    Foo(const Foo&) = delete; // Forbid copying
    Foo& operator=(const Foo&) = delete; // Forbid copy assignment

    ~Foo() = default;   // Destructor
};

void take_foo_by_val(Foo foo) {}

class Friendly;   // Forward declaration

class Friend {
public:
    void foo(Friendly& friendly);
//    void foo2(Friendly friendly) {} // Invalid, Friendly type is incomplete
};

class Friendly {
    friend void Friend::foo(Friendly& friendly);

private:
    int private_field = 0;
};

void Friend::foo(Friendly& friendly) {
    friendly.private_field++;
}

// Operator overloading
struct Vec {
    int x;
    int y;

    Vec operator+(Vec& other) {
        Vec vec{this->x + other.x, this->y + other.y};
        return vec;
    }
};

std::ostream& operator<<(std::ostream& os, const Vec& vec) {
    os << "[" << vec.x << ", " << vec.y << "]";
    return os;
}

struct Castable {
    explicit operator int() const { return value; }
    explicit operator bool() const { return value > 0; }

    int value;
};

struct Callable {
    int operator()(int value) {
        cout << "Called with " << value << endl;
        return value + 1;
    }
};

struct Attributes {
public:
    Attributes(int value): value(value) {}

    [[nodiscard]] int get_value() const {
        return this->value;
    }

private:
    int value;
};

int main() {
    Foo f;
//    f = Foo();

    StaticMembers s{1};
//    s.static_field; // access through object
//    StaticMembers::static_field // access through class

    Methods m;
    m.fun();

    Methods* mp = nullptr;
    mp->does_it_crash();    // UB!!!

    Methods m2;
    m2.const_method();
    m2.overload_by_ref();

    Methods().overload_by_ref();

    Vec v1{1, 2};
    Vec v2{2, 3};
    Vec v3 = v1 + v2;
    cout << v3 << endl;

    Castable c{1};
    int x = static_cast<int>(c);
    cout << x << endl;

    if (c) {
        cout << "Larger than zero" << endl;
    }

    Callable callable;
    callable(5);

    return 0;
}
