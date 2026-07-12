#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../../src/structure/other/disjoint_sparse_table.hpp"
#include "../../../src/structure/types/common_monoids.hpp"

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
}

inline void run_disjoint_sparse_table_standalone_check(){
    test_add();
    test_non_commutative();
    test_xor();
    test_empty();
    test_exceptions();
}
