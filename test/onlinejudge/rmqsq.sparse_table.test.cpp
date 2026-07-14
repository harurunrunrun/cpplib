// competitive-verifier: PROBLEM https://www.spoj.com/problems/RMQSQ/

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/other/sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr MinMonoid<int> minimum_monoid{};

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;

    auto table = std::make_unique<SparseTable<minimum_monoid, 100000>>(values);
    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        if(right < left) std::swap(left, right);
        std::cout << table->prod(left, right + 1) << '\n';
    }
}
