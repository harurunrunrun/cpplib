// competitive-verifier: PROBLEM https://www.spoj.com/problems/RPLN/

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

    int test_count;
    std::cin >> test_count;
    for(int test = 1; test <= test_count; test++){
        int size, query_count;
        std::cin >> size >> query_count;
        std::vector<int> values(static_cast<std::size_t>(size));
        for(int& value: values) std::cin >> value;
        auto table = std::make_unique<SparseTable<minimum_monoid, 100000>>(values);

        std::cout << "Scenario #" << test << ":\n";
        while(query_count-- != 0){
            int left, right;
            std::cin >> left >> right;
            std::cout << table->prod(left - 1, right) << '\n';
        }
    }
}
