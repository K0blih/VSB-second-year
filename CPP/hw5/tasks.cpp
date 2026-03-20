#include "tasks.h"

#include <stdexcept>

UTF8String::UTF8String(const char* ascii) {
    if (ascii == nullptr) {
        return;
    }

    while (*ascii != '\0') {
        m_bytes.push_back(static_cast<uint8_t>(*ascii));
        ascii++;
    }
}

UTF8String::UTF8String(const std::string& ascii) : m_bytes(ascii.begin(), ascii.end()) {}

UTF8String::UTF8String(const std::vector<CodePoint>& points) {
    for (CodePoint point : points) {
        append(point);
    }
}

UTF8String::UTF8String(const std::vector<uint8_t>& bytes) : m_bytes(bytes) {}

UTF8String::UTF8String(UTF8String&& other) noexcept : m_bytes(std::move(other.m_bytes)) {
    other.m_bytes.clear();
}

std::size_t UTF8String::get_byte_count() const noexcept {
    return m_bytes.size();
}

std::size_t UTF8String::get_point_count() const {
    std::size_t count = 0;
    std::size_t pos = 0;

    while (pos < m_bytes.size()) {
        CodePoint point = 0;
        const std::size_t consumed = decode_at(m_bytes, pos, point);
        if (consumed == 0) {
            break;
        }
        pos += consumed;
        count++;
    }

    return count;
}

std::optional<uint8_t> UTF8String::operator[](std::size_t index) const {
    if (index >= m_bytes.size()) {
        return std::nullopt;
    }

    return m_bytes[index];
}

std::optional<CodePoint> UTF8String::nth_code_point(std::size_t index) const {
    std::size_t current = 0;
    std::size_t pos = 0;

    while (pos < m_bytes.size()) {
        CodePoint point = 0;
        const std::size_t consumed = decode_at(m_bytes, pos, point);
        if (consumed == 0) {
            return std::nullopt;
        }
        if (current == index) {
            return point;
        }
        pos += consumed;
        current++;
    }

    return std::nullopt;
}

void UTF8String::append(char ch) {
    m_bytes.push_back(static_cast<uint8_t>(ch));
}

void UTF8String::append(CodePoint point) {
    const std::size_t count = encoded_size(point);
    if (count == 1) {
        m_bytes.push_back(static_cast<uint8_t>(point));
    } else if (count == 2) {
        m_bytes.push_back(static_cast<uint8_t>(0xC0U | ((point >> 6U) & 0x1FU)));
        m_bytes.push_back(static_cast<uint8_t>(0x80U | (point & 0x3FU)));
    } else if (count == 3) {
        m_bytes.push_back(static_cast<uint8_t>(0xE0U | ((point >> 12U) & 0x0FU)));
        m_bytes.push_back(static_cast<uint8_t>(0x80U | ((point >> 6U) & 0x3FU)));
        m_bytes.push_back(static_cast<uint8_t>(0x80U | (point & 0x3FU)));
    } else {
        m_bytes.push_back(static_cast<uint8_t>(0xF0U | ((point >> 18U) & 0x07U)));
        m_bytes.push_back(static_cast<uint8_t>(0x80U | ((point >> 12U) & 0x3FU)));
        m_bytes.push_back(static_cast<uint8_t>(0x80U | ((point >> 6U) & 0x3FU)));
        m_bytes.push_back(static_cast<uint8_t>(0x80U | (point & 0x3FU)));
    }
}

UTF8String& UTF8String::operator+=(const UTF8String& rhs) {
    if (this == &rhs) {
        UTF8String copy(rhs);
        return *this += copy;
    }

    m_bytes.insert(m_bytes.end(), rhs.m_bytes.begin(), rhs.m_bytes.end());
    return *this;
}

UTF8String& UTF8String::operator=(UTF8String&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    m_bytes = std::move(other.m_bytes);
    other.m_bytes.clear();
    return *this;
}

UTF8String::operator std::string() const {
    return std::string(m_bytes.begin(), m_bytes.end());
}

UTF8String::ByteRange UTF8String::bytes() {
    return ByteRange(this);
}

UTF8String::ByteRange UTF8String::bytes() const {
    return ByteRange(this);
}

