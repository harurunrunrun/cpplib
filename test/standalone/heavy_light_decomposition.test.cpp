// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/heavy_light_decomposition.hpp"

std::vector<int> collect(const HeavyLightDecomposition& hld, const std::vector<std::pair<int, int>>& ranges){
    std::vector<int> res;
    for(auto [l, r]: ranges){
        for(int k = l; k < r; k++){
            res.push_back(hld[k]);
        }
    }
    std::sort(res.begin(), res.end());
    return res;
}

std::vector<int> collect_subtree(const HeavyLightDecomposition& hld, std::pair<int, int> range){
    std::vector<int> res;
    for(int k = range.first; k < range.second; k++){
        res.push_back(hld[k]);
    }
    std::sort(res.begin(), res.end());
    return res;
}

void test_fixed_tree(){
    HeavyLightDecomposition hld(9);
    hld.add_edge(0, 1);
    hld.add_edge(0, 2);
    hld.add_edge(1, 3);
    hld.add_edge(1, 4);
    hld.add_edge(2, 5);
    hld.add_edge(5, 6);
    hld.add_edge(5, 7);
    hld.add_edge(7, 8);
    hld.build(0);

    assert(hld.size() == 9);
    assert(hld.parent(0) == -1);
    assert(hld.parent(8) == 7);
    assert(hld.depth(8) == 4);
    assert(hld.subtree(5) == 4);

    assert(hld.lca(3, 4) == 1);
    assert(hld.lca(3, 8) == 0);
    assert(hld.lca(6, 8) == 5);
    assert(hld.dist(3, 8) == 6);
    assert(hld.dist(6, 8) == 3);

    assert(hld.jump(8, 0) == 8);
    assert(hld.jump(8, 1) == 7);
    assert(hld.jump(8, 3) == 2);
    assert(hld.jump(8, 4) == 0);

    assert(hld.is_ancestor(0, 8));
    assert(hld.is_ancestor(5, 8));
    assert(!hld.is_ancestor(1, 8));

    std::vector<int> path_vertices = {0, 1, 2, 3, 5, 7, 8};
    assert(collect(hld, hld.path_query(3, 8)) == path_vertices);

    std::vector<int> path_edges = {1, 2, 3, 5, 7, 8};
    assert(collect(hld, hld.path_query(3, 8, false)) == path_edges);

    std::vector<int> same = {4};
    assert(collect(hld, hld.path_query(4, 4)) == same);
    assert(hld.path_query(4, 4, false).empty());

    std::vector<int> subtree = {5, 6, 7, 8};
    assert(collect_subtree(hld, hld.subtree_query(5)) == subtree);
    std::vector<int> subtree_edges = {6, 7, 8};
    assert(collect_subtree(hld, hld.subtree_query(5, false)) == subtree_edges);
}

void test_root_change(){
    HeavyLightDecomposition hld(5);
    hld.add_edge(0, 1);
    hld.add_edge(1, 2);
    hld.add_edge(1, 3);
    hld.add_edge(3, 4);
    hld.build(3);

    assert(hld.parent(3) == -1);
    assert(hld.lca(0, 2) == 1);
    assert(hld.lca(0, 4) == 3);
    assert(hld.dist(2, 4) == 3);
    assert(hld.jump(0, 2) == 3);
}

void test_exceptions(){
    bool thrown = false;
    try{
        HeavyLightDecomposition bad(3);
        bad.add_edge(0, 1);
        bad.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        HeavyLightDecomposition bad(3);
        bad.add_edge(0, 1);
        bad.add_edge(1, 2);
        bad.add_edge(2, 0);
        bad.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        HeavyLightDecomposition hld(1);
        hld.add_edge(0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    test_fixed_tree();
    test_root_change();
    test_exceptions();
}
