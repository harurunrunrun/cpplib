// competitive-verifier: PROBLEM https://www.spoj.com/problems/ROADNET/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/neighboring_town_pairs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int test = 0; test < test_count; test++){
        int town_count;
        std::cin >> town_count;
        std::vector<std::vector<long long>> distances(
            static_cast<std::size_t>(town_count),
            std::vector<long long>(static_cast<std::size_t>(town_count))
        );
        for(auto& row: distances){
            for(long long& distance: row) std::cin >> distance;
        }
        if(test != 0) std::cout << '\n';
        for(const auto& [left, right]: neighboring_town_pairs(distances)){
            std::cout << left + 1 << ' ' << right + 1 << '\n';
        }
    }
}
