// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/structure/tree/dynamic_tree_vertex_subtree_sum.hpp"

void self_test(){
    const std::vector<long long> values = {2, -1, 4, 3, 5};
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {1, 3}, {3, 4}
    };
    DynamicTreeVertexSubtreeSum<long long> tree(values, edges, 1);
    assert(tree.size() == 5);
    assert(tree.subtree_sum(3) == 8);
    assert(tree.subtree_sum(1) == 13);
    tree.add(4, -2);
    assert(tree.subtree_sum(3) == 6);
    assert(tree.subtree_sum(1) == 11);
    assert(tree.subtree_sum(0) == 2);

    DynamicTreeVertexSubtreeSum<long long> empty({}, {});
    assert(empty.size() == 0);

    bool thrown = false;
    try{
        (void)tree.subtree_sum(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        DynamicTreeVertexSubtreeSum<long long> invalid(
            {1, 2, 3},
            {{0, 1}, {1, 3}}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q, root;
    if(!(std::cin >> n >> q >> root)){
        self_test();
        return 0;
    }
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(n > 0 ? n - 1 : 0));
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        edges.push_back({u, v});
    }

    DynamicTreeVertexSubtreeSum<long long> tree(values, edges, root);
    while(q-- != 0){
        int type;
        std::cin >> type;
        if(type == 0){
            int vertex;
            long long delta;
            std::cin >> vertex >> delta;
            tree.add(vertex, delta);
        }else{
            int vertex;
            std::cin >> vertex;
            std::cout << tree.subtree_sum(vertex) << '\n';
        }
    }
}
