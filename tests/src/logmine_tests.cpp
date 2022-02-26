#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "logmine.h"
#include "sajson.h"

#include <unordered_map>
#include <algorithm>

auto success(const sajson::document& doc) -> bool {
    if (!doc.is_valid()) {
        fprintf(stderr, "%s\n", doc.get_error_message_as_cstring());
        return false;
    }
    return true;
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
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

    std::string expected1{"DateTime Disconnected from broker WORD"};
    std::string expected2{"DateTime Connected as user: WORD database: users1"};

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
    const std::string& expected{"DateTime Connected as user: WORD database: users1"};

    REQUIRE(actual == expected);
}

TEST_CASE( "test zookeeper logs", "[zookeeper]") {
    std::ifstream logs{"../logs/Zookeeper/Zookeeper_2k.log"};

    REQUIRE( logs.is_open() );

    Logmine model;


    std::string line;
    while (std::getline(logs, line)) {
        // std::cout << line << std::endl;
        trim(line);
        if (line == "2015-07-29 19:21:42,709 - INFO  [/10.10.34.13:3888:QuorumCnxManager$Listener@493] - Received connection request /10.10.34.13:44219") {
            std::cout << "here" << "\n";
        }
        model.add(line);
    }
    logs.close();

    std::vector<Cluster> clusters = model.get_clusters();

    std::cout << "number of clusters: " << clusters.size() << std::endl;

    struct {
        auto operator()(Cluster& a, Cluster& b) -> bool { return a.size() < b.size(); }
    } customLess;

    std::sort(clusters.begin(), clusters.end(), customLess);
    for (auto& cluster : clusters) {
        std::cout << "id: " << cluster.id() << ", size: " << cluster.size() << std::endl;
    }
}

TEST_CASE( "testing distance", "[distance]") {
    // 2015-07-29 19:04:12,394 - INFO  [/10.10.34.11:3888:QuorumCnxManager$Listener@493] - Received connection request /10.10.34.11:45307
    // 2015-07-29 19:21:42,709 - INFO  [/10.10.34.13:3888:QuorumCnxManager$Listener@493] - Received connection request /10.10.34.13:44219

    std::string log_line{"2015-07-29 19:04:12,394 - INFO  [/10.10.34.11:3888:QuorumCnxManager$Listener@493] - Received connection request /10.10.34.11:45307"};
    // std::string line2{"2015-07-29 19:21:42,709 - INFO  [/10.10.34.13:3888:QuorumCnxManager$Listener@493] - Received connection request /10.10.34.13:44219"};

    // std::string line1{"- INFO [/10.10.34.11:3888:QuorumCnxManager$Listener@493] - Received connection request WORD"};
    std::string cluster_rep{"- INFO [/10.10.34.13:3888:QuorumCnxManager$Listener@493] - Received connection request WORD"};

    auto t1 = tokenize(log_line);
    auto t2 = tokenize(cluster_rep);

    auto d = distance(t1, t2);

    std::cout << "distance: " << d << std::endl;
}
