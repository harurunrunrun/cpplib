// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/segtree/segtree_2d.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> add_monoid_2d{};

void test_basic(){
    Segtree2D<add_monoid_2d, 4, 5> seg(
        std::vector<std::vector<long long>>{{1, 2, 3}, {4, 5, 6}}
    );
    assert(seg.height() == 2);
    assert(seg.width() == 3);
    assert(!seg.empty());
    assert(seg.get(1, 2) == 6);
    assert(seg.prod(0, 0, 2, 3) == 21);
    assert(seg.prod(0, 1, 2, 3) == 16);
    assert(seg.prod(1, 0, 2, 2) == 9);
    assert(seg.all_prod() == 21);

    seg.set(0, 1, 10);
    assert(seg.prod(0, 0, 1, 3) == 14);
    seg.apply(1, 2, -2);
    assert(seg.get(1, 2) == 4);
    assert(seg.all_prod() == 27);
}

void test_exceptions(){
    bool thrown = false;
    try{
        Segtree2D<add_monoid_2d, 2, 2> seg(3, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        Segtree2D<add_monoid_2d, 2, 2> seg(
            std::vector<std::vector<long long>>{{1, 2}, {3}}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int height, width, query_count;
    if(std::cin >> height >> width >> query_count){
        std::vector<std::vector<long long>> values(
            static_cast<std::size_t>(height),
            std::vector<long long>(static_cast<std::size_t>(width))
        );
        for(auto& row: values){
            for(auto& value: row) std::cin >> value;
        }
        Segtree2D<add_monoid_2d, 32, 32> seg(values);
        while(query_count--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int row, col;
                long long value;
                std::cin >> row >> col >> value;
                seg.set(row, col, value);
            }else if(type == "ADD"){
                int row, col;
                long long value;
                std::cin >> row >> col >> value;
                seg.apply(row, col, value);
            }else if(type == "GET"){
                int row, col;
                std::cin >> row >> col;
                std::cout << seg.get(row, col) << '\n';
            }else if(type == "PROD"){
                int row_begin, col_begin, row_end, col_end;
                std::cin >> row_begin >> col_begin >> row_end >> col_end;
                std::cout << seg.prod(row_begin, col_begin, row_end, col_end) << '\n';
            }else if(type == "ALL"){
                std::cout << seg.all_prod() << '\n';
            }
        }
        return 0;
    }
    test_basic();
    test_exceptions();
}
