#include <iostream>
#include <vector>

struct Subject {
    Subject() = default;
    Subject(std::string name, float difficulty) : name(std::move(name)), difficulty(difficulty)
    {}

    bool operator==(const Subject& subject) const {
        return  this->name == subject.name &&
                this->difficulty == subject.difficulty;
    }

    std::string name;
    float difficulty;
};

struct Student {
    Student() = default;
    Student(std::string name, int age, std::vector<Subject> subjects)
            : name(std::move(name)), age(age), subjects(std::move(subjects))
    {}

    bool operator==(const Student& other) const {
        return  this->name == other.name &&
                this->age == other.age &&
                this->subjects == other.subjects;
    }

    std::string name;
    int age;
    std::vector<Subject> subjects;
};

/**
 * TODO
 *
 * Implement text serialization of a student to an output stream. Use the exact format described below.
 * Format of student:
 * <name> (<age>): [<subject-0>,<subject-1>,...]
 *
 * Format of subject:
 * {<name>, <difficulty>}
 *
 * Difficulty should be printed with a fixed number of decimal places (2).
 *
 * Example:
 * Martin Novak (25): [{DIM, 0.65},{APPS, 0.95}]
 *
 * Implement the << operator for subject and use it in this function.
 * See tests for more details.
 */
std::ostream& operator<<(std::ostream& os, const Student& student);

/**
 * TODO
 *
 * Implement text deserialization of a student from an input stream.
 *
 * The format has to exactly match the format outputted by the << operator implemented above.
 * No trailing commas are allowed, whitespace has to exactly match, etc.
 * If there is any error during parsing, set the failbit on the input stream and return immediately.
 *
 * Important: DO NOT read the WHOLE input stream into a memory buffer (in a real case you would run out of memory).
 * Read the data step-by-step. If it's possible, try to avoid reading the stream char by char and read it in
 * larger chunks (e.g. use existing stdlib logic for reading numbers) where applicable.
 *
 * See tests for more details.
 *
 * Hint: use is.get(), is.putback() and std::getline with a custom separator to parse the input.
 */
std::istream& operator>>(std::istream& is, Student& student);
