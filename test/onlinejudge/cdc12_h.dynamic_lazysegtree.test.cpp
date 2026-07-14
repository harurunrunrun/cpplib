#define PROBLEM "https://www.spoj.com/problems/CDC12_H/"

#include <iostream>
#include <memory>
#include <string>

#include "../../src/structure/segtree/dynamic_lazysegtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr AddSumMonoidAct<long long> range_add_sum{};

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int case_index = 1; case_index <= test_count; case_index++){
        int size, query_count;
        std::cin >> size >> query_count;
        auto tree = std::make_unique<
            DynamicLazySegtree<range_add_sum, 200000>
        >(size);

        std::cout << "Scenario #" << case_index << ":\n";
        while(query_count-- != 0){
            std::string command;
            int left, right;
            std::cin >> command >> left >> right;
            --left;
            if(command == "modification"){
                tree->apply(left, right, 1LL);
                std::cout << "OK\n";
            }else{
                std::cout << tree->prod(left, right) << '\n';
            }
        }
    }
}
