// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/auxiliary_tree.hpp"

void self_test(){
    AuxiliaryTree<8> tree(7);
    tree.add_edge(0, 1);
    tree.add_edge(0, 2);
    tree.add_edge(1, 3);
    tree.add_edge(1, 4);
    tree.add_edge(2, 5);
    tree.add_edge(2, 6);
    tree.build();
    const std::vector<int> vertices = {3, 4, 6, 3};
    assert(tree.compress(vertices) == 6);
    assert(tree.vertex(0) == 0);
    assert(tree.parent_index(0) == -1);
    assert(tree.edge_length(0) == 0);
    assert(!tree.selected(0));
    assert(tree.lca(3, 6) == 0);
    assert(tree.dist(3, 6) == 4);
    assert(tree.compress(std::span<const int>()) == 0);
}

int main(){
    int n, query_count;
    if(!(std::cin >> n >> query_count)){
        self_test();
        return 0;
    }
    AuxiliaryTree<256> tree(n);
    for(int edge = 0; edge + 1 < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u, v);
    }
    tree.build();
    while(query_count--){
        int k;
        std::cin >> k;
        std::vector<int> vertices(static_cast<std::size_t>(k));
        for(int& vertex: vertices) std::cin >> vertex;
        const int size = tree.compress(vertices);
        std::cout << size << '\n';
        for(int index = 0; index < size; ++index){
            const int parent_index = tree.parent_index(index);
            std::cout << tree.vertex(index) << ' '
                << (parent_index == -1 ? -1 : tree.vertex(parent_index)) << ' '
                << tree.edge_length(index) << ' '
                << static_cast<int>(tree.selected(index)) << '\n';
        }
    }
}
