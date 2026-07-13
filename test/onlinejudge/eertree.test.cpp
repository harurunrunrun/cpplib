#define PROBLEM "https://judge.yosupo.jp/problem/eertree"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../../src/algorithm/string/palindromic_tree.hpp"

namespace{

int official_index(int node){
    return node == 0 ? -1 : node - 1;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    std::cin >> text;

    auto tree = std::make_unique<PalindromicTree<26, 500002>>();
    std::vector<int> longest_suffix;
    longest_suffix.reserve(text.size());
    for(char character: text){
        longest_suffix.push_back(official_index(tree->add(character)));
    }

    std::cout << tree->distinct_palindromes() << '\n';
    for(int node = 2; node < tree->node_count(); ++node){
        std::cout << official_index(tree->parent(node)) << ' '
                  << official_index(tree->link(node)) << '\n';
    }
    for(std::size_t index = 0; index < longest_suffix.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << longest_suffix[index];
    }
    std::cout << '\n';
}
