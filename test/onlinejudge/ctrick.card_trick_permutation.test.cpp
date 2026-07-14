// competitive-verifier: PROBLEM https://www.spoj.com/problems/CTRICK/

#include <iostream>

#include "../../src/algorithm/other/card_trick_permutation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size;
        std::cin >> size;
        const auto answer = card_trick_permutation(size);
        for(int index = 0; index < size; ++index){
            if(index != 0) std::cout << ' ';
            std::cout << answer[static_cast<std::size_t>(index)];
        }
        std::cout << '\n';
    }
}
