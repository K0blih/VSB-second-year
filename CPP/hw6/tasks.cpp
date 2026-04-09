#include "tasks.h"

#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string_view>

namespace {
    void skip_whitespace(std::istream& is) {
        while (true) {
            int ch = is.peek();
            if (ch != ' ' && ch != '\n') {
                return;
            }
            is.get();
        }
    }

    bool consume_literal(std::istream& is, std::string_view literal) {
        for (char expected: literal) {
            int ch = is.get();
            if (ch != expected) {
                return false;
            }
        }
        return true;
    }

    std::optional<String> parse_string_value(std::istream& is);
    std::optional<Value> parse_value(std::istream& is);

    std::optional<Number> parse_number_value(std::istream& is) {
        bool negative = false;
        if (is.peek() == '-') {
            negative = true;
            is.get();
        }

        int ch = is.peek();
        if (ch < '0' || ch > '9') {
            return std::nullopt;
        }

        double integer_part = 0.0;
        while (true) {
            ch = is.peek();
            if (ch < '0' || ch > '9') {
                break;
            }
            integer_part = integer_part * 10.0 + static_cast<double>(is.get() - '0');
        }

        double value = integer_part;
        if (is.peek() == '.') {
            is.get();
            double factor = 0.1;
            while (true) {
                ch = is.peek();
                if (ch < '0' || ch > '9') {
                    break;
                }
                value += static_cast<double>(is.get() - '0') * factor;
                factor /= 10.0;
            }
        }

        return Number{negative ? -value : value};
    }

    std::optional<String> parse_string_value(std::istream& is) {
        if (is.get() != '"') {
            return std::nullopt;
        }

        std::string value;
        while (true) {
            int ch = is.get();
            if (ch == EOF) {
                return std::nullopt;
            }

            if (ch == '"') {
                return String{value};
            }

            if (ch == '\\') {
                int escaped = is.get();
                if (escaped == '"' || escaped == '\\') {
                    value.push_back(static_cast<char>(escaped));
                    continue;
                }
                return std::nullopt;
            }

            value.push_back(static_cast<char>(ch));
        }
    }

    std::optional<Array> parse_array_value(std::istream& is) {
        if (is.get() != '[') {
            return std::nullopt;
        }

        Array array;
        skip_whitespace(is);
        if (is.peek() == ']') {
            is.get();
            return array;
        }

        while (true) {
            auto item = parse_value(is);
            if (!item.has_value()) {
                return std::nullopt;
            }
            array.items.push_back(std::move(item.value()));

            skip_whitespace(is);
            int ch = is.peek();
            if (ch == ']') {
                is.get();
                return array;
            }
            if (ch != ',') {
                return std::nullopt;
            }

            is.get();
            skip_whitespace(is);
            if (is.peek() == ',' || is.peek() == ']') {
                return std::nullopt;
            }
        }
    }

    std::optional<Object> parse_object_value(std::istream& is) {
        if (is.get() != '{') {
            return std::nullopt;
        }

        Object object;
        skip_whitespace(is);
        if (is.peek() == '}') {
            is.get();
            return object;
        }

        while (true) {
            auto key = parse_string_value(is);
            if (!key.has_value()) {
                return std::nullopt;
            }

            skip_whitespace(is);
            if (is.get() != ':') {
                return std::nullopt;
            }

            auto value = parse_value(is);
            if (!value.has_value()) {
                return std::nullopt;
            }
            object.items[key->value] = std::move(value.value());

            skip_whitespace(is);
            int ch = is.peek();
            if (ch == '}') {
                is.get();
                return object;
            }
            if (ch != ',') {
                return std::nullopt;
            }

            is.get();
            skip_whitespace(is);
            if (is.peek() != '"') {
                return std::nullopt;
            }
        }
    }

    std::optional<Value> parse_value(std::istream& is) {
        skip_whitespace(is);

        int ch = is.peek();
        if (ch == EOF) {
            return std::nullopt;
        }

        if (ch == 'n') {
            return consume_literal(is, "null") ? std::optional<Value>{Null{}} : std::nullopt;
        }
        if (ch == 't') {
            return consume_literal(is, "true") ? std::optional<Value>{Boolean{true}} : std::nullopt;
        }
        if (ch == 'f') {
            return consume_literal(is, "false") ? std::optional<Value>{Boolean{false}} : std::nullopt;
        }
        if (ch == '"') {
            auto value = parse_string_value(is);
            return value.has_value() ? std::optional<Value>{std::move(value.value())} : std::nullopt;
        }
        if (ch == '[') {
            auto value = parse_array_value(is);
            return value.has_value() ? std::optional<Value>{std::move(value.value())} : std::nullopt;
        }
        if (ch == '{') {
            auto value = parse_object_value(is);
            return value.has_value() ? std::optional<Value>{std::move(value.value())} : std::nullopt;
        }
        if (ch == '-' || (ch >= '0' && ch <= '9')) {
            auto value = parse_number_value(is);
            return value.has_value() ? std::optional<Value>{value.value()} : std::nullopt;
        }

        return std::nullopt;
    }

    void print_string(std::ostream& os, const std::string& value) {
        os << '"';
        for (char ch: value) {
            if (ch == '"' || ch == '\\') {
                os << '\\';
            }
            os << ch;
        }
        os << '"';
    }

