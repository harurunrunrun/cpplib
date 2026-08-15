// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/division_of_big_integers

#include <iostream>
#include <string>

#include "../../src/algorithm/math/integer/fast_big_integer.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::string dividend_text, divisor_text;
        std::cin >> dividend_text >> divisor_text;
        const BigInteger dividend(dividend_text);
        const BigInteger divisor(divisor_text);
        const auto [quotient, remainder] = BigInteger::divmod(
            dividend, divisor
        );
        std::cout << quotient << ' ' << remainder << '\n';
    }
}
