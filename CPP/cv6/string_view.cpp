#include <string_view>
#include <iostream>
#include <string>

using std::cout;
using std::endl;

using namespace std::string_literals;
using namespace std::string_view_literals;

std::string_view foo1(std::string x) {
    return std::string_view(x); // UB, `x` expires when this function finishes
}
void foo2(std::string_view sv) {}
std::string create_string() {
    return "foo";
}

int main() {
    // string-view is a borrowed reference/view into a string.
    // Useful for passing constant views into a string, for example for parsing or searching.
    std::string s4 = "hello";
    std::string_view sv1 = s4;
    cout << sv1 << " " << sv1.size() << endl;

    std::string_view sv2{s4.data() + 1, 2};   // pointer + size -> cheap to copy
    cout << sv2 << " " << sv2.size() << endl;

    // Does not allow assignment
    std::string_view sv3(s4);
//    sv3[0] = 'x'; // error

    // Does not own the string data!
    std::string s5 = "hello";
    std::string_view sv4(s5);
    cout << (void*) sv4.data() << endl;
    s5 += " world hello world hello world"; // sv4 must not be accessed after this line!
    cout << (void*) s5.data() << endl;
//    cout << sv4 << endl; // possibly UB

    // OK - constant string
    foo2("hello");

    // OK - std::string will live until the function finishes executing
    // But the function MUST NOT store the string_view anywhere
    foo2(std::string("hello"));

    std::string_view sl = "foo"sv; // string_view literal initialization

    // Initialization gotchas
    std::string_view sx1 = create_string(); // UB - string disappears
    std::string_view sx2 = "hello"s; // UB - string disappears
    std::string_view sx3 = "abc\0foo"; // contains only "abc"
    std::string_view sx4 = "abc\0foo"sv; // contains "abc\0foo"
    std::string_view sx5(nullptr); // UB

    // View modification (only modifies the view, not the string)
    std::string s6 = "hello";
    std::string_view sv5(s6);
    cout << sv5 << endl;

    sv5.remove_prefix(2);
    sv5.remove_suffix(1);
    cout << sv5 << endl;
}
