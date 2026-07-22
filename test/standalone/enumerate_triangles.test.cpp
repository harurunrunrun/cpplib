// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/specialized/enumerate_triangles.hpp"

std::set<std::array<int, 3>> naive_triangles(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    std::vector<std::vector<unsigned char>> adjacent(
        static_cast<std::size_t>(n),
        std::vector<unsigned char>(static_cast<std::size_t>(n), 0)
    );
    for(const auto& [left, right]: edges){
        adjacent[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)] = 1;
        adjacent[static_cast<std::size_t>(right)][static_cast<std::size_t>(left)] = 1;
    }
    std::set<std::array<int, 3>> result;
    for(int first = 0; first < n; ++first){
        for(int second = first + 1; second < n; ++second){
            for(int third = second + 1; third < n; ++third){
                if(adjacent[static_cast<std::size_t>(first)][static_cast<std::size_t>(second)] &&
                   adjacent[static_cast<std::size_t>(first)][static_cast<std::size_t>(third)] &&
                   adjacent[static_cast<std::size_t>(second)][static_cast<std::size_t>(third)]){
                    result.insert({first, second, third});
                }
            }
        }
    }
    return result;
}

void test_random(){
    std::mt19937 rng(2026071454);
    for(int iteration = 0; iteration < 800; ++iteration){
        const int n = static_cast<int>(rng() % 32);
        std::vector<std::pair<int, int>> edges;
        for(int left = 0; left < n; ++left){
            for(int right = left + 1; right < n; ++right){
                if(rng() % 100 < 30) edges.push_back({left, right});
            }
        }
        std::set<std::array<int, 3>> actual;
        const long long count = enumerate_triangles(
            n, edges,
            [&](int first, int second, int third){
                assert(first < second && second < third);
                assert(actual.insert({first, second, third}).second);
            }
        );
        const auto expected = naive_triangles(n, edges);
        assert(actual == expected);
        assert(count == static_cast<long long>(expected.size()));
        assert(count_triangles(n, edges) == count);
    }
}

void test_complete_and_errors(){
    constexpr int n = 160;
    std::vector<std::pair<int, int>> edges;
    for(int left = 0; left < n; ++left){
        for(int right = left + 1; right < n; ++right) edges.push_back({left, right});
    }
    assert(count_triangles(n, edges) == n * (n - 1LL) * (n - 2LL) / 6);

    bool thrown = false;
    try{ (void)count_triangles(3, {{0, 3}}); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)count_triangles(3, {{1, 1}}); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long mod = 998244353;
    int n, edge_count;
    if(!(std::cin >> n >> edge_count)){
        test_random();
        test_complete_and_errors();
        return 0;
    }
    std::vector<long long> weight(static_cast<std::size_t>(n));
    for(long long& value: weight) std::cin >> value;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [left, right]: edges) std::cin >> left >> right;
    long long answer = 0;
    enumerate_triangles(n, edges, [&](int first, int second, int third){
        answer += weight[static_cast<std::size_t>(first)] *
            weight[static_cast<std::size_t>(second)] % mod *
            weight[static_cast<std::size_t>(third)] % mod;
        if(answer >= mod) answer -= mod;
    });
    std::cout << answer << '\n';
}
