// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/eertree

#include <iostream>
#include <string>

#include "../../src/algorithm/string/palindromic_tree_representation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    std::cin >> text;

    const auto result = palindromic_tree_representation<1000000>(text);
    std::cout << result.nodes.size() << '\n';
    for(const auto& node: result.nodes){
        std::cout << node.parent << ' ' << node.suffix_link << '\n';
    }
    for(std::size_t index = 0; index < result.longest_suffix.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << result.longest_suffix[index];
    }
    std::cout << '\n';
}
