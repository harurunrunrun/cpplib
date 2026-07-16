// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/qtree2.weighted_tree_distance_kth_vertex.test.cpp"
#undef main

#include <cassert>

void generic_boundary_test(){
    WeightedTreeDistanceKthVertex<long long, 8> tree(5);
    tree.add_edge(0, 1, 4);
    tree.add_edge(1, 2, -10);
    tree.add_edge(1, 3, 7);
    tree.add_edge(3, 4, 2);
    tree.build(4);

    assert(tree.size() == 5);
    assert(tree.distance(2, 4) == -1);
    assert(tree.distance(4, 2) == -1);
    assert(tree.distance(3, 3) == 0);
    assert(tree.kth_vertex(2, 4, 0) == 2);
    assert(tree.kth_vertex(2, 4, 1) == 1);
    assert(tree.kth_vertex(2, 4, 2) == 3);
    assert(tree.kth_vertex(2, 4, 3) == 4);
    assert(tree.kth_vertex(4, 2, 3) == 2);
}

int main(){
    generic_boundary_test();
    return onlinejudge_adapter_main();
}
