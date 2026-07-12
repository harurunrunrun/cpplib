// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/other/sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr MinMonoid<int> min_monoid{};
constexpr MaxMonoid<int> max_monoid{};
constexpr GcdMonoid<int> gcd_monoid{};

void test_min(){
    std::vector<int> a{5, 2, 7, -1, 4, 3};
    SparseTable<min_monoid, 8> st(a);
    assert(st.size() == 6);
    assert(!st.empty());
    assert(st.get(2) == 7);
    assert(st.prod(0, 6) == -1);
    assert(st.prod(0, 3) == 2);
    assert(st.prod(4, 6) == 3);
    assert(st.prod(3, 3) == min_monoid.e());
    assert(st.all_prod() == -1);
}

void test_max_gcd(){
    SparseTable<max_monoid, 8> mx(std::vector<int>{5, 2, 7, -1, 4, 3});
    assert(mx.prod(0, 6) == 7);
    assert(mx.prod(3, 5) == 4);

    SparseTable<gcd_monoid, 8> gcd(std::vector<int>{18, 24, 30, 42, 54});
    assert(gcd.prod(0, 5) == 6);
    assert(gcd.prod(1, 4) == 6);
    assert(gcd.prod(3, 5) == 6);
}

void test_empty(){
    SparseTable<min_monoid, 4> st(0);
    assert(st.size() == 0);
    assert(st.empty());
    assert(st.all_prod() == min_monoid.e());
}

void test_exceptions(){
    bool thrown = false;
    try{
        SparseTable<min_monoid, 3> st(4);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    SparseTable<min_monoid, 4> st(std::vector<int>{1, 2, 3});
    thrown = false;
    try{
        (void)st.prod(2, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)st.get(3);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_against_naive(){
    std::vector<int> a{9, 1, 4, 1, 5, 9, 2, 6};
    SparseTable<min_monoid, 8> st(a);
    for(int l = 0; l < (int)a.size(); l++){
        for(int r = l + 1; r <= (int)a.size(); r++){
            int expected = a[l];
            for(int i = l + 1; i < r; i++){
                expected = std::min(expected, a[i]);
            }
            assert(st.prod(l, r) == expected);
        }
    }
}

int main(){
    int n, q;
    if(std::cin >> n >> q){
        std::vector<int> a(static_cast<std::size_t>(n));
        for(int& x: a) std::cin >> x;
        SparseTable<min_monoid, 256> mn(a);
        SparseTable<max_monoid, 256> mx(a);
        SparseTable<gcd_monoid, 256> gcd(a);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "GET"){
                int k;
                std::cin >> k;
                std::cout << mn.get(k) << '\n';
            }else{
                int l, r;
                std::cin >> l >> r;
                if(type == "MIN") std::cout << mn.prod(l, r) << '\n';
                if(type == "MAX") std::cout << mx.prod(l, r) << '\n';
                if(type == "GCD") std::cout << gcd.prod(l, r) << '\n';
            }
        }
        return 0;
    }

    test_min();
    test_max_gcd();
    test_empty();
    test_exceptions();
    test_against_naive();
}
