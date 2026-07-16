// competitive-verifier: PROBLEM https://www.spoj.com/problems/LSORT/
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/minimum_rank_removal_sort_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        int size;
        std::cin >> size;
        std::vector<int> permutation(static_cast<std::size_t>(size));
        for(int& value: permutation) std::cin >> value;
        std::cout << minimum_rank_removal_sort_cost(permutation) << '\n';
    }
    return 0;
}
