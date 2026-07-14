// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADATAXES/

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/other/monoid_merge_sort_tree.hpp"
#include "../../src/structure/types/monoid.hpp"

constexpr int MOD = 1000000007;

constexpr int multiply_mod(int left, int right){
    return static_cast<int>(
        static_cast<long long>(left) * static_cast<long long>(right) % MOD
    );
}

constexpr int one_mod(){ return 1; }
constexpr Monoid<multiply_mod, one_mod> product_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int query_count;
    std::cin >> size >> query_count;
    std::vector<int> heights(static_cast<std::size_t>(size));
    for(int& height : heights) std::cin >> height;

    auto tree = std::make_unique<
        MonoidMergeSortTree<int, product_monoid, 300000>
    >(heights, heights);
    while(query_count-- != 0){
        int left;
        int right;
        int height_limit;
        std::cin >> left >> right >> height_limit;
        std::cout << tree->fold_less_equal(
            left, right + 1, height_limit
        ) << '\n';
    }
}
