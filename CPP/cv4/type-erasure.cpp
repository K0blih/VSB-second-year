#include <iostream>
#include <array>
#include <memory>

/*
 * This is yet another approach for implementing type hierarchies with an open set of types, but
 * closed set of operations (for cases where it should be easy to add new types).
 * It is an alternative to the classic "OOP-style" polymorphism with inheritance and virtual methods.
 * It provides a way of attaching behaviour to classes without needing to modify them.
 *
 * You can learn about it in this talk by Klaus Iglberger: https://www.youtube.com/watch?v=4eeESJQk-mw
 */

struct Square {
    int x;
};

struct Circle {
    int y;
};

struct Strategy1 {
    void fn1(const Circle& circle) const {
        std::cout << "strategy1: fn1: circle" << std::endl;
    }
    void fn1(const Square& sq) const {
        std::cout << "strategy1: fn1: square" << std::endl;
    }
    void fn2(const Circle& circle) const {
        std::cout << "strategy1: fn2: circle" << std::endl;
    }
    void fn2(const Square& square) const {
        std::cout << "strategy1: fn2: square" << std::endl;
    }
};
struct Strategy2 {
    void fn1(const Circle& circle) const {
        std::cout << "strategy2: fn1: circle" << std::endl;
    }
    void fn1(const Square& sq) const {
        std::cout << "strategy2: fn1: square" << std::endl;
    }
    void fn2(const Circle& circle) const {
        std::cout << "strategy2: fn2: circle" << std::endl;
    }
    void fn2(const Square& square) const {
        std::cout << "strategy2: fn2: square" << std::endl;
    }
};

class Shape {
private:
    class ShapeConcept {
    public:
        virtual ~ShapeConcept() = default;

        virtual void fn1() const = 0;
        virtual void fn2() const = 0;
    };

    template <typename T, typename S>
    class ShapeModel: public ShapeConcept {
    public:
        explicit ShapeModel(T&& object, S s): object(std::forward<T>(object)), strategy(s) {}

        void fn1() const override {
            this->strategy.fn1(this->object);
        }
        void fn2() const override {
            this->strategy.fn2(this->object);
        }

    private:
        T object;
        S strategy;
    };

public:
    template <typename T, typename S>
    explicit Shape(T&& item, S strategy): item(new ShapeModel{std::forward<T>(item), strategy}) {}

    virtual void fn1() const {
        this->item->fn1();
    }
    virtual void fn2() const {
        this->item->fn2();
    }

private:
    std::unique_ptr<ShapeConcept> item;
};

int main() {
    // Here we can take an arbitrary shape (type) and attach an arbitrary behavior (strategy) to it.
    // It will still have the same type and it will be possible to store it in a collection easily.
    Shape s1{Circle{}, Strategy1{}};
    Shape s2{Square{}, Strategy2{}};

    s1.fn1();
    s2.fn2();

    return 0;
}
