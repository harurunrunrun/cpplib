// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/prefix_sum.hpp"

void test_prefix_sum(){
    PrefixSum<long long, 8> sum(std::vector<long long>{3, -1, 4, 1, 5});
    assert(sum.size() == 5);
    assert(!sum.empty());
    assert(sum.prefix_sum(0) == 0);
    assert(sum.prefix_sum(5) == 12);
    assert(sum.sum(1, 4) == 4);
    assert(sum.get(2) == 4);

    sum.build(std::vector<long long>{10, 20});
    assert(sum.size() == 2);
    assert(sum.sum(0, 2) == 30);
}

void test_prefix_sum_2d(){
    PrefixSum2D<long long, 4, 5> sum(
        std::vector<std::vector<long long>>{{1, 2, 3}, {4, 5, 6}}
    );
    assert(sum.height() == 2);
    assert(sum.width() == 3);
    assert(!sum.empty());
    assert(sum.sum(0, 0, 2, 3) == 21);
    assert(sum.sum(0, 1, 2, 3) == 16);
    assert(sum.sum(1, 0, 2, 2) == 9);
    assert(sum.get(1, 2) == 6);
}

void test_exceptions(){
    bool thrown = false;
    try{
        PrefixSum<int, 2> sum(3);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        PrefixSum2D<int, 2, 2> sum(
            std::vector<std::vector<int>>{{1, 2}, {3}}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n, query_count;
    if(std::cin >> n >> query_count){
        std::vector<long long> values(static_cast<std::size_t>(n));
        for(auto& value: values) std::cin >> value;
        PrefixSum<long long, 256> one_dimensional(values);
        while(query_count--){
            int left, right;
            std::cin >> left >> right;
            std::cout << one_dimensional.sum(left, right) << '\n';
        }

        int height, width;
        std::cin >> height >> width >> query_count;
        std::vector<std::vector<long long>> matrix(
            static_cast<std::size_t>(height),
            std::vector<long long>(static_cast<std::size_t>(width))
        );
        for(auto& row: matrix){
            for(auto& value: row) std::cin >> value;
        }
        PrefixSum2D<long long, 32, 32> two_dimensional(matrix);
        while(query_count--){
            int row_begin, col_begin, row_end, col_end;
            std::cin >> row_begin >> col_begin >> row_end >> col_end;
            std::cout << two_dimensional.sum(row_begin, col_begin, row_end, col_end) << '\n';
        }
        return 0;
    }

    test_prefix_sum();
    test_prefix_sum_2d();
    test_exceptions();
}