UTF8String::CodePointRange UTF8String::codepoints() {
    return CodePointRange(this);
}

UTF8String::CodePointRange UTF8String::codepoints() const {
    return CodePointRange(this);
}

std::size_t UTF8String::encoded_size(CodePoint point) {
    if (point <= 0x7FU) {
        return 1;
    }
    if (point <= 0x7FFU) {
        return 2;
    }
    if (point <= 0xFFFFU) {
        return 3;
    }
    return 4;
}

std::size_t UTF8String::decode_at(const std::vector<uint8_t>& data, std::size_t pos, CodePoint& out) {
    if (pos >= data.size()) {
        return 0;
    }

    const uint8_t b0 = data[pos];
    if ((b0 & 0x80U) == 0) {
        out = b0;
        return 1;
    }

    if ((b0 & 0xE0U) == 0xC0U) {
        if (pos + 1 >= data.size()) {
            return 0;
        }
        out = ((static_cast<CodePoint>(b0 & 0x1FU)) << 6U)
            | static_cast<CodePoint>(data[pos + 1] & 0x3FU);
        return 2;
    }

    if ((b0 & 0xF0U) == 0xE0U) {
        if (pos + 2 >= data.size()) {
            return 0;
        }
        out = ((static_cast<CodePoint>(b0 & 0x0FU)) << 12U)
            | ((static_cast<CodePoint>(data[pos + 1] & 0x3FU)) << 6U)
            | static_cast<CodePoint>(data[pos + 2] & 0x3FU);
        return 3;
    }

    if ((b0 & 0xF8U) == 0xF0U) {
        if (pos + 3 >= data.size()) {
            return 0;
        }
        out = ((static_cast<CodePoint>(b0 & 0x07U)) << 18U)
            | ((static_cast<CodePoint>(data[pos + 1] & 0x3FU)) << 12U)
            | ((static_cast<CodePoint>(data[pos + 2] & 0x3FU)) << 6U)
            | static_cast<CodePoint>(data[pos + 3] & 0x3FU);
        return 4;
    }

    return 0;
}

std::size_t UTF8String::previous_code_point_start(const std::vector<uint8_t>& data, std::size_t pos) {
    if (pos == 0) {
        return 0;
    }

    std::size_t current = pos - 1;
    while (current > 0 && (data[current] & 0xC0U) == 0x80U) {
        current--;
    }
    return current;
}

UTF8String::ByteIterator::ByteIterator(const UTF8String* owner, std::size_t index) : m_owner(owner), m_index(index) {}

uint8_t UTF8String::ByteIterator::operator*() const {
    return m_owner->m_bytes[m_index];
}

UTF8String::ByteIterator& UTF8String::ByteIterator::operator++() {
    m_index++;
    return *this;
}

UTF8String::ByteIterator UTF8String::ByteIterator::operator++(int) {
    ByteIterator copy = *this;
    ++(*this);
    return copy;
}

UTF8String::ByteIterator& UTF8String::ByteIterator::operator--() {
    m_index--;
    return *this;
}

UTF8String::ByteIterator UTF8String::ByteIterator::operator--(int) {
    ByteIterator copy = *this;
    --(*this);
    return copy;
}

UTF8String::ByteIterator& UTF8String::ByteIterator::operator+=(difference_type offset) {
    m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) + offset);
    return *this;
}

UTF8String::ByteIterator& UTF8String::ByteIterator::operator-=(difference_type offset) {
    return *this += -offset;
}

UTF8String::ByteIterator operator+(UTF8String::ByteIterator it, UTF8String::ByteIterator::difference_type offset) {
    it += offset;
    return it;
}

UTF8String::ByteIterator operator+(UTF8String::ByteIterator::difference_type offset, UTF8String::ByteIterator it) {
    it += offset;
    return it;
}

UTF8String::ByteIterator operator-(UTF8String::ByteIterator it, UTF8String::ByteIterator::difference_type offset) {
    it -= offset;
    return it;
}

UTF8String::ByteIterator::difference_type operator-(const UTF8String::ByteIterator& lhs, const UTF8String::ByteIterator& rhs) {
    return static_cast<UTF8String::ByteIterator::difference_type>(lhs.m_index)
        - static_cast<UTF8String::ByteIterator::difference_type>(rhs.m_index);
}

