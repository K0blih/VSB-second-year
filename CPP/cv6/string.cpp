#include <iostream>
#include <string>
#include <string_view>

using std::cout;
using std::endl;

struct Celsius
{
    long double celsius;
};
constexpr Celsius operator"" _c (long double n)
{
    return Celsius{n};
}

int main() {
    // C-strings
    const char* cstr1 = "hello";
    const char cstr2[] = "hello";

    std::string s1 = "hello";
    auto size = s1.size();
    auto capacity = s1.capacity();

    // No 0 needs to be at the end (it is usually there to allow cheap c_str())
    const char* cstr3 = s1.c_str();

    // conversions
    std::string s2 = s1 + " world"; // const char* -> std::string
    const char* p1 = s1.data();
//    const char* p2 = p1 + " world"; // not possible

    // mutability
    s1[0] = 'x'; // strings are mutable

    // string literals
    using namespace std::string_literals;

    auto lit1 = "hello";    // const char*
    std::cout << typeid(lit1).name() << std::endl;
    auto lit2 = "hello"s;   // std::string
    std::cout << typeid(lit2).name() << std::endl;

    // everything is backed by https://en.cppreference.com/w/cpp/string/basic_string
    wchar_t lit3[] = L"wchar-literal";
    unsigned char lit4[] = u8"utf8-literal with čárky háčky";   // std::u8string (C++20)
    char16_t lit5[] = u"utf16-literal with čárky háčky";    // std::u16string
    char32_t lit6[] = U"utf32-literal with čárky háčky";    // std::u32string
    auto lit7 = R"#(
Multi-line
    raw
string
\x
)#";
    std::cout << lit7 << std::flush;

    // user defined literals
    Celsius celsius = 1.5_c;

    // comparisons
    std::string a1 = "hello";
    std::string a2 = "hellox";
    cout << (a1 == a2) << " " << (a1 < a2) << " " << (a2 < a1) << endl;

    // searching
    std::string s3 = "hello";
    auto it = s3.find('x');
    cout << it << " " << (it == std::string::npos) << endl;

    // working with numbers
    std::string strint = std::to_string(1);
    std::string strfloat = std::to_string(1.56);

    int intval = std::stoi("1");
    int floatval = std::stof("1.56");

    return 0;
}
