// STLveci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
#include <iomanip>

using namespace std;

struct Car {

	Car() {
		//cout << "empty constructor" << endl;
	}
	Car(int id, string brand) : id(id), brand(brand) {
		//cout << "int constructor" << endl;
	}

	~Car() {
		//cout << "destructor: " << this->id << endl;
	}

	bool operator < (const Car& other) const {
		return this->id < other.id;
	}

	bool operator > (const Car& other) const {
		return this->id > other.id;
	}

	bool operator()(const Car& lhs, const Car& rhs) const
	{
		return lhs == rhs;
	}

	bool operator == (const Car& other) const {
		return this->id == other.id && this->brand == other.brand;
	}

	friend ostream& operator<<(ostream& os, const Car& car) {
		os << "Car{id: " << car.id << ", brand : " << car.brand << " } ";
		return os;
	}

	size_t operator()(const Car& car) const
	{
		auto hash1 = std::hash<int>{}(car.id);
		auto hash2 = std::hash<string>{}(car.brand);
		//cout << "Car{id: " << car.id << ", brand : " << car.brand << " } " << " hash = " << (hash1 ^ hash2) << endl;
		return hash1 ^ hash2;
	}

	int id;
	string brand;

};


void set_example() {
	cout << "---------SET---------" << endl;

	set<int> my_set = { 5,3,1,2,5,3,2,1,4,2,3,4,2,1,2,3,4,5,1,2,3,4,5 };
	for (auto& item : my_set) cout << item << " ";
	cout << endl << "only uniq and ordered" << endl << endl;

	cout << "custom datatype Car" << endl;
	cout << "default" << endl;
	set<Car> my_set_of_cars = { {123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"} };
	for (auto& item : my_set_of_cars) {
		cout << item << endl;
	}
	cout << endl << "greater" << endl;
	set<Car, greater<>> my_set_of_cars_greater = { {123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"} };
	for (auto& item : my_set_of_cars_greater) {
		cout << item << endl;
	}

	cout << endl << "less" << endl;
	set<Car, less<>> my_set_of_cars_less = { {123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"} };
	for (auto& item : my_set_of_cars_less) {
		cout << item << endl;
	}
	cout << "---------SET END---------" << endl;
}


void multiset_example() {
	cout << "---------MULTISET---------" << endl;

	multiset<int> my_multiset = { 5,3,1,2,5,3,2,1,4,2,3,4,2,1,2,3,4,5,1,2,3,4,5 };
	for (auto& item : my_multiset) cout << item << " ";
	cout << endl << "ordered but not uniq" << endl << endl;

	cout << "custom datatype Car" << endl;
	cout << "basic" << endl;
	multiset<Car> my_multiset_of_cars = { {123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"} };
	for (auto& item : my_multiset_of_cars) {
		cout << item << endl;
	}
	cout << endl << "greater" << endl;
	multiset<Car, greater<>> my_multiset_of_cars_greater = { {123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"} };
	for (auto& item : my_multiset_of_cars_greater) {
		cout << item << endl;
	}

	cout << endl << "less" << endl;
	multiset<Car, less<>> my_multiset_of_cars_less = { {123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"} };
	for (auto& item : my_multiset_of_cars_less) {
		cout << item << endl;
	}
	cout << "---------MULTISET END---------" << endl;
}


void unorderedset_example() {
	cout << "---------UNORDERED SET---------" << endl;

	unordered_set<int> my_set = { 5,3,1,2,5,3,2,1,4,2,3,4,2,1,2,3,4,5,1,2,3,4,5 };
	for (auto& item : my_set) cout << item << " ";
	cout << endl << endl;

	cout << "custom datatype Car" << endl;
	unordered_set<Car, Car> my_set_of_cars = { {5, "Audi3"}, {123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"},  {5, "Audi2"} };
	for (auto& item : my_set_of_cars) {
		cout << item << endl;
	}

	cout << "---------UNORDERED SET END---------" << endl;
}

void unorderedmultiset_example() {
	cout << "---------UNORDERED MULTISET---------" << endl;

	unordered_multiset<int> my_multiset = { 5,3,1,2,5,3,2,1,4,2,3,4,2,1,2,3,4,5,1,2,3,4,5 };
	for (auto& item : my_multiset) cout << item << " ";
	cout << endl << "grouped into buckets" << endl << endl;

	cout << "custom datatype Car" << endl;
	cout << "basic" << endl;
	unordered_multiset<Car, Car> my_multiset_of_cars = { {5, "Audi3"}, {5, "Audi"} ,{123, "Skoda"}, {1001, "BMW"}, {5, "Audi"}, {5, "Audi"} };
	for (auto& item : my_multiset_of_cars) {
		cout << item << endl;
	}

	cout << "---------UNORDERED MULTISET END---------" << endl;
}



void unorderedmap_example() {
	cout << "---------UNORDERED MAP---------" << endl;
	unordered_map<char, int> my_set = { {'e',5},{'c',3},{'a',1},{'b',2},{'e',5},{'c',3},{'b',2},{'a',1},{'d',4},{'b',2},{'c',3},{'d',4},{'b',2},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5} };

	for (auto& item : my_set) cout << item.first << "->" << item.second << " ";
	cout << endl << endl;

	unordered_map<int, string> my_unordered_map = { {1, "prvni"}, {3, "treti"}, {4, "ctvrty"} };
	my_unordered_map.insert({ 2, "druhy" });
	for (auto& item : my_unordered_map) {
		cout << item.first << "->" << item.second << endl;
	}

	cout << endl << "po updatu" << endl;

	my_unordered_map[1] = "hned pred druhym";
	for (auto& item : my_unordered_map) {
		cout << item.first << " " << item.second << endl;
	}

	cout << endl << "find 4 = ";
	auto result_found = my_unordered_map.find(4);
	if (result_found != my_unordered_map.end()) {
		cout << result_found->first << " " << result_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "find 5 = ";
	auto result_not_found = my_unordered_map.find(5);
	if (result_not_found != my_unordered_map.end()) {
		cout << result_not_found->first << " " << result_not_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << endl;

	unordered_map<int, Car> my_multiset_of_cars = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ignoruje"}} };
	my_multiset_of_cars.insert({ 5, {5, "Audi toto se ignoruje"} });

	for (auto& item : my_multiset_of_cars) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}
	cout << endl << "po updatu" << endl;
	my_multiset_of_cars[5] = { 5, "Audi ale uz updatovane" };
	for (auto& item : my_multiset_of_cars) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	cout << endl << "find 123 = ";
	auto result_found_car = my_multiset_of_cars.find(123);
	if (result_found_car != my_multiset_of_cars.end()) {
		cout << "found " << result_found_car->first << " " << result_found_car->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "---------UNORDERED MAP END---------" << endl;
}

void map_example() {
	cout << "---------MAP---------" << endl;
	map<char, int> my_set = { {'e',5},{'c',3},{'a',1},{'b',2},{'e',5},{'c',3},{'b',2},{'a',1},{'d',4},{'b',2},{'c',3},{'d',4},{'b',2},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5} };

	for (auto& item : my_set) cout << item.first << "->" << item.second << " ";
	cout << endl << endl;

	map<int, string> _map = { {1, "prvni"}, {3, "treti"}, {4, "ctvrty"} };
	_map.insert({ 2, "druhy" });
	for (auto& item : _map) {
		cout << item.first << "->" << item.second << endl;
	}

	cout << endl << "po updatu" << endl;

	_map[1] = "hned pred druhym";
	for (auto& item : _map) {
		cout << item.first << " " << item.second << endl;
	}

	cout << endl << "find 4 = ";
	auto result_found = _map.find(4);
	if (result_found != _map.end()) {
		cout << result_found->first << " " << result_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "find 5 = ";
	auto result_not_found = _map.find(5);
	if (result_not_found != _map.end()) {
		cout << result_not_found->first << " " << result_not_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << endl;

	map<int, Car> my_multiset_of_cars = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ignoruje"}} };
	my_multiset_of_cars.insert({ 5, {5, "Audi toto se ignoruje"} });

	for (auto& item : my_multiset_of_cars) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}


	cout << endl << "less" << endl;
	map<int, Car, less<>> my_multiset_of_cars_less = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ignoruje"}} };
	for (auto& item : my_multiset_of_cars_less) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	cout << endl << "greater" << endl;
	map<int, Car, greater<>> my_multiset_of_cars_greater = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ignoruje"}} };
	for (auto& item : my_multiset_of_cars_greater) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	cout << endl << "po updatu" << endl;
	my_multiset_of_cars[5] = { 5, "Audi ale uz updatovane" };
	for (auto& item : my_multiset_of_cars) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	cout << endl << "find 123 = ";
	auto result_found_car = my_multiset_of_cars.find(123);
	if (result_found_car != my_multiset_of_cars.end()) {
		cout << "found " << result_found_car->first << " " << result_found_car->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "---------MAP END---------" << endl;
}

void multimap_example() {
	cout << "---------MULTIMAP---------" << endl;
	multimap<char, int> my_set = { {'e',5},{'c',3},{'a',1},{'b',2},{'e',5},{'c',3},{'b',2},{'a',1},{'d',4},{'b',2},{'c',3},{'d',4},{'b',2},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5} };

	for (auto& item : my_set) cout << item.first << "->" << item.second << " ";
	cout << endl << endl;

	multimap<int, string> _multimap = { {1, "prvni"}, {3, "treti"}, {4, "ctvrty"} };
	_multimap.insert({ 2, "druhy" });
	for (auto& item : _multimap) {
		cout << item.first << "->" << item.second << endl;
	}

	cout << endl << "po updatu" << endl;

	//_multimap[1] = "hned pred druhym"; //nejde
	for (auto& item : _multimap) {
		cout << item.first << " " << item.second << endl;
	}

	cout << endl << "find 4 = ";
	auto result_found = _multimap.find(4);
	if (result_found != _multimap.end()) {
		cout << result_found->first << " " << result_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "find 5 = ";
	auto result_not_found = _multimap.find(5);
	if (result_not_found != _multimap.end()) {
		cout << result_not_found->first << " " << result_not_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << endl;

	multimap<int, Car> my_multiset_of_cars = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ted neignoruje"}} };
	my_multiset_of_cars.insert({ 5, {5, "Audi toto se ted neignoruje"} });

	for (auto& item : my_multiset_of_cars) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	cout << endl << "less" << endl;
	multimap<int, Car, less<>> my_multiset_of_cars_less = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ted neignoruje"}} };
	for (auto& item : my_multiset_of_cars_less) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	cout << endl << "greater" << endl;
	multimap<int, Car, greater<>> my_multiset_of_cars_greater = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ted neignoruje"}} };
	for (auto& item : my_multiset_of_cars_greater) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	//cout << endl << "po updatu" << endl;
	//my_multiset_of_cars[5] = { 5, "Audi ale uz updatovane" }; // nejde

	cout << endl << "find 123 = ";
	auto result_found_car = my_multiset_of_cars.find(123);
	if (result_found_car != my_multiset_of_cars.end()) {
		cout << "found " << result_found_car->first << " " << result_found_car->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "---------MULTIMAP END---------" << endl;
}

void unorderedmultimap_example() {
	cout << "---------UNORDERED MULTIMAP---------" << endl;
	unordered_multimap<char, int> my_set = { {'e',5},{'c',3},{'a',1},{'b',2},{'e',5},{'c',3},{'b',2},{'a',1},{'d',4},{'b',2},{'c',3},{'d',4},{'b',2},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5},{'a',1},{'b',2},{'c',3},{'d',4},{'e',5} };

	for (auto& item : my_set) cout << item.first << "->" << item.second << " ";
	cout << endl << endl;

	unordered_multimap<int, string> _unordered_multimap = { {1, "prvni"}, {3, "treti"}, {4, "ctvrty"} };
	_unordered_multimap.insert({ 2, "druhy" });
	for (auto& item : _unordered_multimap) {
		cout << item.first << "->" << item.second << endl;
	}

	cout << endl << "po updatu" << endl;

	//_unordered_multimap[1] = "hned pred druhym"; //nejde
	for (auto& item : _unordered_multimap) {
		cout << item.first << " " << item.second << endl;
	}

	cout << endl << "find 4 = ";
	auto result_found = _unordered_multimap.find(4);
	if (result_found != _unordered_multimap.end()) {
		cout << result_found->first << " " << result_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "find 5 = ";
	auto result_not_found = _unordered_multimap.find(5);
	if (result_not_found != _unordered_multimap.end()) {
		cout << result_not_found->first << " " << result_not_found->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << endl;

	unordered_multimap<int, Car> my_multiset_of_cars = { {123,{123, "Skoda"}}, {1001, {1001, "BMW"}}, {5, {5, "Audi"}}, {5, {5, "Audi toto se ted neignoruje"}} };
	my_multiset_of_cars.insert({ 5, {5, "Audi toto se ted neignoruje"} });

	for (auto& item : my_multiset_of_cars) {
		cout << "key[" << setw(4) << item.first << "]->" << item.second << endl;
	}

	//cout << endl << "po updatu" << endl;
	//my_multiset_of_cars[5] = { 5, "Audi ale uz updatovane" }; // nejde

	cout << endl << "find 123 = ";
	auto result_found_car = my_multiset_of_cars.find(123);
	if (result_found_car != my_multiset_of_cars.end()) {
		cout << "found " << result_found_car->first << " " << result_found_car->second << endl;
	}
	else {
		cout << "not found " << endl;
	}
	cout << "---------UNORDERED MULTIMAP END---------" << endl;
}

int main() {
	set_example();
	multiset_example();
	unorderedset_example();
	unorderedmultiset_example();

	map_example();
	multimap_example();
	unorderedmap_example();
	unorderedmultimap_example();
}
