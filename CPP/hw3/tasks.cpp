#include "tasks.h"
#include <cctype>
#include <regex>

Parser create_char_parser(char c) {
    return [c](const char* input) -> const char* {
        if (input == nullptr || *input == '\0') {
            return nullptr;
        }

        if (*input == c) {
            return input + 1;
        }
        return nullptr;
    };
}

Parser regex_match(const std::string& pattern) {
    return [re = std::regex(pattern)](const char* input) -> const char* {
        if (input == nullptr) {
            return nullptr;
        }

        std::cmatch match;
        if (!std::regex_search(input, match, re, std::regex_constants::match_continuous)) {
            return nullptr;
        }
        return input + match.length();
    };
}

Parser skip_ws() {
    return [](const char* input) -> const char* {
        if (input == nullptr) {
            return nullptr;
        }

        while (*input != '\0' && std::isspace(static_cast<unsigned char>(*input))) {
            input++;
        }

        return input;
    };
}

Parser any_of(const std::vector<Parser>& parsers) {
    return [parsers](const char* input) -> const char* {
        for (const Parser& parser : parsers) {
            const char* result = parser(input);
            if (result != nullptr) {
                return result;
            }
        }
        return nullptr;
    };
}

Parser sequence(const std::vector<Parser>& parsers) {
    return [parsers](const char* input) -> const char* {
        if (input == nullptr) {
            return nullptr;
        }

        const char* current = input;
        for (const Parser& parser : parsers) {
            current = parser(current);
            if (current == nullptr) {
                return nullptr;
            }
        }
        return current;
    };
}

Parser repeat(const Parser& parser, std::size_t n) {
    std::vector<Parser> parsers(n, parser);
    return sequence(parsers);
}

Parser create_word_parser(const std::string& word) {
    std::vector<Parser> parsers;
    parsers.reserve(word.size());

    for (char c : word) {
        parsers.push_back(create_char_parser(c));
    }

    return sequence(parsers);
}
