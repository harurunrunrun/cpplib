// competitive-verifier: PROBLEM https://www.codechef.com/problems/FLIPCOIN

#include <cstddef>
#include <iostream>

#include "../../src/structure/segtree/bitassign_rangesum_rangeflip.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int coin_count;
    int operation_count;
    std::cin >> coin_count >> operation_count;
    Bit01WST coins(static_cast<std::size_t>(coin_count));
    while(operation_count-- != 0){
        int type;
        int left;
        int right;
        std::cin >> type >> left >> right;
        const std::size_t range_left = static_cast<std::size_t>(left);
        const std::size_t range_right = static_cast<std::size_t>(right) + 1;
        if(type == 0){
            coins.flip(range_left, range_right);
        }else{
            std::cout << coins.sum(range_left, range_right) << '\n';
        }
    }
}
