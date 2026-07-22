// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/traversal/counting_eulerian_circuits.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

using Mint = FastModint998244353;

long long brute_count(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    (void)n;
    if(edges.empty()) return 0;
    const int m = static_cast<int>(edges.size());
    const int start = edges.front().first;
    std::function<long long(int, std::uint32_t)> search =
        [&](int vertex, std::uint32_t used){
            if(used == (std::uint32_t(1) << m) - 1U){
                return vertex == start ? 1LL : 0LL;
            }
            long long answer = 0;
            for(int edge = 1; edge < m; ++edge){
                if((used >> edge & 1U) != 0) continue;
                if(edges[static_cast<std::size_t>(edge)].first != vertex){
                    continue;
                }
                answer += search(
                    edges[static_cast<std::size_t>(edge)].second,
                    used | (std::uint32_t(1) << edge)
                );
            }
            return answer;
        };
    return search(edges.front().second, 1U);
}

void check_case(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    bool compare_brute
){
    const Mint actual = count_eulerian_circuits<Mint, 500>(n, edges);
    if(compare_brute) assert(actual == Mint(brute_count(n, edges)));
}

void test_random(){
    std::mt19937 rng(2026071604);
    for(int iteration = 0; iteration < 1200; ++iteration){
        const int n = 1 + static_cast<int>(rng() % 5U);
        const int m = 1 + static_cast<int>(rng() % 9U);
        std::vector<std::pair<int, int>> edges;
        for(int edge = 0; edge < m; ++edge){
            edges.push_back({
                static_cast<int>(rng() % static_cast<unsigned int>(n)),
                static_cast<int>(rng() % static_cast<unsigned int>(n))
            });
        }
        check_case(n, edges, true);
    }
}

void test_boundaries(){
    check_case(0, {}, true);
    check_case(1, {{0, 0}}, true);
    check_case(
        1,
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
         {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        true
    );
    check_case(4, {{0, 1}, {1, 2}, {2, 3}, {3, 0}}, true);
    check_case(4, {{0, 1}, {1, 0}, {2, 3}, {3, 2}}, true);
    check_case(3, {{0, 1}, {1, 2}}, true);

    constexpr int n = 80;
    constexpr int copies = 40;
    std::vector<std::pair<int, int>> cycle;
    cycle.reserve(static_cast<std::size_t>(n * copies));
    for(int copy = 0; copy < copies; ++copy){
        for(int vertex = 0; vertex < n; ++vertex){
            cycle.push_back({vertex, (vertex + 1) % n});
        }
    }
    assert((count_eulerian_circuits<Mint, 500>(n, cycle) != Mint(0)));

    bool thrown = false;
    try{ (void)count_eulerian_circuits<Mint, 500>(2, {{0, 2}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)count_eulerian_circuits<Mint, 2>(
            3, {{0, 1}, {1, 2}, {2, 0}}
        );
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    if(!(std::cin >> n >> m)){
        test_random();
        test_boundaries();
        return 0;
    }
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [from, to]: edges) std::cin >> from >> to;
    std::cout << count_eulerian_circuits<Mint, 500>(n, edges) << '\n';
}
