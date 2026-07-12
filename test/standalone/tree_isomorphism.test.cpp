// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/tree/tree_isomorphism.hpp"

void add_edge(std::vector<std::vector<int>>& graph, int u, int v){
    graph[u].push_back(v);
    graph[v].push_back(u);
}

void test_centers(){
    std::vector<std::vector<int>> path(6);
    for(int v = 0; v + 1 < 6; v++){
        add_edge(path, v, v + 1);
    }
    assert((tree_centers(path) == std::vector<int>{2, 3}));

    std::vector<std::vector<int>> star(5);
    for(int v = 1; v < 5; v++){
        add_edge(star, 0, v);
    }
    assert(tree_centers(star) == std::vector<int>{0});
    assert(tree_centers({}).empty());
}

void test_unrooted_isomorphism(){
    std::vector<std::vector<int>> a(8);
    add_edge(a, 0, 1);
    add_edge(a, 0, 2);
    add_edge(a, 1, 3);
    add_edge(a, 1, 4);
    add_edge(a, 2, 5);
    add_edge(a, 5, 6);
    add_edge(a, 5, 7);

    std::vector<std::vector<int>> b(8);
    add_edge(b, 4, 6);
    add_edge(b, 4, 2);
    add_edge(b, 6, 1);
    add_edge(b, 6, 0);
    add_edge(b, 2, 3);
    add_edge(b, 3, 7);
    add_edge(b, 3, 5);

    assert(tree_isomorphic(a, b));

    std::vector<std::vector<int>> path(8);
    for(int v = 0; v + 1 < 8; v++){
        add_edge(path, v, v + 1);
    }
    assert(!tree_isomorphic(a, path));
    assert(tree_isomorphic({}, {}));
}

void test_rooted_isomorphism(){
    std::vector<std::vector<int>> a(5);
    add_edge(a, 0, 1);
    add_edge(a, 1, 2);
    add_edge(a, 1, 3);
    add_edge(a, 3, 4);

    std::vector<std::vector<int>> b(5);
    add_edge(b, 4, 3);
    add_edge(b, 3, 2);
    add_edge(b, 3, 1);
    add_edge(b, 1, 0);

    assert(rooted_tree_isomorphic(a, 0, b, 4));
    assert(rooted_tree_isomorphic(a, 1, b, 3));
    assert(!rooted_tree_isomorphic(a, 0, b, 3));
}

void test_labels(){
    std::vector<std::vector<int>> graph(7);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 1, 4);
    add_edge(graph, 2, 5);
    add_edge(graph, 2, 6);

    auto labels = rooted_tree_isomorphism_labels(graph, 0);
    assert(labels[3] == labels[4]);
    assert(labels[5] == labels[6]);
    assert(labels[1] == labels[2]);
    assert(labels[0] != labels[1]);
}

void test_exceptions(){
    bool thrown = false;
    try{
        std::vector<std::vector<int>> graph(3);
        add_edge(graph, 0, 1);
        (void)tree_centers(graph);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        std::vector<std::vector<int>> graph(3);
        add_edge(graph, 0, 1);
        add_edge(graph, 1, 2);
        add_edge(graph, 2, 0);
        (void)tree_isomorphic(graph, graph);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        std::vector<std::vector<int>> graph(1);
        (void)rooted_tree_isomorphism_labels(graph, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int case_count;
    if(std::cin >> case_count){
        while(case_count--){
            int n, m, root_a, root_b;
            std::cin >> n >> m >> root_a >> root_b;
            std::vector<std::vector<int>> a(n), b(m);
            for(int i = 0; i + 1 < n; i++){
                int u, v;
                std::cin >> u >> v;
                add_edge(a, u, v);
            }
            for(int i = 0; i + 1 < m; i++){
                int u, v;
                std::cin >> u >> v;
                add_edge(b, u, v);
            }
            const auto centers_a = tree_centers(a);
            const auto centers_b = tree_centers(b);
            std::cout << tree_isomorphic(a, b) << ' '
                      << rooted_tree_isomorphic(a, root_a, b, root_b) << ' '
                      << centers_a.size();
            for(int center: centers_a) std::cout << ' ' << center;
            std::cout << ' ' << centers_b.size();
            for(int center: centers_b) std::cout << ' ' << center;
            std::cout << '\n';
        }
        return 0;
    }
    test_centers();
    test_unrooted_isomorphism();
    test_rooted_isomorphism();
    test_labels();
    test_exceptions();
}
