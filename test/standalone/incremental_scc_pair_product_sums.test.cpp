// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/incremental_scc.hpp"
#include "../../src/algorithm/graph/connectivity/incremental_scc_pair_product_sums.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

struct DynamicDSU{
    std::vector<int> parent_or_size;

    explicit DynamicDSU(int n):
        parent_or_size(static_cast<std::size_t>(n), -1){}

    int leader(int vertex){
        int root = vertex;
        while(parent_or_size[static_cast<std::size_t>(root)] >= 0){
            root = parent_or_size[static_cast<std::size_t>(root)];
        }
        while(vertex != root){
            const int parent = parent_or_size[static_cast<std::size_t>(vertex)];
            parent_or_size[static_cast<std::size_t>(vertex)] = root;
            vertex = parent;
        }
        return root;
    }

    bool merge(int left, int right){
        left = leader(left);
        right = leader(right);
        if(left == right) return false;
        if(parent_or_size[static_cast<std::size_t>(left)] >
           parent_or_size[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        parent_or_size[static_cast<std::size_t>(left)] +=
            parent_or_size[static_cast<std::size_t>(right)];
        parent_or_size[static_cast<std::size_t>(right)] = left;
        return true;
    }
};

std::vector<std::vector<unsigned char>> reachability(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int prefix_length
){
    std::vector<std::vector<unsigned char>> reach(
        static_cast<std::size_t>(n),
        std::vector<unsigned char>(static_cast<std::size_t>(n))
    );
    for(int vertex = 0; vertex < n; ++vertex){
        reach[static_cast<std::size_t>(vertex)][
            static_cast<std::size_t>(vertex)
        ] = 1;
    }
    for(int edge = 0; edge < prefix_length; ++edge){
        const auto [from, to] = edges[static_cast<std::size_t>(edge)];
        reach[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)] = 1;
    }
    for(int middle = 0; middle < n; ++middle){
        for(int from = 0; from < n; ++from){
            if(!reach[static_cast<std::size_t>(from)][
                static_cast<std::size_t>(middle)]) continue;
            for(int to = 0; to < n; ++to){
                reach[static_cast<std::size_t>(from)][
                    static_cast<std::size_t>(to)
                ] = static_cast<unsigned char>(
                    reach[static_cast<std::size_t>(from)][
                        static_cast<std::size_t>(to)
                    ] ||
                    reach[static_cast<std::size_t>(middle)][
                        static_cast<std::size_t>(to)
                    ]
                );
            }
        }
    }
    return reach;
}

void check_case(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    bool compare_naive
){
    const int m = static_cast<int>(edges.size());
    const auto actual = incremental_scc(n, edges);
    assert(actual.size() == edges.size());
    for(int edge = 0; edge < m; ++edge){
        const int time = actual[static_cast<std::size_t>(edge)];
        assert(edge + 1 <= time && time <= m + 1);
    }
    if(!compare_naive) return;

    std::vector<int> expected(static_cast<std::size_t>(m), m + 1);
    std::vector<std::vector<int>> events(static_cast<std::size_t>(m + 1));
    for(int edge = 0; edge < m; ++edge){
        const int time = actual[static_cast<std::size_t>(edge)];
        if(time <= m) events[static_cast<std::size_t>(time)].push_back(edge);
    }
    DynamicDSU dsu(n);
    for(int time = 1; time <= m; ++time){
        const auto reach = reachability(n, edges, time);
        for(int edge = 0; edge < time; ++edge){
            if(expected[static_cast<std::size_t>(edge)] <= m) continue;
            const auto [from, to] = edges[static_cast<std::size_t>(edge)];
            if(reach[static_cast<std::size_t>(from)][
                   static_cast<std::size_t>(to)] &&
               reach[static_cast<std::size_t>(to)][
                   static_cast<std::size_t>(from)]){
                expected[static_cast<std::size_t>(edge)] = time;
            }
        }
        for(int edge: events[static_cast<std::size_t>(time)]){
            dsu.merge(
                edges[static_cast<std::size_t>(edge)].first,
                edges[static_cast<std::size_t>(edge)].second
            );
        }
        for(int left = 0; left < n; ++left){
            for(int right = 0; right < n; ++right){
                const bool same_scc =
                    reach[static_cast<std::size_t>(left)][
                        static_cast<std::size_t>(right)] &&
                    reach[static_cast<std::size_t>(right)][
                        static_cast<std::size_t>(left)];
                assert((dsu.leader(left) == dsu.leader(right)) == same_scc);
            }
        }
    }
    assert(actual == expected);
}

void test_random(){
    std::mt19937 rng(2026071701);
    for(int iteration = 0; iteration < 900; ++iteration){
        const int n = 1 + static_cast<int>(rng() % 8U);
        const int m = static_cast<int>(rng() % 21U);
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(m));
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
    check_case(1, {{0, 0}, {0, 0}}, true);
    check_case(4, {{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 2}}, true);

    constexpr int n = 500000;
    std::vector<std::pair<int, int>> cycle;
    cycle.reserve(static_cast<std::size_t>(n));
    for(int vertex = 0; vertex + 1 < n; ++vertex){
        cycle.push_back({vertex, vertex + 1});
    }
    cycle.push_back({n - 1, 0});
    const auto time = incremental_scc(n, cycle);
    for(int value: time) assert(value == n);

    bool thrown = false;
    try{ (void)incremental_scc(3, {{0, 3}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)incremental_scc(-1, {}); }
    catch(const std::runtime_error&){ thrown = true; }
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
    std::vector<FastModint998244353> weights(static_cast<std::size_t>(n));
    for(auto& value: weights) std::cin >> value;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [from, to]: edges) std::cin >> from >> to;
    for(const auto answer:
        incremental_scc_pair_product_sums(weights, edges)){
        std::cout << answer << '\n';
    }
}
