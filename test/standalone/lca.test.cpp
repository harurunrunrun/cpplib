// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/query/lca.hpp"
#include "../../src/algorithm/tree/query/offline_lca.hpp"

int naive_lca(int u, int v, const std::vector<int>& parent, const std::vector<int>& depth){
    while(depth[u] > depth[v]){
        u = parent[u];
    }
    while(depth[v] > depth[u]){
        v = parent[v];
    }
    while(u != v){
        u = parent[u];
        v = parent[v];
    }
    return u;
}

void add_edge(std::vector<std::vector<int>>& graph, int u, int v){
    graph[u].push_back(v);
    graph[v].push_back(u);
}

void test_lca_and_jump(){
    const int n = 17;
    std::vector<int> parent(n, -1), depth(n, 0);
    std::vector<std::vector<int>> graph(n);
    LowestCommonAncestor lca(n);
    for(int v = 1; v < n; v++){
        int p = (v - 1) / 2;
        parent[v] = p;
        depth[v] = depth[p] + 1;
        lca.add_edge(p, v);
        add_edge(graph, p, v);
    }
    lca.build();

    assert(lca.size() == n);
    assert(lca.parent(0) == -1);
    assert(lca.parent(16) == 7);
    assert(lca.depth(16) == 4);
    assert(lca.jump(16, 2) == 3);
    assert(lca.jump(16, 4) == 0);

    std::vector<std::pair<int, int>> queries;
    for(int u = 0; u < n; u++){
        for(int v = 0; v < n; v++){
            int expected = naive_lca(u, v, parent, depth);
            assert(lca.lca(u, v) == expected);
            assert(lca.dist(u, v) == depth[u] + depth[v] - 2 * depth[expected]);
            assert(lca.path_vertex_count(u, v) ==
                   depth[u] + depth[v] - 2 * depth[expected] + 1);
            queries.push_back({u, v});
        }
    }
    auto answers = offline_lca(graph, queries);
    for(int i = 0; i < (int)queries.size(); i++){
        auto [u, v] = queries[i];
        assert(answers[i] == naive_lca(u, v, parent, depth));
    }
}

void test_root_change(){
    std::vector<std::vector<int>> graph(7);
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 3, 4);
    add_edge(graph, 3, 5);
    add_edge(graph, 5, 6);

    LowestCommonAncestor lca(7);
    for(int v = 0; v < 7; v++){
        for(int to: graph[v]){
            if(v < to){
                lca.add_edge(v, to);
            }
        }
    }
    lca.build(3);
    assert(lca.parent(3) == -1);
    assert(lca.lca(0, 2) == 1);
    assert(lca.lca(0, 6) == 3);
    assert(lca.lca(4, 6) == 3);
    assert(lca.dist(2, 6) == 4);
    assert(lca.path_vertex_count(2, 6) == 5);

    std::vector<std::pair<int, int>> queries = {{0, 2}, {0, 6}, {4, 6}, {3, 3}};
    auto answers = offline_lca(graph, queries, 3);
    std::vector<int> expected = {1, 3, 3, 3};
    assert(answers == expected);
}

void test_degenerate_trees(){
    LowestCommonAncestor empty;
    assert(empty.size() == 0);
    empty.build();

    LowestCommonAncestor singleton(1);
    singleton.build();
    assert(singleton.size() == 1);
    assert(singleton.parent(0) == -1);
    assert(singleton.depth(0) == 0);
    assert(singleton.jump(0, 0) == 0);
    assert(singleton.lca(0, 0) == 0);
    assert(singleton.dist(0, 0) == 0);
    assert(singleton.path_vertex_count(0, 0) == 1);
}

void test_rebuild(){
    LowestCommonAncestor lca(9);
    for(int v = 1; v < 9; v++){
        lca.add_edge(v - 1, v);
    }
    lca.build(0);
    assert(lca.lca(2, 7) == 2);
    assert(lca.jump(7, 5) == 2);

    lca.build(8);
    assert(lca.lca(2, 7) == 7);
    assert(lca.jump(2, 5) == 7);
    for(int u = 0; u < 9; u++){
        for(int v = 0; v < 9; v++){
            assert(lca.dist(u, v) == (u < v ? v - u : u - v));
        }
    }
}

void test_exceptions(){
    bool thrown = false;
    try{
        LowestCommonAncestor bad(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        LowestCommonAncestor unbuilt(1);
        (void)unbuilt.lca(0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        LowestCommonAncestor bad(3);
        bad.add_edge(0, 1);
        bad.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        std::vector<std::vector<int>> graph(3);
        add_edge(graph, 0, 1);
        (void)offline_lca(graph, {{0, 1}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        LowestCommonAncestor lca(2);
        lca.add_edge(0, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        LowestCommonAncestor lca(2);
        lca.add_edge(0, 1);
        lca.add_edge(0, 1);
        lca.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    test_lca_and_jump();
    test_root_change();
    test_degenerate_trees();
    test_rebuild();
    test_exceptions();

    int input_n, root, q;
    if(std::cin >> input_n >> root >> q){
        LowestCommonAncestor lca(input_n);
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(input_n));
        for(int i = 0; i + 1 < input_n; i++){
            int u, v;
            std::cin >> u >> v;
            lca.add_edge(u, v);
            add_edge(graph, u, v);
        }
        lca.build(root);
        std::vector<std::pair<int, int>> queries(static_cast<std::size_t>(q));
        for(auto& [u, v]: queries) std::cin >> u >> v;
        const auto offline = offline_lca(graph, queries, root);
        for(int i = 0; i < q; i++){
            const auto [u, v] = queries[static_cast<std::size_t>(i)];
            std::cout << lca.lca(u, v) << ' ' << lca.dist(u, v) << ' '
                << offline[static_cast<std::size_t>(i)] << ' ' << lca.path_vertex_count(u, v) << '\n';
        }
        return 0;
    }
}
