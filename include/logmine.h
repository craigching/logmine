#ifndef LOGMINE_H
#define LOGMINE_H

#include <vector>
#include <regex>
#include <iterator>
#include <iostream>

#include "tokens.h"
#include "align.h"

template<typename T, int K = 1>
inline auto score(const T& t1, const T& t2) -> float {
    return t1 == t2 ? K : 0;
}

auto distance(const std::vector<Token>& log1, const std::vector<Token>& log2) -> float {
    auto len1 = log1.size();
    auto len2 = log2.size();

    auto min = std::min(len1, len2);
    auto max = std::max(len1, len2);

    auto sum = 0.0;

    for (auto i = 0; i < min; ++i) {
        auto t1 = log1[i];
        auto t2 = log2[i];

        auto s = score(t1, t2);
        sum += s / max;
    }

    return 1 - sum;
}

class Cluster {

    const std::vector<Token> rep;
    int size_;

public:
    Cluster(std::vector<Token> rep) : rep(std::move(rep)), size_{1} {}

    auto cluster_distance(const std::vector<Token>& log) -> float {
        return std::abs(distance(rep, log));
    }

    void add(const std::vector<Token>& log) {
        ++size_;
        auto [leftOut, rightOut] = align2<Token>(rep, log, Gap("-"), score<Token>);

        std::cout << "leftOut: " << untokenize(leftOut, " ") << ", rightOut: " << untokenize(rightOut, " ") << std::endl;
    }

    auto size() -> int {
        return this->size_;
    }

};

class Logmine {

    std::vector<Cluster> clusters;

public:
    void add(const std::string& log) {
        std::cout << "message: " << log << std::endl;
        std::vector<Token> tokenized_log = tokenize(log);
        for (auto t : tokenized_log) {
            std::cout << "token, type: " << token_to_str(t.token_type()) << ", value: " << t.to_str() << "\n";
        }
        find_cluster(tokenized_log);
    }

private:

    auto find_cluster(const std::vector<Token>& log) -> void {
        // TODO: Where to put this?
        const auto max_dist = 0.5;
        // Find the distance from the log to the cluster
        auto d = std::numeric_limits<float>::max();
        Cluster* found_cluster = nullptr;
        for (auto& cluster : clusters) {
            auto d1 = cluster.cluster_distance(log);
            // std::cout << "distance: " << d1 << std::endl;
            if (d1 < max_dist && d1 < d) {
                found_cluster = &cluster;
            }
        }

        if (found_cluster != nullptr) {
            found_cluster->add(log);
            std::cout << "cluster size: " << found_cluster->size() << std::endl;
        } else {
            clusters.emplace_back(Cluster(log));
        }
    }
};

#endif // LOGMINE_H
