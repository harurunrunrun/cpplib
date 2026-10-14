// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/set_xor_min

#include <iostream>
#include <memory>
#include "../../src/structure/trie/binary/binary_trie.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int q; std::cin >> q;
    auto trie = std::make_unique<BinaryTrie<unsigned, 30, 15'000'001>>();
    while(q--){
        int type; unsigned x; std::cin >> type >> x;
        if(type == 0){ if(!trie->contains(x)) trie->insert(x); }
        else if(type == 1) trie->erase(x);
        else std::cout << (*trie->min_element(x) ^ x) << '\n';
    }
}
