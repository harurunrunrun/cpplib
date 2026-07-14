// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/structure/tree/dynamic_forest_vertex_add_path_sum.hpp"

namespace{

constexpr int MAX_SIZE = 64;

template<class Tree>
void self_test_one(){
    Tree tree(std::vector<long long>{1, 2, 3});
    assert(tree.size() == 3);
    assert(!tree.connected(0, 2));
    assert(tree.link(0, 1));
    assert(tree.link(1, 2));
    assert(!tree.link(0, 2));
    assert(tree.path_sum(0, 2) == 6);

    tree.add_vertex(1, 4);
    assert(tree.path_sum(0, 2) == 10);
    assert(tree.replace_edge(1, 2, 0, 2));
    assert(tree.path_sum(1, 2) == 10);

    assert(!tree.replace_edge(0, 2, 0, 1));
    assert(tree.path_sum(1, 2) == 10);
    assert(!tree.cut(1, 2));
    assert(tree.cut(0, 2));

    bool thrown = false;
    try{
        static_cast<void>(tree.path_sum(1, 2));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    self_test_one<
        DynamicForestVertexAddPathSumLinkCutTree<long long, MAX_SIZE>
    >();
    self_test_one<
        DynamicForestVertexAddPathSumLazyLinkCutTree<long long, MAX_SIZE>
    >();
    self_test_one<
        DynamicForestVertexAddPathSumTopTree<long long, MAX_SIZE>
    >();
    self_test_one<
        DynamicForestVertexAddPathSumLazyTopTree<long long, MAX_SIZE>
    >();
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }

    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;

    DynamicForestVertexAddPathSumLinkCutTree<long long, MAX_SIZE> link_cut(values);
    DynamicForestVertexAddPathSumLazyLinkCutTree<long long, MAX_SIZE> lazy_link_cut(values);
    DynamicForestVertexAddPathSumTopTree<long long, MAX_SIZE> top_tree(values);
    DynamicForestVertexAddPathSumLazyTopTree<long long, MAX_SIZE> lazy_top_tree(values);

    for(int edge = 1; edge < n; ++edge){
        int left, right;
        std::cin >> left >> right;
        assert(link_cut.link(left, right));
        assert(lazy_link_cut.link(left, right));
        assert(top_tree.link(left, right));
        assert(lazy_top_tree.link(left, right));
    }

    while(q-- != 0){
        int type;
        std::cin >> type;
        if(type == 0){
            int old_left, old_right, new_left, new_right;
            std::cin >> old_left >> old_right >> new_left >> new_right;
            assert(link_cut.replace_edge(old_left, old_right, new_left, new_right));
            assert(lazy_link_cut.replace_edge(old_left, old_right, new_left, new_right));
            assert(top_tree.replace_edge(old_left, old_right, new_left, new_right));
            assert(lazy_top_tree.replace_edge(old_left, old_right, new_left, new_right));
        }else if(type == 1){
            int vertex;
            long long delta;
            std::cin >> vertex >> delta;
            link_cut.add_vertex(vertex, delta);
            lazy_link_cut.add_vertex(vertex, delta);
            top_tree.add_vertex(vertex, delta);
            lazy_top_tree.add_vertex(vertex, delta);
        }else{
            int left, right;
            std::cin >> left >> right;
            const long long expected = link_cut.path_sum(left, right);
            assert(lazy_link_cut.path_sum(left, right) == expected);
            assert(top_tree.path_sum(left, right) == expected);
            assert(lazy_top_tree.path_sum(left, right) == expected);
            std::cout << expected << '\n';
        }
    }
}
