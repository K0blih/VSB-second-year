#include <iostream>
#include <random>

using std::cout;
using std::endl;

int main() {
    // Generates entropy
    std::default_random_engine generator;
    cout << generator() << endl;

    // Creates a distribution based on the entropy generator
    std::uniform_int_distribution<int> distribution(1, 6);
    int dice_roll = distribution(generator);  // generates number in the range 1..6
    cout << dice_roll << endl;

    std::uniform_real_distribution<float> distribution2(1.0, 3.0);
    cout << distribution2(generator) << endl;

    return 0;
}
