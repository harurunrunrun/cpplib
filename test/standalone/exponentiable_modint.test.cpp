// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

#include "../../src/structure/modint/exponentiable_modint.hpp"

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

void test_public_value_operations(){
    using mint = ExponentiableModint<10>;
    static_assert(mint::get_mod() == 10);
    static_assert(ExponentiableModint<1>::get_mod() == 1);
    assert(mint().val() == 0);
    typename mint::next_type next(7);
    mint explicit_layers(7, next);
    assert(explicit_layers.val() == 7);

    mint value = 7;
    value += mint(8);
    assert(value.val() == 5);
    value *= mint(3);
    assert(value.val() == 5);
    assert(value == mint(5));
    assert(value != mint(6));

    ExponentiableModint<1> terminal = 3;
    terminal += ExponentiableModint<1>(0);
    terminal *= ExponentiableModint<1>(2);
    assert(terminal.val() == 0);
    assert(terminal == ExponentiableModint<1>(0));
    assert(!(terminal != ExponentiableModint<1>(99)));
}

int main(){
    int q;
    if(std::cin >> q){
        using mint10 = ExponentiableModint<10>;
        using mint12 = ExponentiableModint<12>;
        using mint998 = ExponentiableModint<998244353>;
        while(q--){
            std::string type;
            std::uint64_t a, b, c;
            std::cin >> type >> a >> b;
            if(type == "POW10") std::cout << mint10(a).pow(mint10(b)).val() << '\n';
            if(type == "POW12") std::cout << mint12(a).pow(mint12(b)).val() << '\n';
            if(type == "POW998") std::cout << mint998(a).pow(mint998(b)).val() << '\n';
            if(type == "ADD10") std::cout << (mint10(a) + mint10(b)).val() << '\n';
            if(type == "MUL10") std::cout << (mint10(a) * mint10(b)).val() << '\n';
            if(type == "NEST10"){
                std::cin >> c;
                std::cout << mint10(a).pow(mint10(b).pow(mint10(c))).val() << '\n';
            }
            if(type == "NEST998"){
                std::cin >> c;
                std::cout << mint998(a).pow(mint998(b).pow(mint998(c))).val() << '\n';
            }
        }
        return 0;
    }

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
    test_public_value_operations();
}
