#include <iostream>

// C-style enum
enum Color {
    Red = 1,
    Green = 2,
    Blue = 3,
    Yellow = 4
};

// C++ style enum
enum class BrushType {
    Soft = 1,
    Medium = 2,
    Hard = 3
};

// default type is int
enum struct Foo: unsigned char {
    A = 255,
//    B = 256 //  invalid value
};

int main() {
    // Members are in global namespace
    Color color1 = Red;
    int x = Red;    // valid

    BrushType brush1 = BrushType::Soft;
//    int y = BrushType::Medium;  // invalid

    std::cout << color1 << std::endl;   // prints integer value
//    std::cout << brush1 << std::endl; // invalid

    return 0;
}
