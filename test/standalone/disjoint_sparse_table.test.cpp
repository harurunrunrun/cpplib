// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/other/disjoint_sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> add_monoid{};
constexpr XorMonoid<int> xor_monoid{};

struct StringMonoid{
    using S = std::string;

    S op(const S& a, const S& b)const{
        return a + b;
    }

    S e()const{
        return "";
    }
};

constexpr StringMonoid string_monoid{};

void test_add(){
    std::vector<long long> a{1, 2, 3, 4, 5, 6};
    DisjointSparseTable<add_monoid, 8> dst(a);
    assert(dst.size() == 6);
    assert(!dst.empty());
    assert(dst.get(2) == 3);
    assert(dst.prod(0, 6) == 21);
    assert(dst.prod(1, 5) == 14);
    assert(dst.prod(4, 6) == 11);
    assert(dst.prod(3, 3) == 0);
    assert(dst.all_prod() == 21);
}

void test_non_commutative(){
    std::vector<std::string> a{"ab", "c", "de", "f", "ghi"};
    DisjointSparseTable<string_monoid, 8> dst(a);
    assert(dst.prod(0, 5) == "abcdefghi");
    assert(dst.prod(1, 4) == "cdef");
    assert(dst.prod(3, 5) == "fghi");
}

void test_xor(){
    std::vector<int> a{5, 2, 7, 1, 4, 3, 8};
    DisjointSparseTable<xor_monoid, 8> dst(a);
    for(int l = 0; l < (int)a.size(); l++){
        for(int r = l; r <= (int)a.size(); r++){
            int expected = 0;
            for(int i = l; i < r; i++){
                expected ^= a[i];
            }
            assert(dst.prod(l, r) == expected);
        }
    }
}

void test_empty(){
    DisjointSparseTable<add_monoid, 4> dst(0);
    assert(dst.size() == 0);
    assert(dst.empty());
    assert(dst.all_prod() == 0);
}

void test_exceptions(){
    bool thrown = false;
    try{
        DisjointSparseTable<add_monoid, 3> dst(4);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        DisjointSparseTable<add_monoid, 3> dst(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        DisjointSparseTable<add_monoid, 3> dst(
            std::vector<long long>{1, 2, 3, 4});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    DisjointSparseTable<add_monoid, 4> dst(std::vector<long long>{1, 2, 3});
    thrown = false;
    try{
        (void)dst.prod(2, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)dst.get(3);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)dst.get(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)dst.prod(0, 4);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n, q;
    if(std::cin >> n >> q){
        std::vector<long long> values(static_cast<std::size_t>(n));
        for(auto& x: values) std::cin >> x;
        std::vector<int> xor_values(values.begin(), values.end());
        std::vector<std::string> words(static_cast<std::size_t>(n));
        for(auto& word: words) std::cin >> word;
        DisjointSparseTable<add_monoid, 256> sum(values);
        DisjointSparseTable<xor_monoid, 256> bit_xor(xor_values);
        DisjointSparseTable<string_monoid, 256> concat(words);
        while(q--){
            std::string type;
            int l, r;
            std::cin >> type >> l >> r;
            if(type == "SUM") std::cout << sum.prod(l, r) << '\n';
            if(type == "XOR") std::cout << bit_xor.prod(l, r) << '\n';
            if(type == "CONCAT") std::cout << concat.prod(l, r) << '\n';
        }
        return 0;
    }

    test_add();
    test_non_commutative();
    test_xor();
    test_empty();
    test_exceptions();
}
