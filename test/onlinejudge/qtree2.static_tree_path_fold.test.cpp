#define PROBLEM "https://www.spoj.com/problems/QTREE2/"

#include <iostream>
#include <string>

#include "../../src/algorithm/tree/static_tree_path_fold.hpp"
#include "../../src/structure/types/monoid.hpp"

constexpr long long qtree2_add(long long left, long long right){
    return left + right;
}

constexpr long long qtree2_zero(){
    return 0;
}

constexpr Monoid<qtree2_add, qtree2_zero> qtree2_sum_monoid{};

using QTree2 = StaticTreePathFold<qtree2_sum_monoid, 10000>;

int qtree2_kth_vertex(const QTree2& tree, int left, int right, int k){
    const int ancestor = tree.lca(left, right);
    const int upward_edges = tree.depth(left) - tree.depth(ancestor);
    const int downward_edges = tree.depth(right) - tree.depth(ancestor);
    const int offset = k - 1;
    if(offset <= upward_edges) return tree.jump(left, offset);
    return tree.jump(right, upward_edges + downward_edges - offset);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int vertex_count;
        std::cin >> vertex_count;
        QTree2 tree(vertex_count);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            long long weight;
            std::cin >> left >> right >> weight;
            tree.add_edge(left - 1, right - 1, weight);
        }
        tree.build();

        std::string command;
        while(std::cin >> command && command != "DONE"){
            int left, right;
            std::cin >> left >> right;
            --left;
            --right;
            if(command == "DIST"){
                std::cout << tree.prod(left, right) << '\n';
            }else{
                int k;
                std::cin >> k;
                std::cout << qtree2_kth_vertex(tree, left, right, k) + 1
                          << '\n';
            }
        }
        std::cout << '\n';
    }
}
