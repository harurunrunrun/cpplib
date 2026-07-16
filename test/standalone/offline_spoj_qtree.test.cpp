// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/qtree.tree_edge_point_set_path_maximum.test.cpp"
#undef main

#include <cassert>
#include <limits>

void generic_boundary_test(){
    TreeEdgePointSetPathMaximum<int, 8> tree(4);
    assert(tree.add_edge(0, 1, -5) == 0);
    assert(tree.add_edge(1, 2, -2) == 1);
    assert(tree.add_edge(1, 3, -7) == 2);
    tree.build(2);

    assert(tree.size() == 4);
    assert(tree.path_maximum(0, 2) == -2);
    assert(tree.path_maximum(2, 3) == -2);
    assert(tree.path_maximum(1, 1) == std::numeric_limits<int>::lowest());

    tree.set_edge(1, -9);
    assert(tree.path_maximum(0, 2) == -5);
    assert(tree.path_maximum(2, 3) == -7);
}

int main(){
    generic_boundary_test();
    return onlinejudge_adapter_main();
}
