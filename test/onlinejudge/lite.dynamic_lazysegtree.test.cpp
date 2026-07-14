#define PROBLEM "https://www.spoj.com/problems/LITE/"

#include <iostream>
#include <memory>

#include "../../src/structure/segtree/dynamic_lazysegtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr FlipCountMonoidAct<int> flip_count{};

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int light_count, command_count;
    std::cin >> light_count >> command_count;
    auto tree = std::make_unique<
        DynamicLazySegtree<flip_count, 100000>
    >(light_count);

    while(command_count-- != 0){
        int type, left, right;
        std::cin >> type >> left >> right;
        --left;
        if(type == 0){
            tree->apply(left, right, true);
        }else{
            std::cout << tree->prod(left, right) << '\n';
        }
    }
}
