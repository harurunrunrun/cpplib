// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../../src/structure/modint/modint.hpp"

using mint = Modint<998244353>;

void test_basic_arithmetic(){
    assert(mint::get_mod() == 998244353);
    assert(mint(-1).val() == 998244352);
    assert(mint(998244353LL * 3 + 7).val() == 7);
    assert(mint::raw(123).val() == 123);

    mint a = 10;
    mint b = 20;
    assert((a + b).val() == 30);
    assert((a - b).val() == 998244343);
    assert((b - a).val() == 10);
    assert((a * b).val() == 200);
    assert((b / a).val() == 2);
    assert((-a).val() == 998244343);
    assert(+a == a);
    assert(a != b);
}

void test_pow_inv(){
    assert(mint(2).pow(0).val() == 1);
    assert(mint(2).pow(10).val() == 1024);
    assert((mint(1234567) * mint(1234567).inv()).val() == 1);

    using mod12 = Modint<12>;
    assert(mod12(5).inv().val() == 5);
    assert((mod12(7) * mod12(7).inv()).val() == 1);

    bool thrown = false;
    try{
        (void)mint(2).pow(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)mod12(6).inv();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_increment_decrement(){
    using mod7 = Modint<7>;
    mod7 x = 6;
    assert((++x).val() == 0);
    assert((x++).val() == 0);
    assert(x.val() == 1);
    assert((--x).val() == 0);
    assert((x--).val() == 0);
    assert(x.val() == 6);
}

void test_stream(){
    std::stringstream ss;
    mint x;
    ss << "-3";
    ss >> x;
    assert(x.val() == 998244350);

    std::stringstream out;
    out << x;
    assert(out.str() == "998244350");
}

int main(){
    int q;
    if(std::cin >> q){
        while(q--){
            std::string type;
            long long a, b;
            std::cin >> type >> a;
            if(type == "POW"){
                std::cin >> b;
                std::cout << mint(a).pow(b) << '\n';
            }else if(type == "INV"){
                std::cout << mint(a).inv() << '\n';
            }else if(type == "INV12"){
                std::cout << Modint<12>(a).inv() << '\n';
            }else if(type == "INC"){
                mint x(a);
                std::cout << ++x << '\n';
            }else if(type == "DEC"){
                mint x(a);
                std::cout << --x << '\n';
            }else{
                std::cin >> b;
                if(type == "ADD") std::cout << mint(a) + mint(b) << '\n';
                if(type == "SUB") std::cout << mint(a) - mint(b) << '\n';
                if(type == "MUL") std::cout << mint(a) * mint(b) << '\n';
                if(type == "DIV") std::cout << mint(a) / mint(b) << '\n';
            }
        }
        return 0;
    }

    test_basic_arithmetic();
    test_pow_inv();
    test_increment_decrement();
    test_stream();
}
