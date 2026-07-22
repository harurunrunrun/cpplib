// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <tuple>
#include <vector>
#include "../../src/algorithm/graph/spanning_tree/prim_mst.hpp"

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
    bool merge(int a, int b){
        a = leader(a);
        b = leader(b);
        if(a == b) return false;
        p[static_cast<std::size_t>(b)] = a;
        return true;
    }
};

std::pair<long long, bool> kruskal_brute(int n, std::vector<std::tuple<int, int, long long>> edges){
    std::sort(edges.begin(), edges.end(), [](auto a, auto b){
        return std::get<2>(a) < std::get<2>(b);
    });
    Dsu dsu(n);
    long long cost = 0;
    int count = 0;
    for(auto [u, v, w]: edges){
        if(dsu.merge(u, v)){
            cost += w;
            count++;
        }
    }
    return {cost, count == std::max(0, n - 1)};
}

void self_test(){
    {
        std::vector<std::vector<PrimEdge<long long>>> graph(4);
        auto add = [&](int u, int v, long long w){
            graph[static_cast<std::size_t>(u)].push_back({v, w});
            graph[static_cast<std::size_t>(v)].push_back({u, w});
        };
        add(0, 1, 1);
        add(1, 2, 2);
        add(0, 2, 10);
        add(2, 3, 3);
        auto res = prim_mst<long long>(graph);
        assert(res.connected);
        assert(res.cost == 6);
        assert(res.edges.size() == 3);
    }
    std::mt19937 rng(20260814);
    for(int n = 0; n <= 30; n++){
        for(int step = 0; step < 80; step++){
            std::vector<std::vector<PrimEdge<long long>>> graph(static_cast<std::size_t>(n));
            std::vector<std::tuple<int, int, long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 4 == 0){
                        long long w = static_cast<int>(rng() % 50) - 20;
                        edges.push_back({u, v, w});
                        graph[static_cast<std::size_t>(u)].push_back({v, w});
                        graph[static_cast<std::size_t>(v)].push_back({u, w});
                    }
                }
            }
            auto expected = kruskal_brute(n, edges);
            auto actual = prim_mst<long long>(graph);
            assert(actual.cost == expected.first);
            assert(actual.connected == expected.second);
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<std::vector<PrimEdge<long long>>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph[static_cast<std::size_t>(u)].push_back({v, w});
        graph[static_cast<std::size_t>(v)].push_back({u, w});
    }
    auto res = prim_mst<long long>(graph);
    std::cout << (res.connected ? res.cost : -1) << '\n';
}
