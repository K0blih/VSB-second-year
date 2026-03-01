#include "tasks.h"

#include <algorithm>
#include <cstring>

UTF8String::UTF8String() noexcept : m_data(nullptr), m_size(0), m_capacity(0) {}

UTF8String::UTF8String(const char* ascii) : UTF8String() {
    if (ascii == nullptr) {
        return;
    }

    const std::size_t len = std::strlen(ascii);
    ensure_capacity(len);
    for (std::size_t i = 0; i < len; i++) {
        m_data[i] = static_cast<uint8_t>(ascii[i]);
    }
    m_size = len;
}

UTF8String::UTF8String(const std::string& ascii) : UTF8String() {
    const std::size_t len = ascii.size();
    ensure_capacity(len);
    for (std::size_t i = 0; i < len; i++) {
        m_data[i] = static_cast<uint8_t>(ascii[i]);
    }
    m_size = len;
}

UTF8String::UTF8String(const std::vector<CodePoint>& points) : UTF8String() {
    for (CodePoint point : points) {
        append(point);
    }
}

UTF8String::UTF8String(const UTF8String& other) : m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity) {
    if (m_capacity > 0) {
        m_data = new uint8_t[m_capacity];
        if (m_size > 0) {
            std::memcpy(m_data, other.m_data, m_size);
        }
    }
}

UTF8String& UTF8String::operator=(const UTF8String& other) {
    if (this == &other) {
        return *this;
    }

    uint8_t* new_data = nullptr;
    if (other.m_capacity > 0) {
        new_data = new uint8_t[other.m_capacity];
        if (other.m_size > 0) {
            std::memcpy(new_data, other.m_data, other.m_size);
        }
    }

    delete[] m_data;
    m_data = new_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    return *this;
}

UTF8String::~UTF8String() {
    delete[] m_data;
}

std::size_t UTF8String::get_byte_count() const noexcept {
    return m_size;
}

std::size_t UTF8String::get_point_count() const {
    std::size_t count = 0;
    std::size_t pos = 0;

    while (pos < m_size) {
        CodePoint point = 0;
        const std::size_t consumed = decode_at(m_data + pos, m_size - pos, point);
        if (consumed == 0) {
            break;
        }
        ++count;
        pos += consumed;
    }

    return count;
}

std::optional<uint8_t> UTF8String::operator[](std::size_t index) const {
    if (index >= m_size) {
        return std::nullopt;
    }
    return m_data[index];
}

std::optional<CodePoint> UTF8String::nth_code_point(std::size_t index) const {
    std::size_t current = 0;
    std::size_t pos = 0;

    while (pos < m_size) {
        CodePoint point = 0;
        const std::size_t consumed = decode_at(m_data + pos, m_size - pos, point);
        if (consumed == 0) {
            return std::nullopt;
        }
        if (current == index) {
            return point;
        }

        ++current;
        pos += consumed;
    }

    return std::nullopt;
}

void UTF8String::append(char ch) {
    append_byte(static_cast<uint8_t>(ch));
}

void UTF8String::append(CodePoint point) {
    const std::size_t count = encoded_size(point);
    ensure_capacity(m_size + count);

    if (count == 1) {
        m_data[m_size++] = static_cast<uint8_t>(point & 0x7F);
    } else if (count == 2) {
        m_data[m_size++] = static_cast<uint8_t>(0xC0U | ((point >> 6U) & 0x1FU));
        m_data[m_size++] = static_cast<uint8_t>(0x80U | (point & 0x3FU));
    } else if (count == 3) {
        m_data[m_size++] = static_cast<uint8_t>(0xE0U | ((point >> 12U) & 0x0FU));
        m_data[m_size++] = static_cast<uint8_t>(0x80U | ((point >> 6U) & 0x3FU));
        m_data[m_size++] = static_cast<uint8_t>(0x80U | (point & 0x3FU));
    } else {
        m_data[m_size++] = static_cast<uint8_t>(0xF0U | ((point >> 18U) & 0x07U));
        m_data[m_size++] = static_cast<uint8_t>(0x80U | ((point >> 12U) & 0x3FU));
        m_data[m_size++] = static_cast<uint8_t>(0x80U | ((point >> 6U) & 0x3FU));
        m_data[m_size++] = static_cast<uint8_t>(0x80U | (point & 0x3FU));
    }
}

UTF8String& UTF8String::operator+=(const UTF8String& rhs) {
    if (rhs.m_size == 0) {
        return *this;
    }

    if (this == &rhs) {
        UTF8String copy(rhs);
        return (*this += copy);
    }

    const std::size_t old_size = m_size;
    ensure_capacity(m_size + rhs.m_size);
    std::memcpy(m_data + old_size, rhs.m_data, rhs.m_size);
    m_size += rhs.m_size;
    return *this;
}

UTF8String::operator std::string() const {
    std::string out;
    out.reserve(m_size);

    for (std::size_t i = 0; i < m_size; i++) {
        out.push_back(static_cast<char>(m_data[i]));
    }

    return out;
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

    const std::size_t size = lhs.get_byte_count();
    for (std::size_t i = 0; i < size; ++i) {
        if (lhs[i].value() != rhs[i].value()) {
            return false;
        }
    }

    return true;
}

bool operator!=(const UTF8String& lhs, const UTF8String& rhs) {
    return !(lhs == rhs);
}

void UTF8String::ensure_capacity(std::size_t required) {
    if (required <= m_capacity) {
        return;
    }

    std::size_t new_capacity = std::max<std::size_t>(1, m_capacity);
    while (new_capacity < required) {
        new_capacity *= 2;
    }

    uint8_t* new_data = new uint8_t[new_capacity];
    if (m_size > 0) {
        std::memcpy(new_data, m_data, m_size);
    }

    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

void UTF8String::append_byte(uint8_t byte) {
    ensure_capacity(m_size + 1);
    m_data[m_size++] = byte;
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

std::size_t UTF8String::decode_at(const uint8_t* data, std::size_t remaining, CodePoint& out) {
    if (remaining == 0) {
        return 0;
    }

    const uint8_t b0 = data[0];
    if ((b0 & 0x80U) == 0) {
        out = b0;
        return 1;
    }

    if ((b0 & 0xE0U) == 0xC0U) {
        if (remaining < 2) {
            return 0;
        }
        out = ((static_cast<CodePoint>(b0 & 0x1FU)) << 6U) | static_cast<CodePoint>(data[1] & 0x3FU);
        return 2;
    }

    if ((b0 & 0xF0U) == 0xE0U) {
        if (remaining < 3) {
            return 0;
        }
        out = ((static_cast<CodePoint>(b0 & 0x0FU)) << 12U)
              | ((static_cast<CodePoint>(data[1] & 0x3FU)) << 6U)
              | static_cast<CodePoint>(data[2] & 0x3FU);
        return 3;
    }

    if ((b0 & 0xF8U) == 0xF0U) {
        if (remaining < 4) {
            return 0;
        }
        out = ((static_cast<CodePoint>(b0 & 0x07U)) << 18U)
              | ((static_cast<CodePoint>(data[1] & 0x3FU)) << 12U)
              | ((static_cast<CodePoint>(data[2] & 0x3FU)) << 6U)
              | static_cast<CodePoint>(data[3] & 0x3FU);
        return 4;
    }

    return 0;
}
