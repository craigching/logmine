#ifndef TOKENS_H
#define TOKENS_H

#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <regex>

enum class Tokens {Text, Gap, Word, Date, Time, DateTime};

auto token_to_str(const Tokens& token) -> std::string {
    switch(token) {

    case Tokens::Text:
        return "Text";

    case Tokens::Gap:
        return "Gap";

    case Tokens::Word:
        return "Word";

    case Tokens::Date:
        return "Date";

    case Tokens::Time:
        return "Time";

    case Tokens::DateTime:
        return "DateTime";
    }

    return "Unknown";
}

class Token {

    std::string _str;
    Tokens _tokenType;

public:
    Token(std::string str, Tokens tokenType) : _str(std::move(str)), _tokenType(tokenType) {}
    auto to_str() -> std::string { return _str; }
    auto token_type() -> Tokens { return _tokenType; }

    auto operator==(const Token& other) const -> bool {
        return _str == other._str && _tokenType == other._tokenType;
    }

    ~Token() = default;
};

class Text : public Token {
public:
    Text(const std::string& str) : Token(str, Tokens::Text) {}
};

class Gap : public Token {
public:
    Gap(const std::string str = "GAP") : Token(str, Tokens::Gap) {}
};

class Word : public Token {
public:
    Word(const std::string str = "WORD") : Token(str, Tokens::Word) {}
};

class DateToken : public Token {

    const static std::string date_regex;

public:
    DateToken(const std::string str = "Date") : Token{str, Tokens::Date} {}

    static auto isa(const std::string& str) -> bool {
        return std::regex_match(str, std::regex{date_regex});
    }
};
const std::string DateToken::date_regex{R"(\d{4}-\d{2}-\d{2})"};

class TimeToken : public Token {
    const static std::string time_regex;

public:
    TimeToken(const std::string str = "Time") : Token{str, Tokens::Time} {}

    static auto isa(const std::string& str) -> bool {
        return std::regex_match(str, std::regex{time_regex});
    }
};
const std::string TimeToken::time_regex{R"(\d{2}:\d{2}:\d{2},\d{3})"};

class DateTimeToken : public Token {

    const static std::string date_time_regex;

public:
    DateTimeToken(const std::string str = "DateTime") : Token{str, Tokens::DateTime} {}

    static auto isa(const std::string& str) -> bool {
        return std::regex_match(str, std::regex{date_time_regex});
    }
};
const std::string DateTimeToken::date_time_regex{R"(20\d{2}-(0[1-9]|1[0-2])-[0-3]\dT([0-1][0-9]|2[0-3]):[0-5]\d:[0-5]\d)"};

auto tokenize(std::string str) -> std::vector<Token> {

    std::istringstream to_tokenize{str};
    std::string str1;
    std::vector<Token> tokens;

    while(to_tokenize >> str1) {
        if (DateToken::isa(str1)) {
            tokens.push_back(DateToken{});
        } else if (TimeToken::isa(str1)) {
            tokens.push_back(TimeToken{});
        } else if (DateTimeToken::isa(str1)) {
            tokens.push_back(DateTimeToken{});
        } else {
            tokens.push_back(Text{str1});
        }
    }

    return tokens;
}

template <typename T>
auto join(const T& v, const std::string& delim) -> std::string {
    std::ostringstream s;
    for (const auto& i : v) {
        if (&i != &v[0]) {
            s << delim;
        }
        s << i;
    }
    return s.str();
}

auto untokenize(std::vector<Token> tokens, const std::string& delim = "") -> std::string {
    std::vector<std::string> s;
    for (auto token : tokens) {
        s.push_back(token.to_str());
    }
    return join(s, delim);
}

auto merge(std::vector<Token> l, std::vector<Token> r) -> std::vector<Token> {

    std::vector<Token> merged;
    merged.reserve(std::min(l.size(), r.size()));

    auto lToken = l.begin();
    auto rToken = r.begin();
    for ( ; lToken != l.end() && rToken != r.end(); lToken++, rToken++) {
        bool found = false;

        if ((*lToken).token_type() == Tokens::Text && (*rToken).token_type() == Tokens::Text && (*lToken).to_str() != (*rToken).to_str()) {
            merged.push_back(Word{});
            continue;
        }

        if ((*lToken).token_type() == Tokens::Word && (*rToken).token_type() == Tokens::Text) {
            merged.push_back(Word{});
            continue;
        }

        if ((*rToken).token_type() == Tokens::Word && (*lToken).token_type() == Tokens::Text) {
            merged.push_back(Word{});
            continue;
        }

        if ((*rToken).token_type() == Tokens::Date && (*lToken).token_type() == Tokens::Date) {
            merged.push_back(DateToken{});
            continue;
        }

        if ((*rToken).token_type() == Tokens::Time && (*lToken).token_type() == Tokens::Time) {
            merged.push_back(TimeToken{});
            continue;
        }

        if ((*rToken).token_type() == Tokens::DateTime && (*lToken).token_type() == Tokens::DateTime) {
            merged.push_back(DateTimeToken{});
            continue;
        }

        if (!found) {
            merged.push_back(Text{(*rToken).to_str()});
        }
    }

    return merged;
}

#endif // TOKENS_H
