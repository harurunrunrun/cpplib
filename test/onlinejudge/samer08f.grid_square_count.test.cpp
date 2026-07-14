// competitive-verifier: PROBLEM https://www.spoj.com/problems/SAMER08F/
#include <iostream>
#include "../../src/algorithm/math/grid_square_count.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    long long n; while(std::cin>>n && n!=0) std::cout<<grid_square_count(n)<<'\n';
}
