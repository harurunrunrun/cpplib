// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/addition_of_big_integers

#include <iostream>
#include <string>

#include "../../src/algorithm/math/integer/fast_big_integer.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::string left, right;
        std::cin >> left >> right;
        std::cout << BigInteger(left) + BigInteger(right) << '\n';
    }
}
