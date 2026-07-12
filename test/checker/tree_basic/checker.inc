#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../../src/algorithm/tree/tree_centroid.hpp"
#include "../../../src/algorithm/tree/tree_diameter.hpp"

void add_edge(std::vector<std::vector<int>>& graph, int u, int v){
    graph[u].push_back(v);
    graph[v].push_back(u);
}

template<class T>
void add_weighted_edge(std::vector<std::vector<std::pair<int, T>>>& graph, int u, int v, T w){
    graph[u].push_back({v, w});
    graph[v].push_back({u, w});
}

void test_diameter(){
    std::vector<std::vector<int>> graph(9);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 1, 4);
    add_edge(graph, 2, 5);
    add_edge(graph, 5, 6);
    add_edge(graph, 5, 7);
    add_edge(graph, 7, 8);

    auto diameter = tree_diameter(graph);
    assert(diameter.length == 6);
    assert((diameter.from == 8 || diameter.to == 8));
    assert(diameter.path.size() == 7);
    assert(diameter.path.front() == diameter.from);
    assert(diameter.path.back() == diameter.to);

    std::vector<std::vector<std::pair<int, long long>>> weighted(5);
    add_weighted_edge(weighted, 0, 1, 4LL);
    add_weighted_edge(weighted, 1, 2, 7LL);
    add_weighted_edge(weighted, 1, 3, 2LL);
    add_weighted_edge(weighted, 3, 4, 5LL);
    auto wdiameter = weighted_tree_diameter(weighted);
    assert(wdiameter.length == 14);
    std::vector<int> expected_path = {2, 1, 3, 4};
    if(wdiameter.path != expected_path){
        std::reverse(expected_path.begin(), expected_path.end());
    }
    assert(wdiameter.path == expected_path);

    auto empty = tree_diameter({});
    assert(empty.from == -1);
    assert(empty.to == -1);
    assert(empty.length == 0);
    assert(empty.path.empty());
}

void test_centroid(){
    std::vector<std::vector<int>> star(6);
    for(int v = 1; v < 6; v++){
        add_edge(star, 0, v);
    }
    assert(tree_centroid(star) == 0);
    assert(tree_centroids(star) == std::vector<int>{0});

    std::vector<std::vector<int>> path(6);
    for(int v = 0; v + 1 < 6; v++){
        add_edge(path, v, v + 1);
    }
    auto centroids = tree_centroids(path);
    std::sort(centroids.begin(), centroids.end());
    assert((centroids == std::vector<int>{2, 3}));

    std::vector<std::vector<int>> one(1);
    assert(tree_centroids(one) == std::vector<int>{0});
    assert(tree_centroid({}) == -1);
}

void test_exceptions(){
    bool thrown = false;
    try{
        std::vector<std::vector<int>> graph(3);
        add_edge(graph, 0, 1);
        (void)tree_diameter(graph);
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
        (void)tree_centroids(graph);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

inline void run_tree_basic_standalone_check(){
    test_diameter();
    test_centroid();
    test_exceptions();
}
