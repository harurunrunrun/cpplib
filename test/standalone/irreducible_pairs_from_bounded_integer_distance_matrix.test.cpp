// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/irreducible_pairs_from_bounded_integer_distance_matrix.hpp"
#include "../../src/algorithm/graph/irreducible_pairs_from_distance_matrix.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int comparison_mode;
        long long maximum_distance;
        int vertex_count;
        std::cin >> comparison_mode >> maximum_distance >> vertex_count;
        std::vector<std::vector<long long>> distances(
            static_cast<std::size_t>(vertex_count),
            std::vector<long long>(static_cast<std::size_t>(vertex_count))
        );
        for(auto& row: distances){
            for(long long& distance: row) std::cin >> distance;
        }

        bool fast_valid = true;
        if(comparison_mode == 2 && !distances.empty()){
            distances.back().pop_back();
        }
        std::vector<std::pair<int, int>> fast_result;
        try{
            fast_result =
                irreducible_pairs_from_bounded_integer_distance_matrix(
                    distances, maximum_distance
                );
        }catch(const std::invalid_argument&){
            fast_valid = false;
        }

        if(comparison_mode == 1){
            std::cout << (!fast_valid ? "OK\n" : "NG\n");
            continue;
        }

        bool ordinary_valid = true;
        std::vector<std::pair<int, int>> ordinary_result;
        try{
            ordinary_result =
                irreducible_pairs_from_distance_matrix(distances);
        }catch(const std::invalid_argument&){
            ordinary_valid = false;
        }
        std::cout << (fast_valid == ordinary_valid
                && (!fast_valid || fast_result == ordinary_result)
            ? "OK\n" : "NG\n");
    }
}
