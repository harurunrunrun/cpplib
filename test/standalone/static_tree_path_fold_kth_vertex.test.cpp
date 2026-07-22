// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/tree/query/static_tree_path_fold.hpp"
#include "../../src/structure/types/monoid.hpp"

constexpr int kth_path_add(int left, int right){
    return left + right;
}

constexpr int kth_path_zero(){
    return 0;
}

constexpr Monoid<kth_path_add, kth_path_zero> kth_path_monoid{};

void self_test(){
    StaticTreePathFold<kth_path_monoid, 2> tree(2);
    tree.add_edge(0, 1, 0);
    tree.build();
    assert(tree.kth_vertex_on_path(0, 1, 0) == 0);
    assert(tree.kth_vertex_on_path(0, 1, 1) == 1);

    bool thrown = false;
    try{
        (void)tree.kth_vertex_on_path(0, 1, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int vertex_count, root, query_count;
    if(!(std::cin >> vertex_count >> root >> query_count)) return 0;
    StaticTreePathFold<kth_path_monoid, 100000> tree(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        tree.add_edge(left, right, 0);
    }
    tree.build(root);
    while(query_count-- > 0){
        int left, right, index;
        std::cin >> left >> right >> index;
        std::cout << tree.kth_vertex_on_path(left, right, index) << '\n';
    }
}
