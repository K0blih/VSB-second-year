#include <iostream>

using std::cout;
using std::endl;

// This file shows examples of "OOP runtime polymorphism", which is implemented in C++ using
// virtual methods.

class WithVTable {
public:
    virtual void foo(std::ostream& os);
//    void foo(std::ostream& os);
};

class A {
public:
    virtual void foo1() {}
    virtual void foo2() {
        cout << "A::foo" << endl;
    }
};
class B: public A {
public:
    virtual void foo1() {} // override

    using A::foo2;
    void foo2(int x=1) {
        cout << "B::foo" << endl;
    }  // hide
};

class Base {
public:
    ~Base() {
        cout << "~Base" << endl;
    }
    virtual void foo() = 0;
    virtual void foo2() {
        cout << "Base::foo2" << endl;
    }
};
class Derived: public Base {
public:
    Derived() {
        cout << "Derived" << endl;
    }
    ~Derived() {
        cout << "~Derived" << endl;
    }
    void foo() override {
        cout << "Derived::foo" << endl;
    }
    void foo2() final {
        cout << "Derived::foo2" << endl;
    }
};
class Derived2: public Derived {
public:
    Derived2(int x): x(x) {

    }
    ~Derived2() {
        cout << "~Derived2" << endl;
    }
    void foo() override {
        cout << "Derived2::foo" << endl;
    }
//    void foo2() final {} // not possible

    int x;
};
class Derived3: public Derived {};

int main() {
    B b;
    b.foo2();
    A* a = &b;
    a->foo2();

    // v-table -> size with/without virtual
    cout << sizeof(WithVTable) << endl;

    {
        Derived b = Derived2(0); // slicing!
        b.foo();
        cout << sizeof(b) << endl;
        cout << sizeof(Derived2) << endl;
    }

    // stack allocation
    Derived2 d{1};
    d.foo();
    d.foo2();

    Base* base = &d;
    base->foo();
    base->foo2();

    // Downcasting
    // Invalid cast at runtime -> returns nullptr
    cout << dynamic_cast<Derived3*>(base) << endl;

    // Valid cast
    cout << dynamic_cast<Derived2*>(base) << endl;

    Derived2 test1{1};
    Base& test2 = test1;
    // Invalid cast at runtime -> terminates with an exception
//    Derived3& test3 = dynamic_cast<Derived3&>(test2);

    // Upcasting
    Derived2* dptr = &test1;
    Base* bptr = static_cast<Base*>(dptr);  // always valid

    Base& base2 = d;
    base2.foo();
    base2.foo2();

    // heap allocation
    Base* base_heap = new Derived2(1);
    base_heap->foo();
    delete base_heap;   // wrong, non-virtual destructor!!!

    // array -> match new/delete with new[]/delete[]
    Base** items = new Base*[3]{};
    items[0] = new Derived();
    items[1] = new Derived2(0);
    items[2] = new Derived();
//    items[2] = new Base(); // not possible -> abstract class

    // polymorphism simple example
    for (int i = 0; i < 3; i++) {
        items[i]->foo();
    }

    delete[] items;

    // What does new/delete do?
    Base* base_ptr = (Base*) malloc(sizeof(Derived)); // allocation
    // initialization (simulated with placement new)
    new (base_ptr) Derived();

    // destructor can be called manually
    base_ptr->~Base();
    free(base_ptr);

    return 0;
}
