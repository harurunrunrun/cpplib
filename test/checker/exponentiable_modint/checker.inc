#include <cassert>
#include <cstdint>

#include "../../../src/structure/modint/exponentiable_modint.hpp"

template<std::uint32_t MOD>
std::uint32_t pow_mod_small(std::uint64_t a, std::uint64_t n){
    if constexpr(MOD == 1){
        return 0;
    }else{
        std::uint64_t res = 1 % MOD;
        a %= MOD;
        while(n > 0){
            if(n & 1){
                res = res * a % MOD;
            }
            a = a * a % MOD;
            n >>= 1;
        }
        return static_cast<std::uint32_t>(res);
    }
}

template<std::uint32_t MOD>
void test_small_power(){
    using mint = ExponentiableModint<MOD>;
    for(std::uint64_t a = 0; a <= 20; a++){
        for(std::uint64_t b = 0; b <= 20; b++){
            assert(mint(a).pow(mint(b)).val() == pow_mod_small<MOD>(a, b));
        }
    }
}

void test_non_coprime(){
    using mint = ExponentiableModint<10>;
    assert(mint(2).pow(mint(0)).val() == 1);
    assert(mint(2).pow(mint(4)).val() == 6);
    assert(mint(2).pow(mint(5)).val() == 2);
    assert(mint(2).pow(mint(6)).val() == 4);
    assert(mint(0).pow(mint(0)).val() == 1);
    assert(mint(0).pow(mint(6)).val() == 0);
}

void test_nested_power(){
    using mint = ExponentiableModint<1000>;
    assert(mint(2).pow(mint(3).pow(mint(4))).val() == pow_mod_small<1000>(2, 81));
    assert(mint(3).pow(mint(2).pow(mint(5))).val() == pow_mod_small<1000>(3, 32));
}

void test_add_mul(){
    using mint = ExponentiableModint<10>;
    assert((mint(7) + mint(8)).val() == 5);
    assert((mint(7) * mint(8)).val() == 6);
    assert((mint(7) + mint(8)).pow(mint(2)).val() == 5);
    assert((mint(2) * mint(5)).pow(mint(2)).val() == 0);
}

void test_abc228_e_shape(){
    using mint = ExponentiableModint<998244353>;
    assert(mint(2).pow(mint(3).pow(mint(2))).val() == 512);
    assert(mint(0).pow(mint(0).pow(mint(1))).val() == 1);
    assert(mint(998244353).pow(mint(2).pow(mint(3))).val() == 0);
}

inline void run_exponentiable_modint_standalone_check(){
    static_assert(exponentiable_modint_totient(1) == 1);
    static_assert(exponentiable_modint_totient(10) == 4);
    static_assert(exponentiable_modint_totient(998244353) == 998244352);

    test_small_power<1>();
    test_small_power<2>();
    test_small_power<3>();
    test_small_power<4>();
    test_small_power<5>();
    test_small_power<8>();
    test_small_power<10>();
    test_small_power<12>();
    test_small_power<17>();
    test_non_coprime();
    test_nested_power();
    test_add_mul();
    test_abc228_e_shape();
}
