// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/irreducible_pairs_from_distance_matrix.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count;
        std::cin >> vertex_count;
        std::vector<std::vector<long long>> distances(
            static_cast<std::size_t>(vertex_count),
            std::vector<long long>(static_cast<std::size_t>(vertex_count))
        );
        for(auto& row: distances){
            for(long long& distance: row) std::cin >> distance;
        }
        try{
            const auto pairs =
                irreducible_pairs_from_distance_matrix(distances);
            std::cout << pairs.size();
            for(const auto& [left, right]: pairs){
                std::cout << ' ' << left << ':' << right;
            }
            std::cout << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "INVALID\n";
        }
    }
}
