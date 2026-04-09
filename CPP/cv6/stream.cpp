#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iterator>

using std::cout;
using std::cin;
using std::endl;

struct Point {
    int x;
    int y;
};

std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "Point(" << point.x << ", " << point.y << ")";
    return os;
}
std::istream& operator>>(std::istream& is, Point& point) {
    is >> point.x >> point.x;
    return is;
}

int main() {
    // Streams are abstractions for reading/writing data
    // They provide the same interface for writing, regardless of the backend (file, terminal, socket, memory buffer, ...)
    // https://en.cppreference.com/w/cpp/io#Stream-based_I.2FO

    // Standard output
    // std::cout, std::cerr are global variables, instances of std::ostream

    // C++ streams are by default synchronized with C output.
    // You can turn it off to get more I/O performance:
    std::ios::sync_with_stdio(false);

    // You can write to an output stream with <<
    cout << "Hello" << 1 << "\n";
    // Support for custom types can be provided with operator<< overload
    Point p{1, 2};
    cout << p << "\n";

    // There are I/O manipulators to modify output (https://en.cppreference.com/w/cpp/io/manip)
    cout << std::hex << 15 << "\n";
    // Manipulators change the state of the stream
    cout << 16 << std::dec << " " << 16 << "\n";

    // streams can be combined with iterators
    std::vector<int> v1{1, 2, 3, 4, 5};
    std::copy_if(v1.begin(), v1.end(), std::ostream_iterator<int>(cout, ", "), [](auto value) {
        return value % 2 == 0;
    });

    // By default std::cout is buffered, std::cerr is unbuffered
    cout << "Hello"; // at this point, nothing may show up in the output yet
    // You can flush the buffer explicitly with std::flush
    cout << std::flush;

    // std::endl is a shortcut for writing "\n" + std::flush
    cout << "Hello" << std::endl;
    // Flushing too often can be wasteful! Avoid endl if you just need a newline.

    // Standard input
    // std::cin is a global variable, instance of std::istream
    int x;
    cin >> x;

    // std::cin skips over whitespaces

    // operator>> can be also overloaded
    Point point2{};
    cin >> point2;

    // You can also read lines
    std::string line;
    std::getline(cin, line);

    // Errors can be checked on the stream after reading
    int x2;
    cin >> x2;
    std::cout << cin.eof() << std::endl;    // end of file reached
    std::cout << cin.bad() << std::endl;    // irrecoverable error
    std::cout << cin.fail() << std::endl;   // formatting/parsing operation failed
    cin.clear(); // clears error state
    std::cout << cin.fail() << std::endl;

    // Peeking
    char c = cin.get(); // read a single byte
    cin.putback(c);     // put it back into the stream
    char c2 = cin.peek(); // simpler form of peeking

    // The only difference between text/binary modes is that text converts between platform newlines
    // Text content is usually written/parsed with << and >>
    // Binary content is usually written/parsed with .read() and .write()

    // If you need to format something into a string, you can use std::stringstream
    std::stringstream ss;
    ss << "Hello" << 1 << std::endl;
    std::string str = ss.str();

    // Files are handled with std::fstream
    std::ofstream fs("out.txt"); // or std::fstream fs("out.txt", std::ios::out);
    assert(fs.is_open()); // should be always checked first!

    fs << "Hello world" << std::endl;

//    fs.seekp();

    // RAII takes care of flushing and closing the file after it goes out of scope

    // Binary mode
    int items[] = {1, 2, 3, 4, 5};
    std::ofstream fs2("out.bin", std::ios::binary);
    fs2.write(reinterpret_cast<const char*>(items), sizeof(int) * 5);
    fs2.close();

    int items2[5]{};
    std::ifstream fs3("out.bin", std::ios::binary);
    fs3.read(reinterpret_cast<char*>(items2), sizeof(int) * 5);
    fs3.close();

    for (auto item: items2) {
        cout << item << endl;
    }

    return 0;
}
