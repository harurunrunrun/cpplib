// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/specialized/irreducible_pairs_from_distance_matrix.hpp"
#include "../../src/algorithm/graph/specialized/irreducible_pairs_from_distance_matrix_subcubic.hpp"

namespace{

void check_dominance_reduction(
    const std::vector<std::vector<long long>>& distances
){
    using irreducible_pairs_distance_matrix_subcubic_detail::BlockMinimizers;
    const int vertex_count = static_cast<int>(distances.size());
    const int largest_width = std::min(vertex_count, 6);
    const std::size_t area =
        static_cast<std::size_t>(vertex_count)
        * static_cast<std::size_t>(vertex_count);
    for(int width = 1; width <= largest_width; width++){
        BlockMinimizers finder(distances, 0, width, area);
        const std::vector<int> actual = finder.compute();
        for(int row = 0; row < vertex_count; row++){
            for(int column = 0; column < vertex_count; column++){
                int expected = 0;
                for(int candidate = 1; candidate < width; candidate++){
                    const __int128_t candidate_sum =
                        static_cast<__int128_t>(distances[row][candidate])
                        + distances[candidate][column];
                    const __int128_t expected_sum =
                        static_cast<__int128_t>(distances[row][expected])
                        + distances[expected][column];
                    if(candidate_sum < expected_sum) expected = candidate;
                }
                if(actual[
                        static_cast<std::size_t>(row)
                            * static_cast<std::size_t>(vertex_count)
                        + static_cast<std::size_t>(column)
                    ] != expected){
                    throw std::runtime_error(
                        "dominance block minimizer mismatch"
                    );
                }
            }
        }
    }
}
}  // namespace


int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int mode;
        int vertex_count;
        std::cin >> mode >> vertex_count;
        std::vector<std::vector<long long>> distances(
            static_cast<std::size_t>(vertex_count),
            std::vector<long long>(static_cast<std::size_t>(vertex_count))
        );
        for(auto& row: distances){
            for(long long& distance: row) std::cin >> distance;
        }
        if(mode == 2 && !distances.empty()){
            distances.back().pop_back();
        }

        bool fast_valid = true;
        std::vector<std::pair<int, int>> fast_result;
        try{
            fast_result =
                irreducible_pairs_from_distance_matrix_subcubic(
                    distances
                );
        }catch(const std::invalid_argument&){
            fast_valid = false;
        }

        bool naive_valid = true;
        std::vector<std::pair<int, int>> naive_result;
        try{
            naive_result =
                irreducible_pairs_from_distance_matrix(distances);
        }catch(const std::invalid_argument&){
            naive_valid = false;
        }
        static bool reduction_checked = false;
        if(mode == 0 && fast_valid && 6 <= vertex_count
                && !reduction_checked){
            check_dominance_reduction(distances);
            reduction_checked = true;
        }


        const bool valid = mode == 1 || mode == 2
            ? !fast_valid && !naive_valid
            : fast_valid && naive_valid && fast_result == naive_result;
        std::cout << (valid ? "OK\n" : "NG\n");
    }
}
