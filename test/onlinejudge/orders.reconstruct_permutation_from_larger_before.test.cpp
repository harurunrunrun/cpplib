// competitive-verifier: PROBLEM https://www.spoj.com/problems/ORDERS/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/sequence/reconstruct_permutation_from_larger_before.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size;
        std::cin >> size;
        std::vector<unsigned int> larger_before(static_cast<std::size_t>(size));
        for(unsigned int& count: larger_before) std::cin >> count;
        const auto answer = reconstruct_permutation_from_larger_before(larger_before);
        for(int index = 0; index < size; ++index){
            if(index != 0) std::cout << ' ';
            std::cout << answer[static_cast<std::size_t>(index)];
        }
        std::cout << '\n';
    }
}
