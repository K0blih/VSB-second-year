#include "tasks.h"

#include <algorithm>
#include <ostream>

InvalidIndexError::InvalidIndexError() : std::logic_error("value is not indexable") {}

Value* Value::operator[](std::size_t) {
    throw InvalidIndexError{};
}

const Value* Value::operator[](std::size_t) const {
    throw InvalidIndexError{};
}

Value* Value::operator[](const std::string&) {
    throw InvalidIndexError{};
}

const Value* Value::operator[](const std::string&) const {
    throw InvalidIndexError{};
}

Integer::Integer(int value) : m_value(value) {}

int Integer::get_value() const {
    return m_value;
}

Integer* Integer::clone() const {
    return new Integer{m_value};
}

void Integer::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Integer::accept(MutatingVisitor& visitor) {
    visitor.visit(*this);
}

Null* Null::clone() const {
    return new Null{};
}

void Null::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Null::accept(MutatingVisitor& visitor) {
    visitor.visit(*this);
}

Array::Array(std::initializer_list<Value*> items)
    : m_items(items) {}

Array::~Array() {
    for (Value* item : m_items) {
        delete item;
    }
}

Array* Array::clone() const {
    auto* copied = new Array{};
    copied->m_items.reserve(m_items.size());
    for (const Value* item : m_items) {
        copied->append(item->clone());
    }
    return copied;
}

std::size_t Array::size() const {
    return m_items.size();
}

void Array::append(Value* value) {
    m_items.push_back(value);
}

void Array::remove(std::size_t index) {
    if (index >= m_items.size()) {
        return;
    }

    delete m_items[index];
    m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(index));
}

Value* Array::operator[](std::size_t index) {
    if (index >= m_items.size()) {
        return nullptr;
    }
    return m_items[index];
}

const Value* Array::operator[](std::size_t index) const {
    if (index >= m_items.size()) {
        return nullptr;
    }
    return m_items[index];
}

void Array::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Array::accept(MutatingVisitor& visitor) {
    visitor.visit(*this);
}

Object::Object(std::initializer_list<Entry> items) {
    m_items.reserve(items.size());
    for (const Entry& item : items) {
        insert(item.first, item.second);
    }
}

Object::~Object() {
    for (Entry& item : m_items) {
        delete item.second;
    }
}

Object* Object::clone() const {
    auto* copied = new Object{};
    copied->m_items.reserve(m_items.size());
    for (const Entry& item : m_items) {
        copied->insert(item.first, item.second->clone());
    }
    return copied;
}

std::size_t Object::size() const {
    return m_items.size();
}

std::vector<std::string> Object::keys() const {
    std::vector<std::string> result;
    result.reserve(m_items.size());
    for (const Entry& item : m_items) {
        result.push_back(item.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

void Object::insert(const std::string& key, Value* value) {
    for (Entry& item : m_items) {
        if (item.first == key) {
            delete item.second;
            item.second = value;
            return;
        }
    }
    m_items.emplace_back(key, value);
}

void Object::remove(const std::string& key) {
    for (auto it = m_items.begin(); it != m_items.end(); it++) {
        if (it->first == key) {
            delete it->second;
            m_items.erase(it);
            return;
        }
    }
}

Value* Object::operator[](const std::string& key) {
    for (Entry& item : m_items) {
        if (item.first == key) {
            return item.second;
        }
    }
    return nullptr;
}

const Value* Object::operator[](const std::string& key) const {
    for (const Entry& item : m_items) {
        if (item.first == key) {
            return item.second;
        }
    }
    return nullptr;
}

void Object::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Object::accept(MutatingVisitor& visitor) {
    visitor.visit(*this);
}

void RemoveNullVisitor::visit(Integer&) {}

void RemoveNullVisitor::visit(Null&) {}

void RemoveNullVisitor::visit(Array& value) {
    for (std::size_t i = 0; i < value.size();) {
        Value* item = value[i];
        if (dynamic_cast<Null*>(item) != nullptr) {
            value.remove(i);
            continue;
        }
        item->accept(*this);
        i++;
    }
}

void RemoveNullVisitor::visit(Object& value) {
    std::vector<std::string> to_remove;

    for (const std::string& key : value.keys()) {
        Value* item = value[key];
        if (dynamic_cast<Null*>(item) != nullptr) {
            to_remove.push_back(key);
            continue;
        }
        item->accept(*this);
    }

    for (const std::string& key : to_remove) {
        value.remove(key);
    }
}

PrintVisitor::PrintVisitor(std::ostream& out) : m_out(out) {}

void PrintVisitor::visit(const Integer& value) {
    m_out << value.get_value();
}

void PrintVisitor::visit(const Null&) {
    m_out << "null";
}

void PrintVisitor::visit(const Array& value) {
    m_out << "[";
    for (std::size_t i = 0; i < value.size(); i++) {
        if (i != 0) {
            m_out << ", ";
        }
        value[i]->accept(*this);
    }
    m_out << "]";
}

void PrintVisitor::visit(const Object& value) {
    m_out << "{";
    const std::vector<std::string> sorted_keys = value.keys();
    for (std::size_t i = 0; i < sorted_keys.size(); ++i) {
        if (i != 0) {
            m_out << ", ";
        }
        m_out << sorted_keys[i] << ": ";
        value[sorted_keys[i]]->accept(*this);
    }
    m_out << "}";
}

std::ostream& operator<<(std::ostream& out, const Value& value) {
    PrintVisitor visitor{out};
    value.accept(visitor);
    return out;
}
