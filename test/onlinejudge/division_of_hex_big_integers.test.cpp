// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/division_of_hex_big_integers
#include <iostream>
#include <string>
#include "../../src/algorithm/math/integer/fast_big_integer.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int t; std::cin >> t;
    while(t--){
        std::string left,right; std::cin >> left >> right;
        const auto [quotient,remainder]=HexBigInteger::divmod(HexBigInteger(left),HexBigInteger(right));
        std::cout << quotient << ' ' << remainder << '\n';
    }
}
