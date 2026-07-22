// competitive-verifier: PROBLEM https://www.spoj.com/problems/GCDEX/

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/number_theory/pairwise_gcd_sums.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<int> upper_bounds;
    for(int upper_bound; std::cin >> upper_bound && upper_bound != 0;){
        upper_bounds.push_back(upper_bound);
    }
    for(const auto sum: math::pairwise_gcd_sums(upper_bounds)){
        std::cout << sum << '\n';
    }
}
