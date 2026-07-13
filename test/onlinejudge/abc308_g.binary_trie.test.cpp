// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc308/tasks/abc308_g

#include <iostream>
#include <set>

#include "../../src/structure/trie/binary_trie.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int q;
    std::cin >> q;
    static BinaryTrie<unsigned, 30, 9000005> trie;
    std::multiset<unsigned> adjacent_xor;
    while(q--){
        int type;
        std::cin >> type;
        if(type == 3){
            std::cout << *adjacent_xor.begin() << '\n';
            continue;
        }
        unsigned value;
        std::cin >> value;
        const int lower = trie.count_less(value);
        if(type == 1){
            const auto left = lower == 0 ? std::nullopt : trie.kth(lower - 1);
            const auto right = lower == trie.size() ? std::nullopt : trie.kth(lower);
            if(left && right) adjacent_xor.erase(adjacent_xor.find(*left ^ *right));
            if(left) adjacent_xor.insert(*left ^ value);
            if(right) adjacent_xor.insert(value ^ *right);
            trie.insert(value);
        }else{
            const auto left = lower == 0 ? std::nullopt : trie.kth(lower - 1);
            const auto right = lower + 1 == trie.size() ? std::nullopt : trie.kth(lower + 1);
            if(left) adjacent_xor.erase(adjacent_xor.find(*left ^ value));
            if(right) adjacent_xor.erase(adjacent_xor.find(value ^ *right));
            if(left && right) adjacent_xor.insert(*left ^ *right);
            trie.erase(value);
        }
    }
}
