// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include "../../src/structure/trie/trie.hpp"

void test_api(){
    Trie<10, 64, '0'> trie;
    assert(trie.is_prefix_free());

    trie.insert("911");
    trie.insert("91125426");
    assert(!trie.is_prefix_free());
    assert(trie.erase("91125426"));
    assert(trie.is_prefix_free());

    trie.clear();
    trie.insert("91125426");
    trie.insert("911");
    assert(!trie.is_prefix_free());

    trie.clear();
    trie.insert("123");
    trie.insert("123");
    assert(!trie.is_prefix_free());
    assert(trie.erase("123"));
    assert(trie.is_prefix_free());

    trie.clear();
    trie.insert("");
    assert(trie.is_prefix_free());
    trie.insert("0");
    assert(!trie.is_prefix_free());
}

int main(){
    test_api();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    Trie<10, 2001, '0'> trie;
    while(test_count-- > 0){
        int phone_count;
        std::cin >> phone_count;
        trie.clear();
        while(phone_count-- > 0){
            std::string phone_number;
            std::cin >> phone_number;
            trie.insert(phone_number);
        }
        std::cout << (trie.is_prefix_free() ? "YES" : "NO") << '\n';
    }
}
