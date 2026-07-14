// competitive-verifier: PROBLEM https://www.spoj.com/problems/SUBLEX/

#include <iostream>
#include <string>

#include "../../src/structure/string/lexicographical_distinct_substrings.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;
    LexicographicalDistinctSubstrings substrings(std::move(text));

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        long long rank;
        std::cin >> rank;
        std::cout << substrings.kth(rank).value() << '\n';
    }
}
