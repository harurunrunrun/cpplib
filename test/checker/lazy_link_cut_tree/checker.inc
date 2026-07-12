#include <algorithm>
#include <cassert>
#include <queue>
#include <random>
#include <utility>
#include <vector>
#include "../../../src/structure/tree/lazy_link_cut_tree.hpp"
#include "../../../src/structure/types/monoid_act_len.hpp"

long long op_sum(long long a, long long, long long b, long long){ return a + b; }
long long e_sum(){ return 0; }
long long mapping_add(long long f, long long x, long long len){ return x + f * len; }
long long composition_add(long long f, long long g){ return f + g; }
long long id_add(){ return 0; }
constexpr Monoid_Act_Len<op_sum, e_sum, mapping_add, composition_add, id_add> range_add_sum;

std::vector<int> path_vertices(int s, int t, const std::vector<std::vector<int>>& graph){
    int n = static_cast<int>(graph.size());
    std::vector<int> parent(n, -1);
    std::queue<int> que;
    parent[s] = s;
    que.push(s);
    while(!que.empty()){
        int v = que.front();
        que.pop();
        if(v == t) break;
        for(int to: graph[v]){
            if(parent[to] == -1){
                parent[to] = v;
                que.push(to);
            }
        }
    }
    assert(parent[t] != -1);
    std::vector<int> result;
    for(int v = t;; v = parent[v]){
        result.push_back(v);
        if(v == s) break;
    }
    return result;
}

long long naive_path_sum(
    int s,
    int t,
    const std::vector<std::vector<int>>& graph,
    const std::vector<long long>& value
){
    auto path = path_vertices(s, t, graph);
    long long result = 0;
    for(int v: path) result += value[v];
    return result;
}

std::vector<int> component(int s, const std::vector<std::vector<int>>& graph){
    int n = static_cast<int>(graph.size());
    std::vector<int> seen(n, 0), result;
    std::queue<int> que;
    seen[s] = 1;
    que.push(s);
    while(!que.empty()){
        int v = que.front();
        que.pop();
        result.push_back(v);
        for(int to: graph[v]){
            if(!seen[to]){
                seen[to] = 1;
                que.push(to);
            }
        }
    }
    return result;
}

inline void run_lazy_link_cut_tree_standalone_check(){
    {
        std::vector<long long> value = {1, 2, 3};
        LazyLinkCutTree<range_add_sum, 10> tree(value);
        assert(tree.link(0, 1));
        assert(tree.link(1, 2));
        assert(tree.path_prod(0, 2) == 6);
        tree.path_apply(2, 0, 5);
        assert(tree.path_prod(0, 2) == 21);
        assert(tree.get(1) == 7);
    }

    constexpr int n = 50;
    std::mt19937 rng(20260712);
    std::vector<long long> value(n);
    for(auto& x: value) x = static_cast<int>(rng() % 200) - 80;
    LazyLinkCutTree<range_add_sum, 80> tree(value);

    std::vector<std::vector<int>> graph(n);
    std::vector<std::pair<int, int>> edges;
    for(int v = 1; v < n; v++){
        int p = static_cast<int>(rng() % v);
        assert(tree.link(v, p));
        graph[v].push_back(p);
        graph[p].push_back(v);
        edges.emplace_back(v, p);
    }

    for(int step = 0; step < 4000; step++){
        int type = static_cast<int>(rng() % 12);
        if(type < 4){
            int u = static_cast<int>(rng() % n);
            int v = static_cast<int>(rng() % n);
            long long delta = static_cast<int>(rng() % 101) - 50;
            tree.path_apply(u, v, delta);
            for(int x: path_vertices(u, v, graph)) value[x] += delta;
        }else if(type < 6){
            int v = static_cast<int>(rng() % n);
            long long x = static_cast<int>(rng() % 200) - 80;
            value[v] = x;
            tree.set(v, x);
            assert(tree.get(v) == value[v]);
        }else if(type < 10){
            int u = static_cast<int>(rng() % n);
            int v = static_cast<int>(rng() % n);
            assert(tree.path_prod(u, v) == naive_path_sum(u, v, graph, value));
            assert(tree.connected(u, v));
        }else{
            int id = static_cast<int>(rng() % edges.size());
            auto [a, b] = edges[id];
            assert(tree.cut(a, b));
            graph[a].erase(std::find(graph[a].begin(), graph[a].end(), b));
            graph[b].erase(std::find(graph[b].begin(), graph[b].end(), a));
            edges.erase(edges.begin() + id);

            auto comp = component(a, graph);
            std::vector<int> in_a(n, 0);
            for(int v: comp) in_a[v] = 1;
            int u = comp[static_cast<std::size_t>(rng() % comp.size())];
            std::vector<int> other;
            for(int v = 0; v < n; v++) if(!in_a[v]) other.push_back(v);
            int v = other[static_cast<std::size_t>(rng() % other.size())];
            assert(!tree.connected(u, v));
            assert(tree.link(u, v));
            graph[u].push_back(v);
            graph[v].push_back(u);
            edges.emplace_back(u, v);
        }
    }
}
