// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include "../../src/algorithm/matching/bipartite_flow.hpp"

long long brute(
    int left_size,
    int right_size,
    const std::vector<long long>& left_cap,
    const std::vector<long long>& right_cap,
    const std::vector<std::tuple<int, int, long long>>& edges
){
    int n = left_size + right_size + 2;
    int source = left_size + right_size;
    int sink = source + 1;
    std::vector<std::tuple<int, int, long long>> all;
    for(int i = 0; i < left_size; i++) all.push_back({source, i, left_cap[static_cast<std::size_t>(i)]});
    for(auto [l, r, c]: edges) all.push_back({l, left_size + r, c});
    for(int i = 0; i < right_size; i++) all.push_back({left_size + i, sink, right_cap[static_cast<std::size_t>(i)]});

    long long best = (1LL << 60);
    for(int mask = 0; mask < (1 << n); mask++){
        if(((mask >> source) & 1) == 0 || ((mask >> sink) & 1) != 0) continue;
        long long cut = 0;
        for(auto [u, v, c]: all){
            if(((mask >> u) & 1) && !((mask >> v) & 1)) cut += c;
        }
        best = std::min(best, cut);
    }
    return best;
}

void self_test(){
    {
        BipartiteFlow<long long> graph(2, 2);
        graph.add_left_capacity(0, 3);
        graph.add_left_capacity(1, 2);
        graph.add_right_capacity(0, 2);
        graph.add_right_capacity(1, 4);
        graph.add_edge(0, 0, 2);
        graph.add_edge(0, 1, 3);
        graph.add_edge(1, 1, 2);
        assert(graph.max_flow().flow == 5);
    }
    std::mt19937 rng(20260823);
    for(int l = 0; l <= 5; l++){
        for(int r = 0; r <= 5; r++){
            for(int step = 0; step < 80; step++){
                BipartiteFlow<long long> graph(l, r);
                std::vector<long long> left_cap(static_cast<std::size_t>(l));
                std::vector<long long> right_cap(static_cast<std::size_t>(r));
                std::vector<std::tuple<int, int, long long>> edges;
                for(int i = 0; i < l; i++){
                    left_cap[static_cast<std::size_t>(i)] = static_cast<int>(rng() % 8);
                    graph.add_left_capacity(i, left_cap[static_cast<std::size_t>(i)]);
                }
                for(int i = 0; i < r; i++){
                    right_cap[static_cast<std::size_t>(i)] = static_cast<int>(rng() % 8);
                    graph.add_right_capacity(i, right_cap[static_cast<std::size_t>(i)]);
                }
                for(int i = 0; i < l; i++){
                    for(int j = 0; j < r; j++){
                        if(rng() % 3 == 0){
                            long long c = static_cast<int>(rng() % 8);
                            graph.add_edge(i, j, c);
                            edges.push_back({i, j, c});
                        }
                    }
                }
                assert(graph.max_flow().flow == brute(l, r, left_cap, right_cap, edges));
            }
        }
    }
}

int main(){
    int l, r, m;
    if(!(std::cin >> l >> r >> m)){
        self_test();
        return 0;
    }
    BipartiteFlow<long long> graph(l, r);
    for(int i = 0; i < l; i++){
        long long c;
        std::cin >> c;
        graph.add_left_capacity(i, c);
    }
    for(int i = 0; i < r; i++){
        long long c;
        std::cin >> c;
        graph.add_right_capacity(i, c);
    }
    for(int i = 0; i < m; i++){
        int u, v;
        long long c;
        std::cin >> u >> v >> c;
        graph.add_edge(u, v, c);
    }
    std::cout << graph.max_flow().flow << '\n';
}
