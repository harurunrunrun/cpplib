// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include <vector>
#include "../../src/algorithm/string/palindromic_tree_representation.hpp"

void self_test(){
    const auto empty = palindromic_tree_representation<8>("");
    assert(empty.nodes.empty());
    assert(empty.longest_suffix.empty());

    const auto result = palindromic_tree_representation<8>("aba");
    assert(result.nodes.size() == 3);
    assert((result.nodes[0] == PalindromicTreeRelation{-1, 0}));
    assert((result.nodes[1] == PalindromicTreeRelation{-1, 0}));
    assert((result.nodes[2] == PalindromicTreeRelation{2, 1}));
    assert((result.longest_suffix == std::vector<int>{1, 2, 3}));
}

int main(){
    self_test();

    std::string text;
    if(!(std::cin >> text)) return 0;
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
