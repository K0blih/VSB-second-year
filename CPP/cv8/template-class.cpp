#include <iostream>
#include <vector>

using std::cout;
using std::endl;

/*
 * We can also use templates to design generic classes - prime example is e.g. std::vector<T>,
 * which can hold any type (that supports the operations that std::vector requires).
 */

template <typename T=int>
class MyContainer {
public:
    explicit MyContainer(std::vector<T> items): items(std::move(items)) {}

    void foo() {
        std::cout << this->items[0] << std::endl;
    }
    void foo2();

    template <typename R>
    void foo2(R r) {
        for (auto& item: this->items) {
            item += r;
        }
    }

    // Not possible - why?
//    template <typename R>
//    virtual void foo3(R r) {}

private:
    std::vector<T> items;
};

template <typename T>
void MyContainer<T>::foo2() {

}

int main() {
    MyContainer<> c1{{1, 2, 3}};
    MyContainer<float> c2{{1, 2, 3}};

    c2.foo();
    c2.foo2(5);

    return 0;
}
