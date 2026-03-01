#include <iostream>

struct Person {
    unsigned char age: 7;
    unsigned char gender: 1;
};

struct PackedBitfield {
    uint8_t b1 : 3, : 2, b2 : 6, b3 : 2;
};

struct MultiType {
    union {
        float float_val;
        int int_val;
    };  // or } data;
};

struct GuessMySize1 {
    bool f1;
    char f2;
    char f3;
};
struct GuessMySize2 {
    bool f1;
    int f2;
    char f3;
};
struct GuessMySize3 {
    int f1;
    bool f2;
    char f3;
};

struct GuessMySize4 {
    bool f1;
    int f2;
    char f3;
} __attribute__((packed));

struct alignas(16) GuessMySize5 {
    int f1;
    bool f2;
    char f3;
//    int f4;
//    int f5;
//    int f6;
};

void sizes() {
    std::cout
            << "GuessMySize1: " << sizeof(GuessMySize1) << " " << alignof(GuessMySize1) << "\n"
            << "GuessMySize2: " << sizeof(GuessMySize2) << " " << alignof(GuessMySize2) << "\n"
            << "GuessMySize3: " << sizeof(GuessMySize3) << " " << alignof(GuessMySize3) << "\n"
            << "GuessMySize4: " << sizeof(GuessMySize4) << " " << alignof(GuessMySize4) << "\n"
            << "GuessMySize5: " << sizeof(GuessMySize5) << " " << alignof(GuessMySize5) << std::endl;

    std::cout << "f2: " << offsetof(GuessMySize2, f2) << " f3: " << offsetof(GuessMySize2, f3) << std::endl;
}

int main() {
    Person person = {};
    person.age = 42;
    person.gender = 0;

    std::cout << sizeof(person) << std::endl;

    PackedBitfield packedbf = {};
    packedbf.b1 = 3;
    packedbf.b2 = 8;
    packedbf.b3 = 2;

    std::cout << sizeof(packedbf) << std::endl;
    std::cout << (int) packedbf.b1 << " " << (int) packedbf.b2 << " " << (int) packedbf.b3 << std::endl;

    MultiType type = {};
    type.float_val = 100;
    std::cout << type.int_val << std::endl; // UB (maybe?)

    sizes();

    return 0;
}
