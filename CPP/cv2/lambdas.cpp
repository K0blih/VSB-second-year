#include <iostream>
#include <vector>
#include <algorithm>

using std::cout;
using std::endl;

// Something like this is created in the background
struct LambdaStruct {
    explicit LambdaStruct(int x): x(x) {}

    void operator()() const {
        cout << x << endl;
    }

    int x;
};

void call_fn(std::function<int()> fn) {
    cout << fn() << endl;
    cout << fn() << endl;
    cout << fn() << endl;
}

auto capturing_1(int x) {
    // capture by value
    return [x]() {
        return x + 1;
    };
}
void capturing_2(int x) {
    // capture by reference
    auto fn = [&x]() {
        x++;
        return x;
    };
    call_fn(fn);
}
// You can also capture everything by value (=), everything by reference (&) or capture this
// https://en.cppreference.com/w/cpp/language/lambda

int main() {
    // basic lambda
    auto a = [](int x) {
        cout << x << endl;
    };
    a(5);

    // auto parameter types
    auto b = [](auto x){
        cout << x << endl;
    };
    b(5);
    b("hello");

    // trailing return type
    auto c = [](int x) -> int {
        return x + 1;
    };
    auto val = c(5);

    // Use in scenarios where you need a short code snippets that doesn't need a name
    std::vector<int> vec{5, 4, 3, 2, 1};
    std::sort(vec.begin(), vec.end(), [](int a, int b) {
        return a < b;
    });
    cout << vec[0] << endl;

    auto fn1 = capturing_1(5);
    cout << fn1() << endl;

    capturing_2(6);

    // lambdas in the background
    auto lambda = [](int x) {
        cout << x << endl;
    };
    lambda(5);

    LambdaStruct lambdaStruct(5);
    lambdaStruct();

    return 0;
}
