// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc308/tasks/abc308_g

#include <iostream>

#include "../../src/structure/trie/minimum_xor_pair_multiset.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int q;
    std::cin >> q;
    MinimumXorPairMultiset<unsigned> values;
    while(q--){
        int type;
        std::cin >> type;
        if(type == 3){
            std::cout << values.minimum_pair_xor() << '\n';
            continue;
        }
        unsigned value;
        std::cin >> value;
        if(type == 1){
            values.insert(value);
        }else{
            values.erase(value);
        }
    }
}
