#pragma once

#include <cstddef>
#include <initializer_list>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Value;
class Integer;
class Array;
class Object;
class Null;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(const Integer& value) = 0;
    virtual void visit(const Array& value) = 0;
    virtual void visit(const Object& value) = 0;
    virtual void visit(const Null& value) = 0;
};

class MutatingVisitor {
public:
    virtual ~MutatingVisitor() = default;

    virtual void visit(Integer& value) = 0;
    virtual void visit(Array& value) = 0;
    virtual void visit(Object& value) = 0;
    virtual void visit(Null& value) = 0;
};

class InvalidIndexError : public std::logic_error {
public:
    InvalidIndexError();
};

class Value {
public:
    Value() = default;
    virtual ~Value() = default;

    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;
    Value(Value&&) = delete;
    Value& operator=(Value&&) = delete;

    virtual Value* operator[](std::size_t index);
    virtual const Value* operator[](std::size_t index) const;
    virtual Value* operator[](const std::string& key);
    virtual const Value* operator[](const std::string& key) const;

    virtual Value* clone() const = 0;

    virtual void accept(Visitor& visitor) const = 0;
    virtual void accept(MutatingVisitor& visitor) = 0;
};

class Integer final : public Value {
public:
    explicit Integer(int value);

    int get_value() const;
    Integer* clone() const override;

    void accept(Visitor& visitor) const override;
    void accept(MutatingVisitor& visitor) override;

private:
    int m_value;
};

class Null final : public Value {
public:
    Null() = default;

    Null* clone() const override;

    void accept(Visitor& visitor) const override;
    void accept(MutatingVisitor& visitor) override;
};

class Array final : public Value {
public:
    Array() = default;
    Array(std::initializer_list<Value*> items);
    ~Array() override;

    Array* clone() const override;

    std::size_t size() const;
    void append(Value* value);
    void remove(std::size_t index);

    Value* operator[](std::size_t index) override;
    const Value* operator[](std::size_t index) const override;

    void accept(Visitor& visitor) const override;
    void accept(MutatingVisitor& visitor) override;

private:
    std::vector<Value*> m_items;
};

class Object final : public Value {
public:
    using Entry = std::pair<std::string, Value*>;

    Object() = default;
    Object(std::initializer_list<Entry> items);
    ~Object() override;

    Object* clone() const override;

    std::size_t size() const;
    std::vector<std::string> keys() const;
    void insert(const std::string& key, Value* value);
    void remove(const std::string& key);

    Value* operator[](const std::string& key) override;
    const Value* operator[](const std::string& key) const override;

    void accept(Visitor& visitor) const override;
    void accept(MutatingVisitor& visitor) override;

private:
    std::vector<Entry> m_items;
};

class RemoveNullVisitor final : public MutatingVisitor {
public:
    void visit(Integer& value) override;
    void visit(Array& value) override;
    void visit(Object& value) override;
    void visit(Null& value) override;
};

class PrintVisitor final : public Visitor {
public:
    explicit PrintVisitor(std::ostream& out);

    void visit(const Integer& value) override;
    void visit(const Array& value) override;
    void visit(const Object& value) override;
    void visit(const Null& value) override;

private:
    std::ostream& m_out;
};

std::ostream& operator<<(std::ostream& out, const Value& value);

/*
[*] Can individual elements be shared among different JSON containers (array/object) with the
mentioned design? Can you take one specific element (e.g. an integer) allocated on the heap and
place it inside an array and also inside an object?

No. With raw-pointer ownership, one heap-allocated element must not be inserted into multiple
containers. Doing so would create multiple owners and eventually double-delete the same object.

[*] Could references (or non-owned pointers) be used for storing the elements in JSON containers?
How would it affect the usability of arrays and objects? Try it and see how easy or hard it is :)

Storing references is not practical here: references cannot be reseated, cannot represent missing
values, and make replacement/removal awkward. Non-owning pointers are possible, but then ownership
has to live elsewhere, which makes arrays/objects harder to use safely.

[*] Non-indexable objects will throw an exception when you attempt to index them using operator[].
Feel free to use an exception from the standard library, but prefer creating a custom exception
if you can. Use a name you find suitable for the exception, the tests only check that an exception
is thrown, not what kind of exception it is.

I used a small custom exception type, InvalidIndexError, derived from std::logic_error. It makes
the reason for failure explicit while still behaving like a normal standard exception.

[*] Think about this interface. What are its benefits or disadvantages? Is it better to put the
indexers into the root Value element? Or should it only be supported by types that actually
implement indexing (arrays/objects)? Think about the trade-offs (compile-time safety vs ergonomics).

Putting both indexers on Value makes the interface ergonomic because code can work through a base
pointer/reference. The downside is weaker compile-time safety, because invalid indexing is a
runtime error instead of a compile-time rejection.

[*] Think about the return type. What should it be? Is std::optional required here?

Returning a pointer works well here: successful lookup/indexing returns the addressed element,
missing array/object items can return nullptr, and non-indexable values can throw. std::optional is
not required because pointer nullability already models absence.

[*] How can you implement copying for a polymorphic object hierarchy? What should be the return type
of the clone method? If you are interested, look for "C++ covariance".

Polymorphic copying is handled with a virtual clone() method overridden by each derived type.
Returning a raw pointer fits the ownership model, and derived classes can use covariant returns
(for example Array* overriding Value*).

[*] Think about the constness of the accept method and of the methods in the visitor. What
parameter type should they take? Should it be const or not? Consider creating two classes
to represent visitors, Visitor and MutatingVisitor (or similar names).

accept() is split into a const Visitor and a mutating MutatingVisitor. Printing can traverse a
const document, while transformations such as removing nulls require mutable access.

[*] How would the implementation look like if you have used e.g. algebraic data types instead?
Would you need the Visitor design pattern in that case?

With algebraic data types such as std::variant, adding operations is often done with pattern
matching / std::visit, so a separate Visitor class hierarchy is usually unnecessary.
*/
