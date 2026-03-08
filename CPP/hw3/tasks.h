#include <cstddef>
#include <functional>
#include <string>
#include <vector>

using Parser = std::function<const char*(const char*)>;

Parser create_char_parser(char c);
Parser regex_match(const std::string& pattern);
Parser skip_ws();
Parser any_of(const std::vector<Parser>& parsers);
Parser sequence(const std::vector<Parser>& parsers);
Parser repeat(const Parser& parser, std::size_t n);
Parser create_word_parser(const std::string& word);
