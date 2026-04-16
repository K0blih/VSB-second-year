#include <iostream>
#include <vector>
#include <array>

using std::cout;
using std::endl;

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Template arguments can also be (integral) values
// Useful e.g. for compiling separate versions of a single function, e.g. to avoid runtime conditions.
template <bool rgb_to_bgr>
void image_processing(std::vector<Pixel>& image, int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            auto& pixel = image[row * cols + col];
            if (rgb_to_bgr) {
                std::swap(pixel.r, pixel.b);
            }
            pixel.r += 10;
            pixel.g += 10;
            pixel.b += 10;
        }
    }
}

// Also useful for providing compile time checks for static sizes (e.g. of arrays)
template <typename T, int size>
class Arr {
public:
    template <int size2>
    Arr<T, size + size2> append(Arr<T, size2> arr) {
        std::array<T, size + size2> newArr{};
        for (int i = 0; i < size; i++) {
            newArr[i] = this->items[i];
        }
        for (int i = 0; i < size2; i++) {
            newArr[size + i] = arr.items[i];
        }
        return Arr<T, size + size2>{newArr};
    }

    Arr<T, size> append(Arr<T, size> arr) {
        std::array<T, size> newArr{};
        for (int i = 0; i < size; i++) {
            newArr[i] = this->items[i] + arr.items[i];
        }
        return Arr<T, size>{newArr};
    }

    std::array<T, size> items;
};

int main() {
    std::vector<Pixel> image {
            {100, 0, 50},
            {0, 0, 10},
            {0, 30, 0},
            {0, 0, 250}
    };

    // Two versions of the function are compiled
    image_processing<true>(image, 2, 2);
    image_processing<false>(image, 2, 2);

    Arr<int, 3> a1{};
    Arr<int, 4> a2{};
    Arr<int, 7> a3 = a1.append(a2);
//    Arr<int, 8> a4 = a1.append(a2);

    return 0;
}
