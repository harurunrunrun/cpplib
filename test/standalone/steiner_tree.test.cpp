// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/algorithm/graph/steiner_tree.hpp"

constexpr long long INF = (1LL << 60);

struct Dsu{
    std::vector<int> p;
    explicit Dsu(int n): p(static_cast<std::size_t>(n)){
        std::iota(p.begin(), p.end(), 0);
    }
    int leader(int x){
        while(p[static_cast<std::size_t>(x)] != x){
            x = p[static_cast<std::size_t>(x)] = p[static_cast<std::size_t>(p[static_cast<std::size_t>(x)])];
        }
        return x;
    }
    void merge(int a, int b){
        a = leader(a);
        b = leader(b);
        if(a != b) p[static_cast<std::size_t>(b)] = a;
    }
};

long long brute(
    int n,
    const std::vector<std::tuple<int, int, long long>>& edges,
    const std::vector<int>& terminals
){
    if(terminals.empty()) return 0;
    long long best = 0;
    bool found = false;
    const int m = static_cast<int>(edges.size());
    for(int mask = 0; mask < (1 << m); mask++){
        Dsu dsu(n);
        long long cost = 0;
        for(int i = 0; i < m; i++){
            if(mask >> i & 1){
                auto [u, v, w] = edges[static_cast<std::size_t>(i)];
                dsu.merge(u, v);
                cost += w;
            }
        }
        bool ok = true;
        int root = dsu.leader(terminals[0]);
        for(int t: terminals){
            if(dsu.leader(t) != root) ok = false;
        }
        if(ok && (!found || cost < best)){
            best = cost;
            found = true;
        }
    }
    return found ? best : INF;
}

void self_test(){
    {
        std::vector<std::vector<SteinerTreeEdge<long long>>> graph(5);
        auto add = [&](int u, int v, long long w){
            graph[static_cast<std::size_t>(u)].push_back({v, w});
            graph[static_cast<std::size_t>(v)].push_back({u, w});
        };
        add(0, 1, 2);
        add(1, 2, 2);
        add(1, 3, 1);
        add(3, 4, 1);
        add(2, 4, 10);
        assert(steiner_tree<long long>(graph, {0, 2, 4}, INF) == 6);
    }
    {
        std::vector<std::vector<SteinerTreeEdge<long long>>> graph(3);
        graph[0].push_back({1, INF});
        graph[1].push_back({0, INF});
        graph[1].push_back({2, 2000000000000000000LL});
        graph[2].push_back({1, 2000000000000000000LL});
        assert(steiner_tree<long long>(graph, {0, 2}, INF) ==
               INF + 2000000000000000000LL);
    }
    {
        std::vector<std::vector<SteinerTreeEdge<long long>>> graph(2);
        graph[1].push_back({1, -1});
        bool thrown = false;
        try{
            (void)steiner_tree<long long>(graph, {0}, INF);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        std::vector<std::vector<SteinerTreeEdge<long long>>> graph(1);
        std::vector<int> terminals(31, 0);
        bool thrown = false;
        try{
            (void)steiner_tree<long long>(graph, terminals, INF);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    std::mt19937 rng(20260808);
    for(int n = 1; n <= 8; n++){
        for(int step = 0; step < 80; step++){
            std::vector<std::tuple<int, int, long long>> edges;
            std::vector<std::vector<SteinerTreeEdge<long long>>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 4 == 0 && static_cast<int>(edges.size()) < 14){
                        long long w = 1 + static_cast<int>(rng() % 8);
                        edges.push_back({u, v, w});
                        graph[static_cast<std::size_t>(u)].push_back({v, w});
                        graph[static_cast<std::size_t>(v)].push_back({u, w});
                    }
                }
            }
            std::vector<int> terminals;
            for(int v = 0; v < n; v++){
                if(rng() % 4 == 0 && static_cast<int>(terminals.size()) < 4) terminals.push_back(v);
            }
            auto actual = steiner_tree<long long>(graph, terminals, INF);
            auto expected = brute(n, edges, terminals);
            assert(actual == expected);
        }
    }
}

int main(){
    int n, m, k;
    if(!(std::cin >> n >> m >> k)){
        self_test();
        return 0;
    }
    std::vector<int> terminals(static_cast<std::size_t>(k));
    for(int i = 0; i < k; i++) std::cin >> terminals[static_cast<std::size_t>(i)];
    std::vector<std::vector<SteinerTreeEdge<long long>>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph[static_cast<std::size_t>(u)].push_back({v, w});
        graph[static_cast<std::size_t>(v)].push_back({u, w});
    }
    long long ans = steiner_tree<long long>(graph, terminals, INF);
    std::cout << (ans == INF ? -1 : ans) << '\n';
}
