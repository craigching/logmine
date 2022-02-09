#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "align.h"
#include "tokens.h"

TEST_CASE( "should test string alignment", "[align2]" ) {

    std::string left{"HEAGAWGHEE"};
    std::string right{"PAWHEAE"};

    auto [leftOut, rightOut] = align2(left, right);

    REQUIRE(leftOut == "AWGHE-E");
    REQUIRE(rightOut == "AW-HEAE");
}

TEST_CASE( "should test string sequence that has two maximums", "[align2]" ) {

    std::string left{"ABCYE"};
    std::string right{"ABCZE"};

    auto [leftOut, rightOut] = align2(left, right);

    REQUIRE(leftOut == "ABCYE");
    REQUIRE(rightOut == "ABCZE");
}

TEST_CASE( "should test string verified at visualization site", "[align2]" ) {

    std::string left{"ABCXE"};
    std::string right{"ABCYZE"};

    auto [leftOut, rightOut] = align2(left, right);

    REQUIRE(leftOut == "ABC");
    REQUIRE(rightOut == "ABC");
}

TEST_CASE( "should test token alignment", "[align2]" ) {

    std::string left{"H E A G A W G H E E"};
    std::string right{"P A W H E A E"};

    auto leftTokens = tokenize(left);
    auto rightTokens = tokenize(right);

    auto [leftOut, rightOut] = align2<Token>(leftTokens, rightTokens, Gap("-"), match<Token>);

    REQUIRE(untokenize(leftOut) == "AWGHE-E");
    REQUIRE(untokenize(rightOut) == "AW-HEAE");
}

TEST_CASE( "should test token merging", "[merge]" ) {

    std::string left{"This is a good test"};
    std::string right{"This is a bad test"};

    auto leftTokens = tokenize(left);
    auto rightTokens = tokenize(right);

    auto [leftOut, rightOut] = align2<Token>(leftTokens, rightTokens, Gap("-"), match<Token>);

    auto out = merge(leftOut, rightOut);

    REQUIRE(untokenize(out, " ") == "This is a WORD test");
}

TEST_CASE( "should test tokens equivalent to ABCXE, ABCYZE", "[merge]" ) {

    std::string left{"This is a good test"};
    std::string right{"This is a really bad test"};

    auto leftTokens = tokenize(left);
    auto rightTokens = tokenize(right);

    auto [leftOut, rightOut] = align2<Token>(leftTokens, rightTokens, Gap("-"), match<Token>);

    auto out = merge(leftOut, rightOut);

    REQUIRE(untokenize(out, " ") == "This is a");
}
