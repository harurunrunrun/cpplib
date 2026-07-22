// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/palindromes_in_deque

#include <iostream>
#include <memory>
#include "../../src/algorithm/string/palindrome/deque_palindrome_queries.hpp"

int main(){
    int query_count;
    std::cin >> query_count;

    using Tree = DequePalindromeQueries<26, 500002, 500000>;
    auto tree = std::make_unique<Tree>();
    while(query_count--){
        int type;
        std::cin >> type;
        DequePalindromeStatistics result;
        if(type == 0){
            char c;
            std::cin >> c;
            result = tree->push_front(c);
        }else if(type == 1){
            char c;
            std::cin >> c;
            result = tree->push_back(c);
        }else if(type == 2){
            result = tree->pop_front();
        }else{
            result = tree->pop_back();
        }

        std::cout << result.distinct_palindromes << ' '
                  << result.longest_prefix << ' '
                  << result.longest_suffix << '\n';
    }
}
