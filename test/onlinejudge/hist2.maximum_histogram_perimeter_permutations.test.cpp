// competitive-verifier: PROBLEM https://www.spoj.com/problems/HIST2/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/dynamic_programming/maximum_histogram_perimeter_permutations.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; while(std::cin>>n && n!=0){ std::vector<int> heights(n); for(int& x:heights) std::cin>>x;
        auto answer=maximum_histogram_perimeter_permutations(heights);
        std::cout<<answer.maximum_perimeter<<' '<<answer.permutation_count<<'\n'; }
}
