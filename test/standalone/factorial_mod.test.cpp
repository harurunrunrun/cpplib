// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/factorial_mod.hpp"

void test_small(){
    math::FactorialModPrime fac3(3);
    assert(fac3.mod() == 3);
    {
        auto res = fac3.factorial(10);
        assert(res.value == 1);
        assert(res.exponent == 4);
    }
    {
        auto res = fac3.factorial(0);
        assert(res.value == 1);
        assert(res.exponent == 0);
    }

    math::FactorialModPrime fac5(5);
    assert(fac5.mod() == 5);
    {
        auto res = fac5.factorial(10);
        assert(res.value == 2);
        assert(res.exponent == 2);
    }
    {
        auto res = fac5.factorial(24);
        assert(res.value == 1);
        assert(res.exponent == 4);
    }
}

void test_naive(){
    for(int p: {2, 3, 5, 7, 11}){
        math::FactorialModPrime fac(p);
        assert(fac.mod() == p);
        for(int n = 0; n <= 80; n++){
            long long value = 1;
            long long exponent = 0;
            for(int x = 1; x <= n; x++){
                int y = x;
                while(y % p == 0){
                    y /= p;
                    exponent++;
                }
                value = value * (y % p) % p;
            }
            auto res = fac.factorial(n);
            assert(res.value == value);
            assert(res.exponent == exponent);
        }
    }
}

void test_exceptions(){
    bool thrown = false;
    try{
        math::FactorialModPrime fac(1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    math::FactorialModPrime fac(7);
    thrown = false;
    try{
        (void)fac.factorial(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int q;
    if(std::cin >> q){
        while(q--){
            int prime;
            long long n;
            std::cin >> prime >> n;
            const math::FactorialModPrime factorial(prime);
            const auto result = factorial.factorial(n);
            std::cout << result.value << ' ' << result.exponent << '\n';
        }
        return 0;
    }

    test_small();
    test_naive();
    test_exceptions();
}