bool operator==(const UTF8String::ByteIterator& lhs, const UTF8String::ByteIterator& rhs) {
    return lhs.m_owner == rhs.m_owner && lhs.m_index == rhs.m_index;
}

bool operator!=(const UTF8String::ByteIterator& lhs, const UTF8String::ByteIterator& rhs) {
    return !(lhs == rhs);
}

bool operator<(const UTF8String::ByteIterator& lhs, const UTF8String::ByteIterator& rhs) {
    return lhs.m_index < rhs.m_index;
}

bool operator>(const UTF8String::ByteIterator& lhs, const UTF8String::ByteIterator& rhs) {
    return rhs < lhs;
}

bool operator<=(const UTF8String::ByteIterator& lhs, const UTF8String::ByteIterator& rhs) {
    return !(rhs < lhs);
}

bool operator>=(const UTF8String::ByteIterator& lhs, const UTF8String::ByteIterator& rhs) {
    return !(lhs < rhs);
}

UTF8String::ByteRange::ByteRange(const UTF8String* owner) : m_owner(owner) {}

UTF8String::ByteIterator UTF8String::ByteRange::begin() const {
    return ByteIterator(m_owner, 0);
}

UTF8String::ByteIterator UTF8String::ByteRange::end() const {
    return ByteIterator(m_owner, m_owner->m_bytes.size());
}

UTF8String::CodePointIterator::CodePointIterator(const UTF8String* owner, std::size_t byte_index) : m_owner(owner), m_byte_index(byte_index) {}

CodePoint UTF8String::CodePointIterator::operator*() const {
    CodePoint point = 0;
    decode_at(m_owner->m_bytes, m_byte_index, point);
    return point;
}

UTF8String::CodePointIterator& UTF8String::CodePointIterator::operator++() {
    CodePoint point = 0;
    m_byte_index += decode_at(m_owner->m_bytes, m_byte_index, point);
    return *this;
}

UTF8String::CodePointIterator UTF8String::CodePointIterator::operator++(int) {
    CodePointIterator copy = *this;
    ++(*this);
    return copy;
}

UTF8String::CodePointIterator& UTF8String::CodePointIterator::operator--() {
    m_byte_index = previous_code_point_start(m_owner->m_bytes, m_byte_index);
    return *this;
}

UTF8String::CodePointIterator UTF8String::CodePointIterator::operator--(int) {
    CodePointIterator copy = *this;
    --(*this);
    return copy;
}

bool operator==(const UTF8String::CodePointIterator& lhs, const UTF8String::CodePointIterator& rhs) {
    return lhs.m_owner == rhs.m_owner && lhs.m_byte_index == rhs.m_byte_index;
}

bool operator!=(const UTF8String::CodePointIterator& lhs, const UTF8String::CodePointIterator& rhs) {
    return !(lhs == rhs);
}

UTF8String::CodePointRange::CodePointRange(const UTF8String* owner) : m_owner(owner) {}

UTF8String::CodePointIterator UTF8String::CodePointRange::begin() const {
    return CodePointIterator(m_owner, 0);
}

UTF8String::CodePointIterator UTF8String::CodePointRange::end() const {
    return CodePointIterator(m_owner, m_owner->m_bytes.size());
}

UTF8String operator+(const UTF8String& lhs, const UTF8String& rhs) {
    UTF8String result(lhs);
    result += rhs;
    return result;
}

bool operator==(const UTF8String& lhs, const UTF8String& rhs) {
    if (lhs.get_byte_count() != rhs.get_byte_count()) {
        return false;
    }

    for (std::size_t i = 0; i < lhs.get_byte_count(); ++i) {
        if (lhs[i].value() != rhs[i].value()) {
            return false;
        }
    }

    return true;
}

bool operator!=(const UTF8String& lhs, const UTF8String& rhs) {
    return !(lhs == rhs);
}

Tree::Tree(int value) : m_value(std::make_shared<BigData>(value)) {}

Tree::Tree(std::shared_ptr<BigData> value) : m_value(std::move(value)) {}

BigData& Tree::get_value() const {
    return *m_value;
}

Tree* Tree::get_parent() const {
    return m_parent;
}

