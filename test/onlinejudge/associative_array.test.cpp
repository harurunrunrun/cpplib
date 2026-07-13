// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/associative_array

#include <cstdint>
#include <iostream>

#include "../../src/structure/hash/associative_array.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::cin >> query_count;
    AssociativeArray<std::uint64_t, std::uint64_t, 1000000> table;
    while(query_count-- > 0){
        int type;
        std::uint64_t key;
        std::cin >> type >> key;
        if(type == 0){
            std::uint64_t value;
            std::cin >> value;
            table.insert_or_assign(key, value);
        }else{
            const std::uint64_t* value = table.find(key);
            std::cout << (value == nullptr ? 0 : *value) << '\n';
        }
    }
}
