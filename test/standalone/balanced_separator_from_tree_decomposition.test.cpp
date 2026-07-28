// competitive-verifier: STANDALONE

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/specialized/structural_analysis/balanced_separator_from_tree_decomposition.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        int vertex_count, bag_count;
        std::cin >> vertex_count >> bag_count;
        std::vector<long long> weights(static_cast<std::size_t>(vertex_count));
        for(long long& weight: weights) std::cin >> weight;
        std::vector<std::vector<int>> bags(static_cast<std::size_t>(bag_count));
        for(auto& bag: bags){
            int size;
            std::cin >> size;
            bag.resize(static_cast<std::size_t>(size));
            for(int& vertex: bag) std::cin >> vertex;
        }
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(bag_count - 1));
        for(int index = 1; index < bag_count; ++index){
            int first, second;
            std::cin >> first >> second;
            edges.emplace_back(first, second);
        }
        const auto result = balanced_separator_from_tree_decomposition(
            bags, edges, weights
        );
        std::cout << result.bag << ' ' << result.vertices.size();
        for(const int vertex: result.vertices) std::cout << ' ' << vertex;
        std::cout << ' ' << result.maximum_component_weight << '\n';
    }
}
