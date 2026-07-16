// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/pairwise_gcd_sums.hpp"

int main(){
    std::vector<int> upper_bounds;
    for(int upper_bound; std::cin >> upper_bound && upper_bound != 0;){
        upper_bounds.push_back(upper_bound);
    }
    for(const auto value: math::pairwise_gcd_sums(upper_bounds)) std::cout << value << '\n';
}
