#define PROBLEM "https://www.spoj.com/problems/HORRIBLE/"

#include <iostream>
#include <memory>

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
    while(test_count-- != 0){
        int size, command_count;
        std::cin >> size >> command_count;
        auto tree = std::make_unique<
            DynamicLazySegtree<range_add_sum, 100000>
        >(size);

        while(command_count-- != 0){
            int type, left, right;
            std::cin >> type >> left >> right;
            --left;
            if(type == 0){
                long long value;
                std::cin >> value;
                tree->apply(left, right, value);
            }else{
                std::cout << tree->prod(left, right) << '\n';
            }
        }
    }
}
