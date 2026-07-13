// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/tree_edge_split_diameters.hpp"

namespace{

template<class Function>
bool throws_runtime_error(Function&& function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

void self_test(){
    {
        const std::vector<std::pair<int, int>> edges = {
            {0, 1}, {1, 2}, {2, 3}
        };
        const auto split = tree_edge_split_diameters<4>(4, edges);
        assert((split == std::vector<TreeEdgeSplitDiameter>{{0, 2}, {1, 1}, {2, 0}}));
        const auto answer = maximum_product_of_two_vertex_disjoint_tree_paths<4>(4, edges);
        assert((answer == TreeTwoDisjointPathsResult{1, 1, 1, 2, 1, 1}));
    }
    {
        const std::vector<std::pair<int, int>> edges = {{1, 0}};
        assert((tree_edge_split_diameters<2>(2, edges) ==
            std::vector<TreeEdgeSplitDiameter>{{0, 0}}));
        assert((maximum_product_of_two_vertex_disjoint_tree_paths<2>(2, edges) ==
            TreeTwoDisjointPathsResult{0, 0, 1, 0, 0, 0}));
    }
    {
        const std::vector<std::pair<int, int>> edges = {
            {1, 0}, {0, 2}, {3, 0}, {0, 4}
        };
        const auto split = tree_edge_split_diameters<5>(5, edges);
        assert((split == std::vector<TreeEdgeSplitDiameter>{{0, 2}, {2, 0}, {0, 2}, {2, 0}}));
        const auto answer = maximum_product_of_two_vertex_disjoint_tree_paths<5>(5, edges);
        assert(answer.product == 0);
        assert(answer.cut_edge_index == 0);
    }
    {
        const std::vector<std::pair<int, int>> no_edges;
        assert(tree_edge_split_diameters<1>(1, no_edges).empty());
        assert((maximum_product_of_two_vertex_disjoint_tree_paths<1>(1, no_edges) ==
            TreeTwoDisjointPathsResult{0, -1, -1, -1, 0, 0}));
    }
    assert(throws_runtime_error([]{
        (void)tree_edge_split_diameters<2>(0, {});
    }));
    assert(throws_runtime_error([]{
        (void)tree_edge_split_diameters<2>(3, {{0, 1}, {1, 2}});
    }));
    assert(throws_runtime_error([]{
        (void)tree_edge_split_diameters<3>(3, {{0, 1}});
    }));
    assert(throws_runtime_error([]{
        (void)tree_edge_split_diameters<3>(3, {{0, 1}, {1, 3}});
    }));
    assert(throws_runtime_error([]{
        (void)tree_edge_split_diameters<3>(3, {{0, 1}, {0, 1}});
    }));
    assert(throws_runtime_error([]{
        (void)tree_edge_split_diameters<2>(2, {{0, 0}});
    }));
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)){
        self_test();
        return 0;
    }
    while(case_count-- != 0){
        int vertex_count;
        bool detailed;
        std::cin >> vertex_count >> detailed;
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(vertex_count - 1)
        );
        for(auto& [left, right]: edges){
            std::cin >> left >> right;
        }

        const auto split = tree_edge_split_diameters<100000>(vertex_count, edges);
        const auto answer =
            maximum_product_of_two_vertex_disjoint_tree_paths<100000>(vertex_count, edges);
        std::cout << answer.product << ' ' << answer.cut_edge_index << ' '
                  << answer.cut_u << ' ' << answer.cut_v << ' '
                  << answer.first_diameter << ' ' << answer.second_diameter;
        if(detailed){
            for(const TreeEdgeSplitDiameter& current: split){
                std::cout << ' ' << current.first_diameter << ' ' << current.second_diameter;
            }
        }else{
            std::uint64_t first_hash = 0;
            std::uint64_t second_hash = 0;
            for(std::size_t edge = 0; edge < split.size(); ++edge){
                const std::uint64_t index = static_cast<std::uint64_t>(edge) + 1;
                first_hash += index * static_cast<std::uint64_t>(split[edge].first_diameter + 1);
                second_hash += index * static_cast<std::uint64_t>(split[edge].second_diameter + 1);
            }
            std::cout << ' ' << first_hash << ' ' << second_hash;
        }
        std::cout << '\n';
    }
}
