// competitive-verifier: PROBLEM https://www.spoj.com/problems/ORDERSET/

#include <iostream>

#include "../../src/structure/bbst/red_black_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int q;
    std::cin >> q;
    RedBlackSet<int, 200000> values;
    while(q--){
        char type;
        int argument;
        std::cin >> type >> argument;
        if(type == 'I'){
            values.insert(argument);
        }else if(type == 'D'){
            values.erase(argument);
        }else if(type == 'C'){
            std::cout << values.order_of_key(argument) << '\n';
        }else{
            const auto value = values.kth(argument - 1);
            if(value){
                std::cout << *value << '\n';
            }else{
                std::cout << "invalid\n";
            }
        }
    }
}