    void print_number(std::ostream& os, double value) {
        std::ostringstream ss;
        ss << std::setprecision(std::numeric_limits<double>::digits10) << value;
        std::string text = ss.str();
        if (auto dot_pos = text.find('.'); dot_pos != std::string::npos) {
            while (!text.empty() && text.back() == '0') {
                text.pop_back();
            }
            if (!text.empty() && text.back() == '.') {
                text.pop_back();
            }
        }
        os << text;
    }

    void append_bytes(std::vector<uint8_t>& out, const void* data, size_t size) {
        const auto* bytes = static_cast<const uint8_t*>(data);
        out.insert(out.end(), bytes, bytes + size);
    }

    void append_size(std::vector<uint8_t>& out, size_t value) {
        uint64_t stored = value;
        append_bytes(out, &stored, sizeof(stored));
    }

    void serialize_key(std::vector<uint8_t>& out, const std::string& key) {
        append_size(out, key.size());
        append_bytes(out, key.data(), key.size());
    }

    void serialize_into(std::vector<uint8_t>& out, const Value& value) {
        std::visit(overloaded{
                [&](const Null&) {
                    out.push_back(0);
                },
                [&](const Boolean& boolean) {
                    out.push_back(1);
                    out.push_back(boolean.value ? 1 : 0);
                },
                [&](const Number& number) {
                    out.push_back(2);
                    append_bytes(out, &number.value, sizeof(number.value));
                },
                [&](const String& string) {
                    out.push_back(3);
                    append_size(out, string.value.size());
                    append_bytes(out, string.value.data(), string.value.size());
                },
                [&](const Array& array) {
                    out.push_back(4);
                    append_size(out, array.items.size());
                    for (const auto& item: array.items) {
                        serialize_into(out, item);
                    }
                },
                [&](const Object& object) {
                    out.push_back(5);
                    append_size(out, object.items.size());
                    for (const auto& [key, item]: object.items) {
                        serialize_key(out, key);
                        serialize_into(out, item);
                    }
                }
        }, value);
    }

    template<typename T>
    T read_raw(const std::vector<uint8_t>& data, size_t& index) {
        T value{};
        std::copy_n(data.begin() + static_cast<std::ptrdiff_t>(index), sizeof(T),
                    reinterpret_cast<uint8_t*>(&value));
        index += sizeof(T);
        return value;
    }

    std::string read_string_contents(const std::vector<uint8_t>& data, size_t& index) {
        uint64_t length = read_raw<uint64_t>(data, index);
        std::string value;
        value.reserve(static_cast<size_t>(length));
        for (uint64_t i = 0; i < length; ++i) {
            value.push_back(static_cast<char>(data[index++]));
        }
        return value;
    }

    Value deserialize_from(const std::vector<uint8_t>& data, size_t& index) {
        uint8_t type = data[index++];
        switch (type) {
            case 0:
                return Null{};
            case 1:
                return Boolean{data[index++] != 0};
            case 2:
                return Number{read_raw<double>(data, index)};
            case 3:
                return String{read_string_contents(data, index)};
            case 4: {
                uint64_t length = read_raw<uint64_t>(data, index);
                Array array;
                array.items.reserve(static_cast<size_t>(length));
                for (uint64_t i = 0; i < length; ++i) {
                    array.items.push_back(deserialize_from(data, index));
                }
                return array;
            }
            case 5: {
                uint64_t length = read_raw<uint64_t>(data, index);
                Object object;
                for (uint64_t i = 0; i < length; ++i) {
                    std::string key = read_string_contents(data, index);
                    object.items[std::move(key)] = deserialize_from(data, index);
                }
                return object;
            }
            default:
                return Null{};
        }
    }
}

bool Array::operator==(const Array& other) const {
    return other.items == this->items;
}

bool Object::operator==(const Object& other) const {
    return other.items == this->items;
}

bool String::operator==(const String& other) const {
    return other.value == this->value;
}

bool Null::operator==(const Null&) const {
    return true;
}

bool Number::operator==(const Number& other) const {
    return other.value == this->value;
}

bool Boolean::operator==(const Boolean& other) const {
    return other.value == this->value;
}

std::optional<Value> parse_json(std::istream& is) {
    return parse_value(is);
}

std::ostream& operator<<(std::ostream& os, const Value& value) {
    std::visit(overloaded{
            [&](const Null&) {
                os << "null";
            },
            [&](const Boolean& boolean) {
                os << (boolean.value ? "true" : "false");
            },
            [&](const Number& number) {
                print_number(os, number.value);
            },
            [&](const String& string) {
                print_string(os, string.value);
            },
            [&](const Array& array) {
                os << '[';
                for (size_t i = 0; i < array.items.size(); ++i) {
                    if (i != 0) {
                        os << ", ";
                    }
                    os << array.items[i];
                }
                os << ']';
            },
            [&](const Object& object) {
                os << '{';
                bool first = true;
                for (const auto& [key, item]: object.items) {
                    if (!first) {
                        os << ", ";
                    }
                    first = false;
                    print_string(os, key);
                    os << ": " << item;
                }
                os << '}';
            }
    }, value);
    return os;
}

std::vector<uint8_t> serialize(const Value& value) {
    std::vector<uint8_t> out;
    serialize_into(out, value);
    return out;
}

Value deserialize(const std::vector<uint8_t>& data) {
    size_t index = 0;
    return deserialize_from(data, index);
}
