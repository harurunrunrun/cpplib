// competitive-verifier: PROBLEM https://www.spoj.com/problems/PHONELST/

#include <iostream>
#include <string>

#include "../../src/structure/trie/trie.hpp"

namespace{

Trie<10, 100001, '0'> phone_numbers;

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int phone_count;
        std::cin >> phone_count;
        phone_numbers.clear();
        while(phone_count-- > 0){
            std::string phone_number;
            std::cin >> phone_number;
            phone_numbers.insert(phone_number);
        }
        std::cout << (phone_numbers.is_prefix_free() ? "YES" : "NO") << '\n';
    }
}
