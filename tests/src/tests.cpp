#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// #include "align.h"
#include "logmine.h"

TEST_CASE( "should test string alignment", "[align2]" ) {

    auto t1 = tokenize("2020-09-06T16:00:00 Disconnected from broker broker1");
    auto t2 = tokenize("2020-09-06T16:00:00 Disconnected from broker broker2");

    auto dist = distance(t1, t2);
    auto expected = 0.2f;
    auto precision = std::abs(dist - expected);

    REQUIRE(precision < 0.001);
}
