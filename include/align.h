#ifndef ALIGN_H
#define ALIGN_H

#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <tuple>

template<typename T, int MATCH_COST = 3>
inline auto match(const T& l, const T& r) -> int {
    return l == r ? MATCH_COST : -MATCH_COST;
}

// Implements Smith-Waterman which performs local alignment for two sequences
// https://gtuckerkellogg.github.io/pairwise/demo/ provides a visualization
// for testing
template<typename T, int GAP_COST = 2, int MATCH_COST = 3>
auto align2(const std::vector<T>& left, const std::vector<T>& right, const T& GAP, std::function<int(const T&, const T&)> match) -> std::tuple<std::vector<T>, std::vector<T>> {

    const int llen = left.size();
    const int rlen = right.size();
    int grid [llen + 1][rlen + 1];

    auto max = 0;
    auto mi = 0;
    auto mj = 0;

    for (auto i = 0; i < llen + 1; ++i) {
        for (auto j = 0; j < rlen + 1; ++j) {
            if (i == 0 || j == 0) {
                grid[i][j] = 0;
            } else {
                auto h1 = grid[i - 1][j - 1] + match(left[i - 1], right[j - 1]);
                auto h2 = grid[i][j - 1] - GAP_COST;
                auto h3 = grid[i - 1][j] - GAP_COST;
                auto m = std::max(0, std::max(h1, std::max(h2, h3)));
                grid[i][j] = m;
                if (m >= max) {
                    max = m;
                    mi = i;
                    mj = j;
                }
            }
        }
    }

    std::vector<T> leftOut;
    std::vector<T> rightOut;

    auto shouldContinue = true;
    while (shouldContinue) {

        auto h1 = grid[mi][mj - 1];
        auto h2 = grid[mi - 1][mj - 1];
        auto h3 = grid[mi - 1][mj];

        if (h2 == 0) {
            leftOut.push_back(left[mi - 1]);
            rightOut.push_back(right[mj - 1]);
            mi -= 1;
            mj -= 1;
        } else if (h1 >= h2 && h1 >= h3) {
            leftOut.push_back(GAP);
            rightOut.push_back(right[mj - 1]);
            mj -= 1;
        } else if (h2 >= h1 && h2 >= h3) {
            leftOut.push_back(left[mi - 1]);
            rightOut.push_back(right[mj - 1]);
            mi -= 1;
            mj -= 1;
        } else if (h3 >= h1 && h3 >= h2) {
            leftOut.push_back(left[mi - 1]);
            rightOut.push_back(GAP);
            mi -= 1;
        }

        if (h2 == 0) {
            shouldContinue = false;
        }
    }

    std::reverse(leftOut.begin(), leftOut.end());
    std::reverse(rightOut.begin(), rightOut.end());

    return std::make_tuple(leftOut, rightOut);
}

// Implements Needleman-Wunsch which performs global alignment for two sequences
template<typename T, int GAP_COST = -2, int MATCH_COST = 3>
auto align(const std::vector<T>& left, const std::vector<T>& right, const T& GAP) -> std::tuple<std::vector<T>, std::vector<T>> {

    const int llen = left.size();
    const int rlen = right.size();
    int grid [llen + 1][rlen + 1];

    for (auto i = 0; i < llen + 1; ++i) {
        for (auto j = 0; j < rlen + 1; ++j) {
            if (i == 0) {
                grid[i][j] = j * GAP_COST;
            } else if (j == 0) {
                grid[i][j] = i * GAP_COST;
            } else {
                auto t = grid[i][j - 1] + GAP_COST;
                auto l = grid[i - 1][j] + GAP_COST;
                auto tl = left[i - 1] == right[j - 1] ? MATCH_COST : -MATCH_COST;
                tl += grid[i - 1][j - 1];
                grid[i][j] = std::max(std::max(t, l), tl);
            }
        }
    }

    std::vector<T> leftOut;
    std::vector<T> rightOut;

    auto i = llen;
    auto j = rlen;

    leftOut.push_back(left[i]);
    rightOut.push_back(right[j]);

    while(i >= 1 && j >= 1) {

        auto di = 0;
        auto dj = -1;
        auto m = grid[i + di][j + dj];

        if (grid[i - 1][j - 1] > m) {
            di = -1;
            dj = -1;
            m = grid[i + di][j + dj];
        }
        if (grid[i - 1][j] > m) {
            di = -1;
            dj = 0;
            m = grid[i + di][j + dj];
        }

        i += di;
        j += dj;

        if (di == -1 && dj == -1) {
            leftOut.push_back(left[i]);
            rightOut.push_back(right[j]);
        } else if (di == -1 && dj == 0) {
            rightOut.push_back(GAP);
            leftOut.push_back(left[i]);
        } else {
            leftOut.push_back(GAP);
            rightOut.push_back(right[j]);
        }
    }

    std::reverse(leftOut.begin(), leftOut.end());
    std::reverse(rightOut.begin(), rightOut.end());

    return std::make_tuple(leftOut, rightOut);
}

auto align(const std::string_view& left, const std::string_view& right) -> std::tuple<std::string, std::string> {
    auto [leftOut, rightOut] = align<char>(std::vector<char>{left.begin(), left.end()}, std::vector<char>{right.begin(), right.end()}, '-');
    return std::make_tuple(std::string{leftOut.begin(), leftOut.end()}, std::string{rightOut.begin(), rightOut.end()});
}

auto align2(const std::string_view& left, const std::string_view& right) -> std::tuple<std::string, std::string> {
    auto [leftOut, rightOut] = align2<char>(std::vector<char>{left.begin(), left.end()}, std::vector<char>{right.begin(), right.end()}, '-', match<char>);
    return std::make_tuple(std::string{leftOut.begin(), leftOut.end()}, std::string{rightOut.begin(), rightOut.end()});
}

#endif // ALIGN_H
