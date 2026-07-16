// competitive-verifier: PROBLEM https://www.spoj.com/problems/BAT3/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/longest_decreasing_subsequence_with_pivot_reset.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n,pivot_index; std::cin>>n>>pivot_index; std::vector<int> values(n);
        for(int& value:values) std::cin>>value;
        std::cout<<longest_decreasing_subsequence_with_pivot_reset(values,pivot_index)<<'\n'; }
    return 0;
}
