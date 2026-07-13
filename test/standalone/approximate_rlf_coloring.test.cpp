// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/coloring.hpp"

namespace {

void require(bool condition){
    if(!condition) throw std::runtime_error("RLF coloring test failed");
}

template<class Exception, class Function>
void require_throws(Function&& function){
    bool thrown = false;
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        thrown = true;
    }
    require(thrown);
}

void check_result(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    const approximate::graph::ColoringResult& result
){
    require(result.colors.size() == vertex_count);
    std::vector<bool> seen(result.color_count, false);
    std::size_t distinct_colors = 0;
    for(const std::size_t color : result.colors){
        require(color < result.color_count);
        if(!seen[color]){
            seen[color] = true;
            ++distinct_colors;
        }
    }
    require(distinct_colors == result.color_count);
    for(const auto [left, right] : edges){
        if(left != right) require(result.colors[left] != result.colors[right]);
    }
}

void boundary_tests(){
    using approximate::graph::recursive_largest_first_coloring;
    require_throws<std::invalid_argument>([]{
        static_cast<void>(recursive_largest_first_coloring(1, {{0, 0}}));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(recursive_largest_first_coloring(2, {{0, 2}}));
    });
    const auto empty = recursive_largest_first_coloring(0, {});
    check_result(0, {}, empty);
    require(empty.color_count == 0);
    const auto duplicated = recursive_largest_first_coloring(
        3, {{0, 1}, {1, 0}, {0, 1}, {1, 2}}
    );
    const auto simple = recursive_largest_first_coloring(3, {{0, 1}, {1, 2}});
    require(duplicated.colors == simple.colors);
}

}  // namespace

int main(){
    using approximate::graph::recursive_largest_first_coloring;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t case_count = 0;
    std::cin >> case_count;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t identifier = 0;
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::cin >> identifier >> vertex_count >> edge_count;
        std::vector<std::pair<std::size_t, std::size_t>> edges(edge_count);
        for(auto& [left, right] : edges) std::cin >> left >> right;
        const auto result = recursive_largest_first_coloring(vertex_count, edges);
        check_result(vertex_count, edges, result);
        std::cout << "CASE " << identifier << ' ' << result.color_count;
        for(const std::size_t color : result.colors) std::cout << ' ' << color;
        std::cout << '\n';
    }
}
