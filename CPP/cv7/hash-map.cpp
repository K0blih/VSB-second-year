#include <cstring>
#include <iostream>
#include <unordered_map>

using std::cout;
using std::endl;

int main() {
    // Hash-map (dictionary) from keys to values
    // Keys need to be hashable
    std::unordered_map<int, int> map1{
        {1, 2},
        {3, 4}
    };

    // Access items
    map1[5] = 8;
    cout << map1[5] << endl;
    cout << map1.size() << endl;

    cout << map1[6] << endl; // If not present, the value will be default-constructed
    cout << map1.size() << endl;

    // Element query
    auto it = map1.find(5);
    cout << (it != map1.end()) << endl;

    int count = map1.count(5);
    cout << (count > 0) << endl;

    auto elem_it = map1.find(5);
    auto key = elem_it->first;
    auto value = elem_it->second;

    map1[8] = 10;

    // This is safe, iterators must be valid even after reallocation
    auto value2 = *elem_it;

    // Iteration order is not guaranteed. If you need it, use std::map
    for (auto [key, value] : map1) {
        cout << key << " " << value << endl;
    }

    // You can also use custom objects as keys, but you need to provide a hashing function and an equality operator
    struct Person {
        const char* name;
        int age;
        int friend_count;

        bool operator==(const Person& other) const {
            return !std::strcmp(this->name, other.name);
        }
    };

    struct PersonHasher {
        size_t operator()(const Person& person) const
        {
            auto hash1 = std::hash<int>{}(person.age);
            auto hash2 = std::hash<int>{}(person.friend_count);
            return hash1 ^ hash2;
        }
    };

    std::unordered_map<Person, int, PersonHasher> person_map;
    person_map[Person{"Michal", 5, 8}] = 1;

    return 0;
}
