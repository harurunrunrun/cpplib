// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/twopaths.maximum_product_of_two_vertex_disjoint_tree_paths.test.cpp"
#undef main

#include <cassert>
#include <utility>
#include <vector>

void generic_boundary_test(){
    const auto singleton =
        maximum_product_of_two_vertex_disjoint_tree_paths<8>(1, {});
    assert(singleton.product == 0);
    assert(singleton.cut_edge_index == -1);

    const std::vector<std::pair<int, int>> chain = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}
    };
    const auto result =
        maximum_product_of_two_vertex_disjoint_tree_paths<8>(5, chain);
    assert(result.product == 2);
    assert(result.cut_edge_index == 1);
    assert(result.first_diameter == 1);
    assert(result.second_diameter == 2);
}

int main(){
    generic_boundary_test();
    return onlinejudge_adapter_main();
}
