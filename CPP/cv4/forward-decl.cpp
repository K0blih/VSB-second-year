/*
 * What to do when two classes need to use themselves cyclically?
 * Well, besides refactoring (which might be a good idea in this situation),
 * you can use a forward declaration.
 *
 * This says: there will be a class named Foo, but it is defined elsewhere.
 */
class Foo;

class Bar {
    // Here the layout of Foo is not known, therefore you have to use Foo behind an indirection!
    Foo* foo;

    // If you want to implement something that knows the layout of Foo, you have to do it in
    // the corresponding .cpp file, where you will include e.g. "foo.h"
    // to know the definition of Foo.
    void take_foo(Foo foo);
};

// In another translation header file:
class Bar;

class Foo {
    void take_bar(Bar bar); // implemented in foo.cpp
};
