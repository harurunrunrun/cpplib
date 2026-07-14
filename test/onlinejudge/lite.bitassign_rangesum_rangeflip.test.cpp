// competitive-verifier: PROBLEM https://www.spoj.com/problems/LITE/

#include <cstddef>
#include <iostream>

#include "../../src/structure/segtree/bitassign_rangesum_rangeflip.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int light_count, command_count;
    std::cin >> light_count >> command_count;
    Bit01WST lights(static_cast<std::size_t>(light_count));
    while(command_count-- != 0){
        int type, left, right;
        std::cin >> type >> left >> right;
        --left;
        if(type == 0){
            lights.flip(
                static_cast<std::size_t>(left),
                static_cast<std::size_t>(right)
            );
        }else{
            std::cout << lights.sum(
                static_cast<std::size_t>(left),
                static_cast<std::size_t>(right)
            ) << '\n';
        }
    }
}
