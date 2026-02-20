#include "tasks.h"

namespace math
{
    int fibonacci(int n)
    {
        if (n == 0)
        {
            return 0;
        }
        if (n == 1)
        {
            return 1;
        }

        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

namespace strutils
{
    std::string camel_to_snake(const std::string &input)
    {
        std::string result = "";

        for (std::size_t i = 0; i < input.size(); i++)
        {
            char c = input[i];

            if (std::isupper(c))
            {
                if (i != 0)
                {
                    result += '_';
                }

                result += (c - 'A' + 'a');
            }
            else
            {
                result += c;
            }
        }

        return result;
    }

    bool parse_uint(const std::string &input, std::uint32_t &result)
    {
        if (input == "")
        {
            return false;
        }

        std::uint32_t value = 0;
        std::uint32_t max_u32 = std::numeric_limits<std::uint32_t>::max();

        for (char ch : input)
        {
            if (ch < '0' || ch > '9')
            {
                return false;
            }

            std::uint32_t digit = static_cast<std::uint32_t>(ch - '0');

            if (value > max_u32 / 10)
            {
                return false;
            }
            value *= 10;
            if (value > max_u32 - digit)
            {
                return false;
            }
            value += digit;
        }

        result = value;
        return true;
    }

    bool validate_utf8(const std::vector<std::uint8_t> &bytes, std::size_t &codepoints)
    {
        std::size_t i = 0;
        std::size_t count = 0;

        while (i < bytes.size())
        {
            std::uint8_t b0 = bytes[i];

            if ((b0 & 0x80u) == 0)
            {
                count++;
                i++;
                continue;
            }

            std::size_t len = 0;
            std::uint32_t cp = 0;

            if ((b0 & 0xE0u) == 0xC0u)
            {
                len = 2;
                cp = static_cast<std::uint32_t>(b0 & 0x1Fu);
            }
            else if ((b0 & 0xF0u) == 0xE0u)
            {
                len = 3;
                cp = static_cast<std::uint32_t>(b0 & 0x0Fu);
            }
            else if ((b0 & 0xF8u) == 0xF0u)
            {
                len = 4;
                cp = static_cast<std::uint32_t>(b0 & 0x07u);
            }
            else
            {
                return false;
            }

            if (i + len > bytes.size())
            {
                return false;
            }

            for (std::size_t k = 1; k < len; k++)
            {
                std::uint8_t bx = bytes[i + k];
                if ((bx & 0xC0u) != 0x80u)
                {
                    return false;
                }
                cp = (cp << 6) | static_cast<std::uint32_t>(bx & 0x3Fu);
            }

            if ((len == 2 && cp < 0x80u) ||
                (len == 3 && cp < 0x800u) ||
                (len == 4 && cp < 0x10000u))
            {
                return false;
            }

            if (cp >= 0xD800u && cp <= 0xDFFFu)
            {
                return false;
            }

            if (cp > 0x10FFFFu)
            {
                return false;
            }

            count++;
            i += len;
        }

        codepoints = count;
        return true;
    }
}