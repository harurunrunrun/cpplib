// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADATAXES/

#include <iostream>
#include <vector>

#include "../../src/structure/other/static_range_product_less_equal_mod.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size;
    int query_count;
    std::cin >> size >> query_count;
    std::vector<int> heights(static_cast<std::size_t>(size));
    for(int& height : heights) std::cin >> height;
    StaticRangeProductLessEqualMod<1000000007, 300000> tree(heights);
    while(query_count-- != 0){
        int left;
        int right;
        int height_limit;
        std::cin >> left >> right >> height_limit;
        std::cout << tree.product_less_equal(
            left, right + 1, height_limit
        ) << '\n';
    }
}
