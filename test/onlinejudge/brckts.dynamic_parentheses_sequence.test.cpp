// competitive-verifier: PROBLEM https://www.spoj.com/problems/BRCKTS/

#include <iostream>
#include <string>

#include "../../src/structure/segtree/dynamic_parentheses_sequence.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    for(int test = 1; test <= 10; test++){
        int size;
        std::string brackets;
        std::cin >> size >> brackets;
        (void)size;
        DynamicParenthesesSequence<30000> sequence(std::move(brackets));

        std::cout << "Test " << test << ":\n";
        int operation_count;
        std::cin >> operation_count;
        while(operation_count-- != 0){
            int position;
            std::cin >> position;
            if(position == 0){
                std::cout << (sequence.balanced() ? "YES\n" : "NO\n");
                continue;
            }
            sequence.toggle(position - 1);
        }
    }
}
