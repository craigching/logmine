#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "logmine.h"
#include "sajson.h"

#include <unordered_map>

auto success(const sajson::document& doc) -> bool {
    if (!doc.is_valid()) {
        fprintf(stderr, "%s\n", doc.get_error_message_as_cstring());
        return false;
    }
    return true;
}

TEST_CASE( "should test string alignment", "[align2]" ) {

    auto t1 = tokenize("2020-09-06T16:00:00 Disconnected from broker broker1");
    auto t2 = tokenize("2020-09-06T16:00:00 Disconnected from broker broker2");

    auto dist = distance(t1, t2);
    auto expected = 0.2f;
    auto precision = std::abs(dist - expected);

    REQUIRE(precision < 0.001);
}

TEST_CASE( "", "" ) {
    auto logs = std::string{R"([
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker1" },
        { "message": "2020-09-06T16:00:00 Connected as user: cching, database: users1" },
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker2" },
        { "message": "2020-09-06T16:00:00 Connected as user: blah, database: users1" },
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker3" },
        { "message": "2020-09-06T16:00:00 Connected as user: blahblah, database: users1" },
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker4" }
    ])"};

    const sajson::document& document = sajson::parse(
        sajson::dynamic_allocation(), sajson::mutable_string_view(sajson::string(logs.c_str(), logs.length())));
    if (!success(document)) {
        REQUIRE(false);
    }

    Logmine model;

    const sajson::value& root = document.get_root();
    for (auto i = 0; i < root.get_length(); ++i) {
        const sajson::value& e = root.get_array_element(i);
        const sajson::value& message = e.get_value_of_key(sajson::literal("message"));
        model.add(message.as_cstring());
    }

    const std::vector<Cluster> clusters = model.get_clusters();

    std::unordered_map<std::string, int> cluster_size_map{};

    for (auto& cluster : clusters) {
        cluster_size_map[cluster.id()] = cluster.size();
    }

    std::string expected1{"2020-09-06T16:00:00 Disconnected from broker WORD"};
    std::string expected2{"2020-09-06T16:00:00 Connected as user: WORD database: users1"};

    REQUIRE(cluster_size_map.find(expected1) != cluster_size_map.end());
    REQUIRE(cluster_size_map[expected1] == 4);

    REQUIRE(cluster_size_map.find(expected2) != cluster_size_map.end());
    REQUIRE(cluster_size_map[expected2] == 3);

    REQUIRE(clusters.size() == 2);
}

TEST_CASE( "test merge", "[merge]" ) {
    auto log1 = tokenize("2020-09-06T16:00:00 Connected as user: cching, database: users1");
    auto log2 = tokenize("2020-09-06T16:00:00 Connected as user: blah, database: users1");

    auto merged = merge(log1, log2);

    auto actual = untokenize(merged, " ");
    const std::string& expected{"2020-09-06T16:00:00 Connected as user: WORD database: users1"};

    REQUIRE(actual == expected);
}
