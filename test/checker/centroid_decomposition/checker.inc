#include <cassert>
#include <stdexcept>
#include <vector>

#include "../../../src/algorithm/tree/centroid_decomposition.hpp"

void add_edge(std::vector<std::vector<int>>& graph, int u, int v){
    graph[u].push_back(v);
    graph[v].push_back(u);
}

void test_path(){
    std::vector<std::vector<int>> graph(7);
    for(int v = 0; v + 1 < 7; v++){
        add_edge(graph, v, v + 1);
    }

    CentroidDecomposition cd(graph);
    assert(cd.size() == 7);
    assert(cd.root() == 3);
    assert(cd.parent(3) == -1);
    assert(cd.depth(3) == 0);

    assert(cd.parent(1) == 3);
    assert(cd.parent(5) == 3);
    assert(cd.parent(0) == 1);
    assert(cd.parent(2) == 1);
    assert(cd.parent(4) == 5);
    assert(cd.parent(6) == 5);

    const auto& tree = cd.tree();
    int edges = 0;
    for(const auto& adjacency: tree){
        edges += (int)adjacency.size();
    }
    assert(edges == 2 * 6);
}

void test_star(){
    std::vector<std::vector<int>> graph(6);
    for(int v = 1; v < 6; v++){
        add_edge(graph, 0, v);
    }

    CentroidDecomposition cd(graph);
    assert(cd.root() == 0);
    for(int v = 1; v < 6; v++){
        assert(cd.parent(v) == 0);
        assert(cd.depth(v) == 1);
    }
}

void test_empty_and_single(){
    CentroidDecomposition empty;
    assert(empty.size() == 0);
    assert(empty.root() == -1);

    std::vector<std::vector<int>> graph(1);
    CentroidDecomposition one(graph);
    assert(one.root() == 0);
    assert(one.parent(0) == -1);
    assert(one.depth(0) == 0);
}

void test_exceptions(){
    bool thrown = false;
    try{
        std::vector<std::vector<int>> graph(3);
        add_edge(graph, 0, 1);
        CentroidDecomposition cd(graph);
        (void)cd;
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
        CentroidDecomposition cd(graph);
        (void)cd;
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        std::vector<std::vector<int>> graph(1);
        CentroidDecomposition cd(graph);
        (void)cd.parent(1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

inline void run_centroid_decomposition_standalone_check(){
    test_path();
    test_star();
    test_empty_and_single();
    test_exceptions();
}