bool Tree::has_parent() const {
    return m_parent != nullptr;
}

Tree* Tree::get_left_child() const {
    return m_left_child.get();
}

Tree* Tree::get_right_child() const {
    return m_right_child.get();
}

Tree* Tree::get_root() {
    Tree* current = this;
    while (current->m_parent != nullptr) {
        current = current->m_parent;
    }
    return current;
}

const Tree* Tree::get_root() const {
    const Tree* current = this;
    while (current->m_parent != nullptr) {
        current = current->m_parent;
    }
    return current;
}

std::unique_ptr<Tree> Tree::take_left_child() {
    if (m_left_child) {
        m_left_child->m_parent = nullptr;
    }
    return std::move(m_left_child);
}

std::unique_ptr<Tree> Tree::take_right_child() {
    if (m_right_child) {
        m_right_child->m_parent = nullptr;
    }
    return std::move(m_right_child);
}

std::unique_ptr<Tree> Tree::take_child(Tree& child) {
    if (m_left_child.get() == &child) {
        return take_left_child();
    }
    if (m_right_child.get() == &child) {
        return take_right_child();
    }
    throw std::runtime_error("node is not a child of this tree");
}

std::unique_ptr<Tree> Tree::set_left_child(std::unique_ptr<Tree> child) {
    if (child) {
        child->m_parent = this;
    }

    std::unique_ptr<Tree> previous = std::move(m_left_child);
    if (previous) {
        previous->m_parent = nullptr;
    }

    m_left_child = std::move(child);
    return previous;
}

std::unique_ptr<Tree> Tree::set_right_child(std::unique_ptr<Tree> child) {
    if (child) {
        child->m_parent = this;
    }

    std::unique_ptr<Tree> previous = std::move(m_right_child);
    if (previous) {
        previous->m_parent = nullptr;
    }

    m_right_child = std::move(child);
    return previous;
}

void Tree::swap_children() {
    m_left_child.swap(m_right_child);
}

bool Tree::is_same_tree_as(Tree* other) const {
    return other != nullptr && get_root() == other->get_root();
}

void Tree::replace_value(std::shared_ptr<BigData> value) {
    m_value = value;
    if (m_left_child) {
        m_left_child->replace_value(value);
    }
    if (m_right_child) {
        m_right_child->replace_value(value);
    }
}

Tree::Iterator Tree::begin() {
    return Iterator(leftmost(this));
}

Tree::Iterator Tree::end() {
    return Iterator(nullptr);
}

Tree::Iterator Tree::begin() const {
    return Iterator(const_cast<Tree*>(leftmost(this)));
}

Tree::Iterator Tree::end() const {
    return Iterator(nullptr);
}

Tree* Tree::leftmost(Tree* node) {
    if (node == nullptr) {
        return nullptr;
    }
    while (node->m_left_child) {
        node = node->m_left_child.get();
    }
    return node;
}

const Tree* Tree::leftmost(const Tree* node) {
    if (node == nullptr) {
        return nullptr;
    }
    while (node->m_left_child) {
        node = node->m_left_child.get();
    }
    return node;
}

Tree::Iterator::Iterator(Tree* current) : m_current(current) {}

Tree& Tree::Iterator::operator*() const {
    return *m_current;
}

Tree* Tree::Iterator::operator->() const {
    return m_current;
}

Tree::Iterator& Tree::Iterator::operator++() {
    if (m_current == nullptr) {
        return *this;
    }

    if (m_current->m_right_child) {
        m_current = Tree::leftmost(m_current->m_right_child.get());
        return *this;
    }

    Tree* child = m_current;
    Tree* parent = m_current->m_parent;
    while (parent != nullptr && parent->m_right_child.get() == child) {
        child = parent;
        parent = parent->m_parent;
    }

    m_current = parent;
    return *this;
}

Tree::Iterator Tree::Iterator::operator++(int) {
    Iterator copy = *this;
    ++(*this);
    return copy;
}

bool operator==(const Tree::Iterator& lhs, const Tree::Iterator& rhs) {
    return lhs.m_current == rhs.m_current;
}

bool operator!=(const Tree::Iterator& lhs, const Tree::Iterator& rhs) {
    return !(lhs == rhs);
}
