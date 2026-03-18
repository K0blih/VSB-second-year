#include <iostream>
#include <memory>
#include <utility>

using std::cout;
using std::endl;

class Foo {
public:
    explicit Foo(int x): x(x) {}

    int get_x() const {
        return this->x;
    }

private:
    int x;
};

class Node {
public:
    Node() = default;
    ~Node() {
        cout << "~Node" << endl;
    }
    explicit Node(std::shared_ptr<Node> ptr): link(std::move(ptr)) {}

    std::shared_ptr<Node> link;
};

class NodeWeakRef {
public:
    NodeWeakRef() = default;
    ~NodeWeakRef() {
        cout << "~NodeWeakRef" << endl;
    }
    explicit NodeWeakRef(std::shared_ptr<NodeWeakRef> ptr): link(ptr) {}

    std::weak_ptr<NodeWeakRef> link;
};

void take_foo(std::unique_ptr<Foo> foo) {

}

// Uniquely-owned pointer
void unique_ptrs() {
    //    std::unique_ptr<Foo> p1 = std::unique_ptr<Foo>(new Foo(1)); // may leak with exceptions!
    std::unique_ptr<Foo> p1 = std::make_unique<Foo>(1); // prefer make_unique

    // construction -> memory allocation (new)
    // destruction -> memory is freed (delete)
    // unique_ptr -> it is the unique owner of data

    // get raw pointer from unique_ptr
    Foo* raw_ptr = p1.get();
    // this pointer must not be used after p1 is deleted/moved

    // -> operator delegates to the inner object
    // how is this possible? operator -> is "recursive"
    cout << p1->get_x() << endl;

    // pass unique_ptr to a function
//    take_foo(p1);   // deleted copy constructor
    take_foo(std::move(p1));
    // after the move p1 is reset to NULL!
//    cout << p1.get() << endl;

//    p1.reset(); // deallocate the object
//    p1.reset(new Foo(2)); // deallocate the previous object and manage a new one
//    p1 = std::make_unique<Foo>(2); // better

    Foo* raw_ptr2 = p1.release(); // stop managing the object without destroying it

    std::unique_ptr<Foo> p_null; // unique_ptr can be NULL :(
    cout << p_null.get() << endl;

    auto array = std::make_unique<int[]>(100); // unique_ptr can also manage arrays
    array[0] = 5;
    array[1] = 2;
}

// (Thread-safe) Reference-counted pointer
void shared_ptrs() {
    std::shared_ptr p1 = std::make_unique<Foo>(5);
    cout << p1.use_count() << endl;
    std::shared_ptr p2 = p1; // second pointer pointing to the same data
    cout << p1.use_count() << endl;

    p1.reset(); // p1 no longer points to Foo, but p2 does, so the memory is still valid
    cout << p2->get_x() << endl;
    // after the last reference disappears, the object is deallocated
    p2.reset(); // now the memory was deallocated

    // shared_ptr has a pointer to shared state on the heap (contains refcount and other things)

    // DO NOT do this! The two pointers will be separate and memory will be freed twice
//    auto* memory = new Foo(1);
//    std::shared_ptr<Foo> ptr1(memory);
//    std::shared_ptr<Foo> ptr2(memory);

    // Cycles
//    auto n1 = std::make_shared<Node>();
//    n1->link = n1;
    // memory leak created here

    auto n2 = std::make_shared<NodeWeakRef>();
    n2->link = n2;
    // n2->expired(); // is the weak link valid?
    // n2->link.lock(); // upgrade to shared_ptr
}

int main() {
//    unique_ptrs();
    shared_ptrs();

    return 0;
}
