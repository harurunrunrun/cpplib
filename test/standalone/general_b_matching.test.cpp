// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/general/general_b_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int vertex_count;
        int edge_count;
        std::cin >> vertex_count >> edge_count;
        std::vector<int> capacity(
            static_cast<std::size_t>(vertex_count)
        );
        for(int& value: capacity) std::cin >> value;
        GeneralBMatching matching(capacity);
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [first, second]: edges){
            std::cin >> first >> second;
            matching.add_edge(first, second);
        }
        const auto result = matching.solve();
        std::vector<int> degree(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<unsigned char> selected(
            static_cast<std::size_t>(edge_count), 0
        );
        for(int edge_id: result.selected_edge_ids){
            if(edge_id < 0 || edge_count <= edge_id
                || selected[static_cast<std::size_t>(edge_id)] != 0){
                return 2;
            }
            selected[static_cast<std::size_t>(edge_id)] = 1;
            ++degree[static_cast<std::size_t>(
                edges[static_cast<std::size_t>(edge_id)].first
            )];
            ++degree[static_cast<std::size_t>(
                edges[static_cast<std::size_t>(edge_id)].second
            )];
        }
        if(static_cast<int>(result.selected_edge_ids.size())
            != result.size || degree != result.degree) return 3;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(degree[static_cast<std::size_t>(vertex)]
                > capacity[static_cast<std::size_t>(vertex)]) return 4;
        }
        std::cout << result.size << '\n';
    }
}
