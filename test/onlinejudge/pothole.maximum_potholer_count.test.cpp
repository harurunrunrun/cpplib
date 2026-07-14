// competitive-verifier: PROBLEM https://www.spoj.com/problems/POTHOLE/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/maximum_potholer_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int chamber_count;
        std::cin >> chamber_count;
        std::vector<std::pair<int, int>> corridors;
        for(int from = 0; from + 1 < chamber_count; from++){
            int degree;
            std::cin >> degree;
            while(degree--){
                int to;
                std::cin >> to;
                corridors.emplace_back(from, to - 1);
            }
        }
        std::cout << maximum_potholer_count(chamber_count, corridors) << '\n';
    }
}
