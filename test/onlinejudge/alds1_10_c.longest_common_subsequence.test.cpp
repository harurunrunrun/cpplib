#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_10_C"

#include <iostream>
#include <string>

#include "../../src/algorithm/string/longest_common_subsequence.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        std::string a, b;
        std::cin >> a >> b;
        std::cout << longest_common_subsequence_length(a, b) << '\n';
    }
}
