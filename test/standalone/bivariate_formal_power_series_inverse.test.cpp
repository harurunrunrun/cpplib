// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/formal_power_series/bivariate_formal_power_series_inverse.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Mint = Modint<998244353>;
    int case_count;
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        int row_count, column_count;
        std::cin >> row_count >> column_count;
        std::vector<std::vector<Mint>> series(
            static_cast<std::size_t>(row_count),
            std::vector<Mint>(static_cast<std::size_t>(column_count))
        );
        for(auto& row: series){
            for(Mint& coefficient: row){
                long long value;
                std::cin >> value;
                coefficient = Mint{value};
            }
        }
        const auto inverse = bivariate_formal_power_series_inverse(series);
        for(const auto& row: inverse){
            for(std::size_t column = 0; column < row.size(); ++column){
                if(column) std::cout << ' ';
                std::cout << row[column].val();
            }
            std::cout << '\n';
        }
    }
}
