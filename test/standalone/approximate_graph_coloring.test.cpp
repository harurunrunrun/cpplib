// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/coloring.hpp"

namespace {

void print_coloring(const approximate::graph::ColoringResult& result){
    std::cout << result.color_count << ' ' << result.colors.size();
    for(const std::size_t color : result.colors) std::cout << ' ' << color;
}

bool is_proper(
    const approximate::graph::ColoringResult& result,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    for(const auto [left, right] : edges){
        if(result.colors[left] == result.colors[right]) return false;
    }
    return true;
}

}  // namespace

int main(){
    using namespace approximate::graph;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool rejected = false;
    try{
        static_cast<void>(dsatur_coloring(1, {{0, 0}}));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 2;

    std::size_t query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::cin >> vertex_count >> edge_count;
        std::vector<std::pair<std::size_t, std::size_t>> edges(edge_count);
        for(auto& [left, right] : edges) std::cin >> left >> right;
        const ColoringResult plain = greedy_coloring(vertex_count, edges);
        const ColoringResult largest = largest_first_coloring(vertex_count, edges);
        const ColoringResult smallest = smallest_last_coloring(vertex_count, edges);
        const ColoringResult welsh = welsh_powell_coloring(vertex_count, edges);
        const ColoringResult saturation = dsatur_coloring(vertex_count, edges);
        if(!is_proper(plain, edges) || !is_proper(largest, edges) ||
           !is_proper(smallest, edges) || !is_proper(welsh, edges) ||
           !is_proper(saturation, edges)){
            return 3;
        }
        print_coloring(plain);
        std::cout << " | ";
        print_coloring(largest);
        std::cout << " | ";
        print_coloring(smallest);
        std::cout << " | ";
        print_coloring(welsh);
        std::cout << " | ";
        print_coloring(saturation);
        std::cout << '\n';
    }
}
