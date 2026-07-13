// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc259/tasks/abc259_g

#include <iostream>

#include "../../src/algorithm/matching/burn_bury.hpp"

int main(){
    int height, width;
    std::cin >> height >> width;
    BurnBury<long long> solver(height + width);
    constexpr long long INF = 1LL << 60;
    for(int row = 0; row < height; ++row){
        for(int column = 0; column < width; ++column){
            long long value;
            std::cin >> value;
            if(value >= 0){
                solver.add_pair_cost(
                    row,
                    height + column,
                    -value,
                    0,
                    -value,
                    -value
                );
            }else{
                const long long penalty = -value;
                solver.add_pair_cost(
                    row,
                    height + column,
                    penalty,
                    0,
                    INF,
                    penalty
                );
            }
        }
    }
    std::cout << -solver.solve().cost << '\n';
}
