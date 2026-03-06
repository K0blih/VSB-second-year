#include <iostream>
#include <regex>

using std::cout;
using std::endl;

int main() {
    // Search
    std::regex r1{R"#((\d+). (.*) \((\d+),-\))#"};

    cout << std::regex_search("1. Vepro knedlo zelo (125,-)", r1) << endl;
    cout << std::regex_search("hello", r1) << endl;

    // Match
    std::smatch match;
    std::string s2 = "1. Svickova (140,-)";
    std::regex_match(s2, match, r1);
    cout << match.empty() << " " << match.str() << endl;
    for (auto item: match) {
        cout << item << endl;
    }

    // Replace
    std::string s3 = "a: x, c: d, y: c";
    std::regex r2{R"#(a|b|c)#"};
    auto result = std::regex_replace(s3, r2, "[$&]");   // $& represents the original match
    cout << result << endl;

    // Split/iterate
    std::regex word_regex(R"#((\w+))#");
    std::string s1 = "hello world how are you";
    auto words_begin = std::sregex_iterator(s1.begin(), s1.end(), word_regex);
    auto words_end = std::sregex_iterator();

    while (words_begin != words_end) {
        cout << words_begin->str() << endl;
        words_begin++;
    }

    return 0;
}
