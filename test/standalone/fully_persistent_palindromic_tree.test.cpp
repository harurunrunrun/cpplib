// competitive-verifier: STANDALONE

#include <algorithm>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/palindrome/fully_persistent_palindromic_tree.hpp"

int main(){
    int q;
    if(!(std::cin >> q)) return 0;
    FullyPersistentPalindromicTree<3, 2000, 1000> tree;
    while(q--){
        char operation;
        int version;
        std::cin >> operation >> version;
        if(operation == 'A'){
            char c;
            std::cin >> c;
            std::cout << tree.append(version, c) << '\n';
        }else if(operation == 'D'){
            std::cout << tree.distinct_palindromes(version) << '\n';
        }else if(operation == 'C'){
            std::cout << tree.count_palindromic_substrings(version) << '\n';
        }else if(operation == 'L'){
            std::cout << tree.longest_suffix_palindrome_length(version) << '\n';
        }else if(operation == 'M'){
            std::cout << tree.longest_palindrome_length(version) << '\n';
        }else if(operation == 'S'){
            std::cout << tree.str(version) << '\n';
        }else if(operation == 'H'){
            std::string value;
            std::cin >> value;
            std::cout << tree.contains(value, version) << '\n';
        }else if(operation == 'T'){
            const auto lengths = tree.suffix_palindrome_lengths(version);
            std::cout << lengths.size();
            for(int length: lengths) std::cout << ' ' << length;
            std::cout << '\n';
        }else{
            const auto nodes = tree.palindrome_nodes(version);
            std::vector<int> lengths;
            lengths.reserve(nodes.size());
            for(int node: nodes) lengths.push_back(tree.length(node));
            std::sort(lengths.begin(), lengths.end());
            std::cout << lengths.size();
            for(int length: lengths) std::cout << ' ' << length;
            std::cout << '\n';
        }
    }
}
