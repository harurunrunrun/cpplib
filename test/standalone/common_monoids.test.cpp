// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> add_monoid{};
constexpr MulMonoid<long long> mul_monoid{};
constexpr MinMonoid<int> min_monoid{};
constexpr MaxMonoid<int> max_monoid{};
constexpr GcdMonoid<int> gcd_monoid{};
constexpr LcmMonoid<int> lcm_monoid{};
constexpr XorMonoid<int> xor_monoid{};
constexpr BitAndMonoid<int> bit_and_monoid{};
constexpr BitOrMonoid<int> bit_or_monoid{};

void test_direct(){
    static_assert(add_monoid.op(2, 3) == 5);
    static_assert(add_monoid.e() == 0);
    static_assert(mul_monoid.op(2, 3) == 6);
    static_assert(mul_monoid.e() == 1);
    static_assert(min_monoid.op(5, -1) == -1);
    static_assert(max_monoid.op(5, -1) == 5);
    static_assert(gcd_monoid.op(18, 24) == 6);
    static_assert(gcd_monoid.e() == 0);
    static_assert(lcm_monoid.op(6, 10) == 30);
    static_assert(lcm_monoid.e() == 1);
    static_assert(xor_monoid.op(6, 3) == 5);
    static_assert(bit_and_monoid.op(6, 3) == 2);
    static_assert(bit_or_monoid.op(6, 3) == 7);
}

void test_segtree_add(){
    Segtree<add_monoid, 8> seg(std::vector<long long>{1, 2, 3, 4, 5});
    assert(seg.prod(0, 5) == 15);
    assert(seg.prod(1, 4) == 9);
    seg.set(2, 10);
    assert(seg.prod(0, 5) == 22);
}

void test_segtree_min_max(){
    Segtree<min_monoid, 8> mn(std::vector<int>{5, 2, 7, -1, 4});
    Segtree<max_monoid, 8> mx(std::vector<int>{5, 2, 7, -1, 4});
    assert(mn.prod(0, 5) == -1);
    assert(mn.prod(0, 3) == 2);
    assert(mx.prod(0, 5) == 7);
    assert(mx.prod(3, 5) == 4);
}

void test_segtree_other(){
    Segtree<gcd_monoid, 8> gcd_seg(std::vector<int>{12, 18, 30, 42});
    Segtree<lcm_monoid, 8> lcm_seg(std::vector<int>{2, 3, 4, 5});
    Segtree<xor_monoid, 8> xor_seg(std::vector<int>{1, 2, 3, 4});
    Segtree<bit_and_monoid, 8> and_seg(std::vector<int>{7, 6, 3});
    Segtree<bit_or_monoid, 8> or_seg(std::vector<int>{1, 2, 4});

    assert(gcd_seg.prod(0, 4) == 6);
    assert(gcd_seg.prod(1, 3) == 6);
    assert(lcm_seg.prod(0, 4) == 60);
    assert(xor_seg.prod(0, 4) == 4);
    assert(and_seg.prod(0, 3) == 2);
    assert(or_seg.prod(0, 3) == 7);
}

int main(){
    int q;
    if(std::cin >> q){
        while(q--){
            std::string type;
            long long a, b;
            std::cin >> type;
            if(type.size() > 1 && type[0] == 'E'){
                if(type == "EADD") std::cout << add_monoid.e() << '\n';
                if(type == "EMUL") std::cout << mul_monoid.e() << '\n';
                if(type == "EMIN") std::cout << min_monoid.e() << '\n';
                if(type == "EMAX") std::cout << max_monoid.e() << '\n';
                if(type == "EGCD") std::cout << gcd_monoid.e() << '\n';
                if(type == "ELCM") std::cout << lcm_monoid.e() << '\n';
                if(type == "EXOR") std::cout << xor_monoid.e() << '\n';
                if(type == "EAND") std::cout << bit_and_monoid.e() << '\n';
                if(type == "EOR") std::cout << bit_or_monoid.e() << '\n';
                continue;
            }
            std::cin >> a >> b;
            if(type == "ADD") std::cout << add_monoid.op(a, b) << '\n';
            if(type == "MUL") std::cout << mul_monoid.op(a, b) << '\n';
            if(type == "MIN") std::cout << min_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "MAX") std::cout << max_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "GCD") std::cout << gcd_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "LCM") std::cout << lcm_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "XOR") std::cout << xor_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "AND") std::cout << bit_and_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "OR") std::cout << bit_or_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
        }
        return 0;
    }

    test_direct();
    test_segtree_add();
    test_segtree_min_max();
    test_segtree_other();
}
