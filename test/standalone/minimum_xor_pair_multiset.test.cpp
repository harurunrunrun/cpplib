// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/structure/trie/minimum_xor_pair_multiset.hpp"

void self_test(){
    MinimumXorPairMultiset<unsigned> values;
    assert(values.empty());
    values.insert(7);
    values.insert(7);
    values.insert(2);
    assert(values.size() == 3);
    assert(values.count(7) == 2);
    assert(values.minimum_pair_xor() == 0);
    assert(values.erase(7));
    assert(values.minimum_pair_xor() == 5);
    assert(!values.erase(100));

    values.erase(7);
    bool thrown = false;
    try{
        (void)values.minimum_pair_xor();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }
    MinimumXorPairMultiset<unsigned> values;
    while(q-- != 0){
        int type;
        std::cin >> type;
        if(type == 3){
            std::cout << values.minimum_pair_xor() << '\n';
        }else{
            unsigned value;
            std::cin >> value;
            if(type == 1) values.insert(value);
            else values.erase(value);
        }
    }
}
