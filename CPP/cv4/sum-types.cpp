#include <iostream>
#include <utility>
#include <variant>
#include <vector>

/*
 * Algebraic data types (aka sum types) are an approach for building type hierarchies.
 * They are designed for cases where you have a closed set of types, but an open set of operations,
 * i.e. when you want to be able to add new operations to a fixed set of types easily.
 * They are not very ergonomic in C++, but they are usable.
 *
 * Algebraic data type (or a sum type, or a tagged union) is a type that can hold the value of exactly
 * one of N types (variants) at a given moment.
 * It is basically the same thing as C/C++ union, but with an additional "tag", which holds the information
 * about what union variant is currently active.
 *
 * For example, you can say that a Message is either a Header message or a Data message. And then
 * implement various operations for both cases.
*/

// You could implement them using unions, but it's incredibly dangerous and verbose, because unions
// in C++ can lead to UB very quickly.
struct Header {
    uint32_t client_address;
};
struct Data {
    std::vector<int8_t> bytes;
};
enum class MessageType {
    Header,
    Data
};

struct Message {
public:
    static Message Header(uint32_t client_address) {
        return Message{client_address};
    }
    static Message Data(std::vector<int8_t> bytes) {
        return Message{std::move(bytes)};
    }

    ~Message() {
        if (this->type == MessageType::Header) {
            this->header.~Header();
        } else if (type == MessageType::Data) {
            this->data.~Data();
        }
    }
    Message(const Message& other) = delete;
    Message& operator=(Message other) = delete;

    [[nodiscard]] MessageType get_type() const {
        return this->type;
    }

    [[nodiscard]] ::Header& as_header() {
        return this->header;
    }
    [[nodiscard]] const ::Header& as_header() const {
        return this->header;
    }
    [[nodiscard]] ::Data& as_data() {
        return this->data;
    }
    [[nodiscard]] const ::Data& as_data() const {
        return this->data;
    }

private:
    explicit Message(uint32_t client_address): type(MessageType::Header) {
        new (&this->header) ::Header{client_address};
    }
    explicit Message(std::vector<int8_t> bytes): type(MessageType::Data) {
        new (&this->data) ::Data{std::move(bytes)};
    }

    union {
        ::Header header;
        ::Data data;
    };
    MessageType type;
};

void adt_union() {
    auto msg1 = Message::Header(1);

    if (msg1.get_type() == MessageType::Header) {
        std::cout << msg1.as_header().client_address << std::endl;
        // std::cout << msg1.as_data().bytes.size() << std::endl; // UB!
    }

    auto msg2 = Message::Data({1, 2, 3});
    if (msg1.get_type() == MessageType::Data) {
        std::cout << msg1.as_data().bytes.size() << std::endl;
    }
}

// You can implement the above thing in a much more reasonable and safer way using
// std::variant:
using MessageVariant = std::variant<Header, Data>;
// ^ This says: MessageVariant is either a Header or Data.
// Its data is stored in-line, there is no additional indirection or heap allocation.
// Note that the combination of Header and Data is done outside of these classes, they know nothing
// about it. Therefore, I can create an arbitrary amount of different variant types that use Header
// or Data.

// Sadly, these two lines are required to use `std::visit` ergonomically...
// We'll explain variadic templates later.
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void adt_variant() {
    MessageVariant v1 = Header{1};
    std::cout << "Variant index: " << v1.index() << std::endl;

    if (std::holds_alternative<Header>(v1)) {
        std::cout << "v1 is Header!" << std::endl;
    }
    std::cout << std::get<Header>(v1).client_address << std::endl;
    // std::cout << std::get<Data>(v1).bytes.size() << std::endl; // UB!

    v1 = Data{{1, 2, 3}};

    // Matching on the type is a bit annoying, but can be done using `std::visit`:
    std::visit(overloaded {
            [](const Data& data) { std::cout << data.bytes.size() << std::endl; },
            [](const Header& header) { std::cout << header.client_address << std::endl; },
    }, v1);

    // You can also use `std::visit` as an expression:
    auto id = std::visit(overloaded {
            [](const Data& data) { return 1; },
            [](const Header& header) { return 2; },
    }, v1);
    std::cout << id << std::endl;
}

struct PrintMessage {
    // We define the visiting class by overloading the call operator ()
    void operator()(const Data& msg) {
        std::cout << "Data" << std::endl;
    }
    void operator()(const Header& header) {
        std::cout << "Header" << std::endl;
    }
};

struct GetValue {
    int operator()(const Data& msg) {
        return 1;
    }
    int operator()(const Header& header) {
        return 2;
    }
};

void visit_variant_with_class() {
    // You can also create class-based visit functions for std::variants
    MessageVariant msg = Header{};
    std::visit(PrintMessage{}, msg);

    // It can also return values
    int value = std::visit(GetValue{}, msg);
    std::cout << value << std::endl;
}

int main() {
//    adt_union();
//    adt_variant();
    visit_variant_with_class();

    return 0;
}
