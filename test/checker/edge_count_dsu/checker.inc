#define PROBLEM "https://example.com/standalone/edge_count_dsu"

#include <cassert>
#include <numeric>
#include <random>
#include <vector>
#include "../../../src/structure/dsu/edge_count_dsu.hpp"

struct NaiveGraph{
    int n;
    std::vector<std::pair<int, int>> edges;

    explicit NaiveGraph(int n): n(n){}

    void add_edge(int u, int v){
        edges.push_back({u, v});
    }

    std::vector<int> component(int start) const{
        std::vector<std::vector<int>> graph(n);
        for(auto [u, v]: edges){
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        std::vector<int> used(n);
        std::vector<int> stack{start};
        used[start] = 1;
        for(int i = 0; i < static_cast<int>(stack.size()); i++){
            int v = stack[i];
            for(int to: graph[v]){
                if(used[to]) continue;
                used[to] = 1;
                stack.push_back(to);
            }
        }
        return used;
    }

    bool same(int u, int v) const{
        return component(u)[v] != 0;
    }

    int component_size(int v) const{
        auto comp = component(v);
        int res = 0;
        for(int x: comp) res += x;
        return res;
    }

    int component_edges(int v) const{
        auto comp = component(v);
        int res = 0;
        for(auto [u, w]: edges){
            if(comp[u] && comp[w]) res++;
        }
        return res;
    }
};

inline void run_edge_count_dsu_standalone_check(){
    constexpr int N = 10;
    EdgeCountDSU<N> dsu(N);
    NaiveGraph naive(N);

    assert(dsu.size() == N);
    assert(dsu.groups() == N);
    for(int v = 0; v < N; v++){
        assert(dsu.component_size(v) == 1);
        assert(dsu.edges(v) == 0);
        assert(dsu.excess_edges(v) == 0);
        assert(dsu.is_tree(v));
        assert(!dsu.is_unicyclic(v));
    }

    std::mt19937 rng(987654321);

    for(int step = 0; step < 200; step++){
        int u = static_cast<int>(rng() % N);
        int v = static_cast<int>(rng() % N);
        bool before_same = naive.same(u, v);

        bool merged = dsu.add_edge(u, v);
        naive.add_edge(u, v);
        assert(merged == !before_same);

        int expected_groups = 0;
        std::vector<int> seen(N);
        for(int a = 0; a < N; a++){
            if(seen[a]) continue;
            expected_groups++;
            auto comp = naive.component(a);
            for(int b = 0; b < N; b++) seen[b] |= comp[b];
        }
        assert(dsu.groups() == expected_groups);

        for(int a = 0; a < N; a++){
            assert(dsu.component_size(a) == naive.component_size(a));
            assert(dsu.edges(a) == naive.component_edges(a));
            assert(dsu.excess_edges(a) == dsu.edges(a) - dsu.component_size(a) + 1);
            assert(dsu.is_tree(a) == (dsu.edges(a) == dsu.component_size(a) - 1));
            assert(dsu.is_unicyclic(a) == (dsu.edges(a) == dsu.component_size(a)));
            for(int b = 0; b < N; b++){
                assert(dsu.same(a, b) == naive.same(a, b));
            }
        }
    }
}
