// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "../../src/structure/tree/dynamic_tree_edge_path_sum.hpp"

void self_test(){
    DynamicTreeEdgePathSum<long long> tree(
        5,
        {{0, 1, 2}, {1, 2, -3}, {1, 3, 5}, {3, 4, 7}},
        3
    );
    assert(tree.size() == 5);
    assert(tree.edge_count() == 4);
    assert(tree.path_sum(2, 4) == 9);
    tree.set_edge(1, 4);
    assert(tree.edge_value(1) == 4);
    assert(tree.path_sum(2, 4) == 16);
    assert(tree.path_sum(3, 3) == 0);

    DynamicTreeEdgePathSum<long long> empty(0, {});
    assert(empty.size() == 0);

    bool thrown = false;
    try{
        tree.set_edge(4, 1);
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
    std::vector<std::tuple<int, int, long long>> edges;
    edges.reserve(static_cast<std::size_t>(n > 0 ? n - 1 : 0));
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        long long weight;
        std::cin >> u >> v >> weight;
        edges.push_back({u, v, weight});
    }
    DynamicTreeEdgePathSum<long long> tree(n, edges, root);
    while(q-- != 0){
        int type, first;
        long long second;
        std::cin >> type >> first >> second;
        if(type == 0){
            tree.set_edge(first, second);
        }else{
            std::cout << tree.path_sum(first, static_cast<int>(second)) << '\n';
        }
    }
}
