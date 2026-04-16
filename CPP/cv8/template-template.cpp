#include <vector>
#include <string>

/*
 * Another way of specifying the properties of input types of generic functions is to
 * say that some type will have some type argument. This is called "template template parameters".
 */

template <typename Type>
using Table = std::vector<std::pair<std::string, std::vector<Type>>>;

// We take two types, C and T
template <typename C, typename T>
void foo1(C& container, T item) {
    container.push_back(item);
}

// We take two types, C1 and T. Additionally, we say that C1 takes T as a template parameter.
template <template<typename> typename C1, typename T>
void foo2(C1<T>& container, T item) {
    container.push_back(item);
}

int main() {
    Table<int> t1{};
    Table<std::string> t2{};

    // Compare compiler errors
    std::vector<int> items{1, 2, 3};
    foo1(items, 1);
//    foo1(items, "1");
    foo2(items, 1);
//    foo2(items, "1");

    return 0;
}
