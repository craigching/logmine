#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "align.h"
#include "tokens.h"

TEST_CASE( "should test token merging", "[merge]" ) {

    std::string left{"This is a good test"};
    std::string right{"This is a bad test"};

    auto leftTokens = tokenize(left);
    auto rightTokens = tokenize(right);

    auto out = merge(leftTokens, rightTokens);

    REQUIRE(untokenize(out, " ") == "This is a WORD test");
}
