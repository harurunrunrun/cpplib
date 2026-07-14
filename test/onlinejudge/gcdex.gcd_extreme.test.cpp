// competitive-verifier: PROBLEM https://www.spoj.com/problems/GCDEX/
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/gcd_extreme.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::vector<int> queries;
    for(int n; std::cin >> n && n != 0;) queries.push_back(n);
    for(const auto value: math::gcd_extreme(queries)) std::cout << value << '\n';
}
