// This tells doctest to provide a main() function
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <memory>
#include <vector>
#include <sstream>

#include "doctest.h"
#include "student.h"

TEST_CASE("Text serialize no subjects") {
    Student s1{"Michal Kolomaznik", 23, {}};

    std::stringstream ss;
    ss << s1;

    REQUIRE(ss.str() == "Michal Kolomaznik (23): []");
}

TEST_CASE("Text serialize single subject") {
    Student s1{"Michal Kolomaznik", 23, {
            Subject{"DIM", 0.8},
    }};

    std::stringstream ss;
    ss << s1;

    REQUIRE(ss.str() == "Michal Kolomaznik (23): [{DIM, 0.80}]");
}

TEST_CASE("Text serialize multiple subjects") {
    Student s1{"Michal Kolomaznik", 23, {
            Subject{"DIM", 0.8},
            Subject{"APPS", 1.0},
            Subject{"C++I", 0.75}
    }};

    std::stringstream ss;
    ss << s1;

    REQUIRE(ss.str() == "Michal Kolomaznik (23): [{DIM, 0.80},{APPS, 1.00},{C++I, 0.75}]");
}

TEST_CASE("Text deserialize no subjects") {
    Student s1{"Michal Kolomaznik", 23, {}};

    Student deserialized;
    std::stringstream ss;
    ss << "Michal Kolomaznik (23): []";
    ss >> deserialized;
    REQUIRE(!ss.fail());
    REQUIRE(s1 == deserialized);
}

TEST_CASE("Text deserialize single subject") {
    Student s1{"Michal Kolomaznik", 23, {
            Subject{"DIM", 0.8}
    }};

    Student deserialized;
    std::stringstream ss;
    ss << "Michal Kolomaznik (23): [{DIM, 0.80}]";
    ss >> deserialized;
    REQUIRE(!ss.fail());
    REQUIRE(s1 == deserialized);
}

TEST_CASE("Text deserialize multiple subjects") {
    Student s1{"Michal Kolomaznik", 23, {
            Subject{"DIM", 0.8},
            Subject{"APPS", 1.0},
            Subject{"C++I", 0.75}
    }};

    Student deserialized;
    std::stringstream ss;
    ss << "Michal Kolomaznik (23): [{DIM, 0.80},{APPS, 1.00},{C++I, 0.75}]";
    ss >> deserialized;
    REQUIRE(!ss.fail());
    REQUIRE(s1 == deserialized);
}

TEST_CASE("Text deserialize empty") {
    Student deserialized;
    std::stringstream ss;
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize missing space after name") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc(24): []";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize missing age") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (): []";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize missing subjects") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23)";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize missing closing brace") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23): [";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize partial subject") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23): [{DIM]";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize comma in subjects") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23): [,]";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize trailing comma") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23): [{DIM, 0.80},]";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize missing comma") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23): [{DIM, 0.80}{APPS, 1.00}]";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize multiple commas") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23): [{DIM, 0.80},,{APPS, 1.00}]";
    ss >> deserialized;
    REQUIRE(ss.fail());
}

TEST_CASE("Text deserialize invalid difficulty") {
    Student deserialized;
    std::stringstream ss;
    ss << "Jakub Kolotoc (23): [{DIM, foo}]";
    ss >> deserialized;
    REQUIRE(ss.fail());
}
