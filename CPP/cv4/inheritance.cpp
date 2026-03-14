#include <iostream>

using std::cout;
using std::endl;

// EBCO (empty base class optimization)
// Every object in C++ has to have size at least 1.
// But when you derived from an empty base class, its size (1) is not added automatically to the
// derived class, to avoid wasting space. Therefore, `sizeof(Derived)` here is 1.
class Base {

};
class Derived: public Base {

};

// Inheritance visibility
// https://stackoverflow.com/a/1372858/1107768
class A
{
public:
    int x;
protected:
    int y;
private:
    int z;
};

class B : public A
{
    // x is public
    // y is protected
    // z is not accessible from B
};

class C : protected A
{
    // x is protected
    // y is protected
    // z is not accessible from C
};

class D : private A    // 'private' is default for classes
{
    // x is private
    // y is private
    // z is not accessible from D
};

// Multiple inheritance - diamond problem
// (please don't use multiple inheritance where the base classes have attributes :) ).
class Base1 {
//    int x;
    virtual void foo() = 0;
};

//class Base2: Base1 {
//    void foo() override {}
//};
//class Base3: Base1 {
//    void foo() override {}
//};
//class Base4 final: Base2, Base3 {
//    void foo() final {}
//};

// Virtual inheritance
// Each variable from base class only has a single copy
class Base2: virtual Base1 {
    void foo() override {}
};
class Base3: virtual Base1 {
    void foo() override {}
};
class Base4: Base2, Base3 {
    void foo() override {}
};

// Multiple v-tables
// More at https://shaharmike.com/cpp/vtable-part1/
class Mother {
public:
    virtual void foo() {
        cout << "Mother::foo" << endl;
    }
};

class Father {
public:
    virtual void foo() {
        cout << "Father::foo" << endl;
    }
};

class Child : public Mother, public Father {
public:
    virtual void foo() {
        cout << "Child::foo" << endl;
//        Father::foo();  // call parent method
    }
};


// Constructor call order and constructor delegation
class Ctor1 {
public:
    Ctor1() {
        cout << "Ctor1" << endl;
    }
    explicit Ctor1(int x) {
        cout << "Ctor1(int x)" << endl;
    }
    ~Ctor1() {
        cout << "~Ctor1" << endl;
    }
};
class Ctor2: public Ctor1 {
public:
    Ctor2() {
        cout << "Ctor2" << endl;
    }
    explicit Ctor2(int x): Ctor1(x + 1) {
        cout << "Ctor2(int x)" << endl;
    }
    ~Ctor2() {
        cout << "~Ctor2" << endl;
    }
};

int main() {
    cout << "Base1: " << sizeof(Base1) << "\n"
         << "Base2: " << sizeof(Base2) << "\n"
         << "Base3: " << sizeof(Base3) << "\n"
         << "Base4: " << sizeof(Base4) << endl;
    //    Base4 b;
//    b.x = 4;

    Child a;
    a.foo();
    Mother* mother = &a;
    mother->foo();
    Father* father = &a;
    father->foo();

    // Father* has a different address!
    cout << &a << " " << mother << " " << father << endl;

    {
        Ctor2 ctor1;
    }
    {
        Ctor2 ctor2(1);
    }

    return 0;
}
