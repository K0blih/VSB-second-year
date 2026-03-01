#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

using CodePoint = uint32_t;

class UTF8String {
public:
    UTF8String() noexcept;
    UTF8String(const char* ascii);
    UTF8String(const std::string& ascii);
    UTF8String(const std::vector<CodePoint>& points);

    UTF8String(const UTF8String& other);
    UTF8String& operator=(const UTF8String& other);
    ~UTF8String();

    std::size_t get_byte_count() const noexcept;
    std::size_t get_point_count() const;

    std::optional<uint8_t> operator[](std::size_t index) const;
    std::optional<CodePoint> nth_code_point(std::size_t index) const;

    void append(char ch);
    void append(CodePoint point);

    UTF8String& operator+=(const UTF8String& rhs);
    explicit operator std::string() const;

private:
    void ensure_capacity(std::size_t required);
    void append_byte(uint8_t byte);
    static std::size_t encoded_size(CodePoint point);
    static std::size_t decode_at(const uint8_t* data, std::size_t remaining, CodePoint& out);

    uint8_t* m_data;
    std::size_t m_size;
    std::size_t m_capacity;
};

UTF8String operator+(const UTF8String& lhs, const UTF8String& rhs);
bool operator==(const UTF8String& lhs, const UTF8String& rhs);
bool operator!=(const UTF8String& lhs, const UTF8String& rhs);
