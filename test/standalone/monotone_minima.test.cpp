// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/other/utility/monotone_minima.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if(!monotone_minima(0, 0, [](int, int){ return 0; }).empty()){
        throw std::runtime_error("empty monotone minima failed");
    }
    bool rejected = false;
    try{
        static_cast<void>(
            monotone_minima(1, 0, [](int, int){ return 0; })
        );
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected){
        throw std::runtime_error("empty columns were accepted");
    }

    int case_count;
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        int row_count, column_count;
        std::cin >> row_count >> column_count;
        std::vector<std::vector<long long>> matrix(
            static_cast<std::size_t>(row_count),
            std::vector<long long>(static_cast<std::size_t>(column_count))
        );
        for(auto& row: matrix){
            for(long long& element: row) std::cin >> element;
        }
        const auto result = monotone_minima(
            row_count,
            column_count,
            [&](int row, int column){
                return matrix[static_cast<std::size_t>(row)]
                             [static_cast<std::size_t>(column)];
            }
        );
        for(int row = 0; row < row_count; ++row){
            if(row) std::cout << ' ';
            std::cout << result[static_cast<std::size_t>(row)];
        }
        std::cout << '\n';
    }
}
