// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/palindromes_in_deque

#include <iostream>
#include <memory>
#include "../../src/algorithm/string/deque_palindromic_tree.hpp"

int main(){
    int query_count;
    std::cin >> query_count;

    using Tree = DequePalindromicTree<26, 500002, 500000>;
    auto tree = std::make_unique<Tree>();
    while(query_count--){
        int type;
        std::cin >> type;
        if(type == 0){
            char c;
            std::cin >> c;
            tree->push_front(c);
        }else if(type == 1){
            char c;
            std::cin >> c;
            tree->push_back(c);
        }else if(type == 2){
            tree->pop_front();
        }else{
            tree->pop_back();
        }

        const auto result = tree->statistics();
        std::cout << result.distinct_palindromes << ' '
                  << result.longest_prefix << ' '
                  << result.longest_suffix << '\n';
    }
}
