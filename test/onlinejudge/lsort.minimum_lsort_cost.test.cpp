// competitive-verifier: PROBLEM https://www.spoj.com/problems/LSORT/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_lsort_cost.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n; std::cin>>n; std::vector<int> permutation(n);
        for(int& x:permutation) std::cin>>x;
        std::cout<<minimum_lsort_cost(permutation)<<'\n'; }
}
