// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/combinatorics/combinatorics.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint<998244353>;

void test_basic(){
    Combinatorics<mint, 20> comb;
    assert(comb.factorial(0).val() == 1);
    assert(comb.factorial(5).val() == 120);
    assert((comb.factorial(5) * comb.inverse_factorial(5)).val() == 1);
    assert((mint(7) * comb.inverse(7)).val() == 1);
}

void test_comb_perm(){
    Combinatorics<mint, 20> comb;
    assert(comb.permutation(5, 3).val() == 60);
    assert(comb.permutation(5, 6).val() == 0);
    assert(comb.combination(5, 2).val() == 10);
    assert(comb.combination(5, 0).val() == 1);
    assert(comb.combination(5, 6).val() == 0);
    assert(comb.combination(5, -1).val() == 0);
    assert(comb.homogeneous(3, 4).val() == 15);
    assert(comb.homogeneous(0, 0).val() == 1);
    assert(comb.homogeneous(0, 3).val() == 0);
}

void test_bell(){
    Combinatorics<mint, 20> comb;
    assert(comb.bell_number(0).val() == 1);
    assert(comb.bell_number(1).val() == 1);
    assert(comb.bell_number(2).val() == 2);
    assert(comb.bell_number(3).val() == 5);
    assert(comb.bell_number(4).val() == 15);
    assert(comb.bell_number(5).val() == 52);
    assert(comb.bell_number(6).val() == 203);
}

void test_exceptions(){
    Combinatorics<mint, 8> comb;
    bool thrown = false;
    try{
        (void)comb.factorial(9);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)comb.inverse(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)comb.homogeneous(5, 5);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int q;
    if(std::cin >> q){
        Combinatorics<mint, 1000> comb;
        while(q--){
            std::string type;
            int n, k;
            std::cin >> type >> n;
            if(type == "FACT") std::cout << comb.factorial(n) << '\n';
            if(type == "IFACT") std::cout << comb.inverse_factorial(n) << '\n';
            if(type == "INV") std::cout << comb.inverse(n) << '\n';
            if(type == "BELL") std::cout << comb.bell_number(n) << '\n';
            if(type == "PERM" || type == "COMB" || type == "HOM"){
                std::cin >> k;
                if(type == "PERM") std::cout << comb.permutation(n, k) << '\n';
                if(type == "COMB") std::cout << comb.combination(n, k) << '\n';
                if(type == "HOM") std::cout << comb.homogeneous(n, k) << '\n';
            }
        }
        return 0;
    }

    test_basic();
    test_comb_perm();
    test_bell();
    test_exceptions();
}
