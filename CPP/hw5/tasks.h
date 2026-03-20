#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using CodePoint = uint32_t;

class UTF8String {
public:
    class ByteIterator;
    class ByteRange;
    class CodePointIterator;
    class CodePointRange;

    UTF8String() = default;
    UTF8String(const char* ascii);
    UTF8String(const std::string& ascii);
    UTF8String(const std::vector<CodePoint>& points);
    UTF8String(const std::vector<uint8_t>& bytes);

    UTF8String(const UTF8String&) = default;
    UTF8String(UTF8String&& other) noexcept;
    UTF8String& operator=(const UTF8String&) = default;
    UTF8String& operator=(UTF8String&& other) noexcept;
    ~UTF8String() = default;

    std::size_t get_byte_count() const noexcept;
    std::size_t get_point_count() const;

    std::optional<uint8_t> operator[](std::size_t index) const;
    std::optional<CodePoint> nth_code_point(std::size_t index) const;

    void append(char ch);
    void append(CodePoint point);

    UTF8String& operator+=(const UTF8String& rhs);
    explicit operator std::string() const;

    ByteRange bytes();
    ByteRange bytes() const;
    CodePointRange codepoints();
    CodePointRange codepoints() const;

private:
    friend class ByteIterator;
    friend class ByteRange;
    friend class CodePointIterator;
    friend class CodePointRange;

    static std::size_t encoded_size(CodePoint point);
    static std::size_t decode_at(const std::vector<uint8_t>& data, std::size_t pos, CodePoint& out);
    static std::size_t previous_code_point_start(const std::vector<uint8_t>& data, std::size_t pos);

    std::vector<uint8_t> m_bytes;
};

class UTF8String::ByteIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = uint8_t;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = uint8_t;

    ByteIterator() = default;
    ByteIterator(const UTF8String* owner, std::size_t index);

    uint8_t operator*() const;

    ByteIterator& operator++();
    ByteIterator operator++(int);
    ByteIterator& operator--();
    ByteIterator operator--(int);

    ByteIterator& operator+=(difference_type offset);
    ByteIterator& operator-=(difference_type offset);

    friend ByteIterator operator+(ByteIterator it, difference_type offset);
    friend ByteIterator operator+(difference_type offset, ByteIterator it);
    friend ByteIterator operator-(ByteIterator it, difference_type offset);
    friend difference_type operator-(const ByteIterator& lhs, const ByteIterator& rhs);

    friend bool operator==(const ByteIterator& lhs, const ByteIterator& rhs);
    friend bool operator!=(const ByteIterator& lhs, const ByteIterator& rhs);
    friend bool operator<(const ByteIterator& lhs, const ByteIterator& rhs);
    friend bool operator>(const ByteIterator& lhs, const ByteIterator& rhs);
    friend bool operator<=(const ByteIterator& lhs, const ByteIterator& rhs);
    friend bool operator>=(const ByteIterator& lhs, const ByteIterator& rhs);

private:
    const UTF8String* m_owner = nullptr;
    std::size_t m_index = 0;
};

class UTF8String::ByteRange {
public:
    explicit ByteRange(const UTF8String* owner);

    ByteIterator begin() const;
    ByteIterator end() const;

private:
    const UTF8String* m_owner;
};

class UTF8String::CodePointIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = CodePoint;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = CodePoint;

    CodePointIterator() = default;
    CodePointIterator(const UTF8String* owner, std::size_t byte_index);

    CodePoint operator*() const;

    CodePointIterator& operator++();
    CodePointIterator operator++(int);
    CodePointIterator& operator--();
    CodePointIterator operator--(int);

    friend bool operator==(const CodePointIterator& lhs, const CodePointIterator& rhs);
    friend bool operator!=(const CodePointIterator& lhs, const CodePointIterator& rhs);

private:
    const UTF8String* m_owner = nullptr;
    std::size_t m_byte_index = 0;
};

class UTF8String::CodePointRange {
public:
    explicit CodePointRange(const UTF8String* owner);

    CodePointIterator begin() const;
    CodePointIterator end() const;

private:
    const UTF8String* m_owner;
};

UTF8String operator+(const UTF8String& lhs, const UTF8String& rhs);
bool operator==(const UTF8String& lhs, const UTF8String& rhs);
bool operator!=(const UTF8String& lhs, const UTF8String& rhs);

// Big data that we cannot afford to copy
struct BigData {
    explicit BigData(int value): value(value) {}

    BigData(const BigData&) = delete;
    BigData& operator=(const BigData&) = delete;

    int value;
};

class Tree {
public:
    class Iterator;

    explicit Tree(int value);
    explicit Tree(std::shared_ptr<BigData> value);

    BigData& get_value() const;

    Tree* get_parent() const;
    bool has_parent() const;
    Tree* get_left_child() const;
    Tree* get_right_child() const;
    Tree* get_root();
    const Tree* get_root() const;

    std::unique_ptr<Tree> take_left_child();
    std::unique_ptr<Tree> take_right_child();
    std::unique_ptr<Tree> take_child(Tree& child);

    std::unique_ptr<Tree> set_left_child(std::unique_ptr<Tree> child);
    std::unique_ptr<Tree> set_right_child(std::unique_ptr<Tree> child);

    void swap_children();
    bool is_same_tree_as(Tree* other) const;
    void replace_value(std::shared_ptr<BigData> value);

    Iterator begin();
    Iterator end();
    Iterator begin() const;
    Iterator end() const;

private:
    friend class Iterator;

    static Tree* leftmost(Tree* node);
    static const Tree* leftmost(const Tree* node);

    std::shared_ptr<BigData> m_value;
    std::unique_ptr<Tree> m_left_child;
    std::unique_ptr<Tree> m_right_child;

    // Pointer choice answer:
    // value -> shared_ptr (shared across nodes)
    // children -> unique_ptr (owned exclusively by parent)
    // parent -> raw observing pointer (non-owning backlink)
    Tree* m_parent = nullptr;
};

class Tree::Iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Tree;
    using difference_type = std::ptrdiff_t;
    using pointer = Tree*;
    using reference = Tree&;

    Iterator() = default;
    explicit Iterator(Tree* current);

    Tree& operator*() const;
    Tree* operator->() const;

    Iterator& operator++();
    Iterator operator++(int);

    friend bool operator==(const Iterator& lhs, const Iterator& rhs);
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs);

private:
    Tree* m_current = nullptr;
};
