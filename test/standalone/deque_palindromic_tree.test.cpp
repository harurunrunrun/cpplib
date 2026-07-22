// competitive-verifier: STANDALONE

#include <iostream>
#include <memory>
#include "../../src/algorithm/string/palindrome/deque_palindromic_tree.hpp"
#include "../../src/algorithm/string/palindrome/deque_palindromic_tree.hpp"

int main(){
    int query_count;
    if(!(std::cin >> query_count)) return 0;

    using Tree = DequePalindromicTree<26, 500002, 500000>;
    auto tree = std::make_unique<Tree>();
    for(int query_index = 0; query_index < query_count; ++query_index){
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

        const auto result = tree->query();
        std::cout << result.distinct_palindromes << ' '
                  << result.longest_prefix << ' '
                  << result.longest_suffix << '\n';
    }
}
