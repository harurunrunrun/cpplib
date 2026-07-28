// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/bipartite/dulmage_mendelsohn.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int left_size;
        int right_size;
        int edge_count;
        std::cin >> left_size >> right_size >> edge_count;
        DulmageMendelsohnDecomposition decomposition(
            left_size, right_size
        );
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [left, right]: edges){
            std::cin >> left >> right;
            decomposition.add_edge(left, right);
        }
        DulmageMendelsohnResult result = decomposition.solve();
        int left_deficient = 0;
        int balanced = 0;
        int right_deficient = 0;
        auto count_part = [&](DulmageMendelsohnPart part){
            if(part == DulmageMendelsohnPart::left_deficient){
                ++left_deficient;
            }else if(part == DulmageMendelsohnPart::balanced){
                ++balanced;
            }else{
                ++right_deficient;
            }
        };
        for(auto part: result.left_part) count_part(part);
        for(auto part: result.right_part) count_part(part);
        for(int left = 0; left < left_size; ++left){
            int right = result.left_match[static_cast<std::size_t>(left)];
            if(right == -1) continue;
            if(result.right_match[static_cast<std::size_t>(right)] != left){
                return 2;
            }
            bool exists = false;
            for(auto edge: edges){
                if(edge.first == left && edge.second == right) exists = true;
            }
            if(!exists) return 3;
        }
        std::cout << result.matching_size << ' '
                  << left_deficient << ' ' << balanced << ' '
                  << right_deficient << '\n';
    }
}
