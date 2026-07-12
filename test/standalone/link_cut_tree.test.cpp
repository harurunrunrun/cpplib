// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <queue>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/tree/link_cut_tree.hpp"
#include "../../src/structure/types/monoid.hpp"

long long add_ll(long long a, long long b){ return a + b; }
long long zero_ll(){ return 0; }
constexpr Monoid<add_ll, zero_ll> sum_monoid;

std::string concat_string(std::string a, std::string b){ return a + b; }
std::string empty_string(){ return ""; }
constexpr Monoid<concat_string, empty_string> concat_monoid;

long long naive_path_sum(
    int s,
    int t,
    const std::vector<std::vector<int>>& graph,
    const std::vector<long long>& value
){
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
    long long result = 0;
    for(int v = t;; v = parent[v]){
        result += value[v];
        if(v == s) break;
    }
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

int main(){
    {
        std::vector<std::string> value = {"a", "b", "c"};
        LinkCutTree<concat_monoid, 10> tree(value);
        assert(tree.link(0, 1));
        assert(tree.link(1, 2));
        assert(tree.path_prod(0, 2) == "abc");
        assert(tree.path_prod(2, 0) == "cba");
    }

    constexpr int n = 50;
    std::mt19937 rng(424242);
    std::vector<long long> value(n);
    for(auto& x: value) x = static_cast<int>(rng() % 200) - 80;
    LinkCutTree<sum_monoid, 80> tree(value);

    std::vector<std::vector<int>> graph(n);
    std::vector<std::pair<int, int>> edges;
    for(int v = 1; v < n; v++){
        int p = static_cast<int>(rng() % v);
        assert(tree.link(v, p));
        graph[v].push_back(p);
        graph[p].push_back(v);
        edges.emplace_back(v, p);
    }

    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 10);
        if(type < 4){
            int v = static_cast<int>(rng() % n);
            long long delta = static_cast<int>(rng() % 101) - 40;
            value[v] += delta;
            tree.set(v, value[v]);
            assert(tree.get(v) == value[v]);
        }else if(type < 8){
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
