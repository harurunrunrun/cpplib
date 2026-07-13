#define PROBLEM "https://www.spoj.com/problems/ADAORANG/"

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/algorithm/range/static_range_count_distinct.hpp"
#include "../../src/algorithm/tree/euler_tour.hpp"
#include "../../src/algorithm/tree/lca.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int vertex_count, query_count, root;
        std::cin >> vertex_count >> query_count >> root;
        std::vector<int> color(static_cast<std::size_t>(vertex_count));
        for(int& value: color) std::cin >> value;

        auto tour = std::make_unique<TreeEulerTour<450000>>(vertex_count);
        LowestCommonAncestor lca(vertex_count);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            tour->add_edge(left, right);
            lca.add_edge(left, right);
        }
        tour->build(root);
        lca.build(root);

        std::vector<int> preorder_color(static_cast<std::size_t>(vertex_count));
        for(int index = 0; index < vertex_count; ++index){
            preorder_color[static_cast<std::size_t>(index)] = color[
                static_cast<std::size_t>(tour->preorder_vertex(index))
            ];
        }
        StaticRangeCountDistinct<int, 450000> distinct(preorder_color);

        while(query_count-- != 0){
            int left, right;
            std::cin >> left >> right;
            const auto [subtree_left, subtree_right] = tour->subtree_range(
                lca.lca(left, right)
            );
            std::cout << distinct.count(subtree_left, subtree_right) << '\n';
        }
    }
}
