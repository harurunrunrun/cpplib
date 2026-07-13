// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include "../../src/structure/segtree/dynamic_segtree_2d.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> dynamic_add_monoid_2d{};

void test_basic(){
    DynamicSegtree2D<dynamic_add_monoid_2d, 1000000000LL, 1000000000LL> seg;
    assert(seg.row_node_count() == 1);
    assert(seg.column_node_count() == 0);
    assert(seg.all_prod() == 0);

    seg.set(123456789, 987654321, 5);
    seg.set(123456790, 987654321, 7);
    seg.set(900000000, 100, 11);
    assert(seg.get(123456789, 987654321) == 5);
    assert(seg.get(0, 0) == 0);
    assert(seg.prod(123456000, 987654000, 123457000, 987655000) == 12);
    assert(seg.prod(0, 0, 1000000000LL, 1000000000LL) == 23);
    assert(seg.all_prod() == 23);

    seg.apply(123456789, 987654321, -2);
    assert(seg.get(123456789, 987654321) == 3);
    assert(seg.all_prod() == 21);
    assert(seg.row_node_count() < 100);
    assert(seg.column_node_count() < 4000);

    seg.clear();
    assert(seg.row_node_count() == 1);
    assert(seg.column_node_count() == 0);
    assert(seg.all_prod() == 0);
}

int main(){
    int query_count;
    if(std::cin >> query_count){
        DynamicSegtree2D<dynamic_add_monoid_2d, 1024, 1024> seg;
        while(query_count--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                long long row, col, value;
                std::cin >> row >> col >> value;
                seg.set(row, col, value);
            }else if(type == "ADD"){
                long long row, col, value;
                std::cin >> row >> col >> value;
                seg.apply(row, col, value);
            }else if(type == "GET"){
                long long row, col;
                std::cin >> row >> col;
                std::cout << seg.get(row, col) << '\n';
            }else if(type == "PROD"){
                long long row_begin, col_begin, row_end, col_end;
                std::cin >> row_begin >> col_begin >> row_end >> col_end;
                std::cout << seg.prod(row_begin, col_begin, row_end, col_end) << '\n';
            }else if(type == "ALL"){
                std::cout << seg.all_prod() << '\n';
            }else if(type == "CLEAR"){
                seg.clear();
            }
        }
        return 0;
    }
    test_basic();
}
