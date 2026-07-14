#define PROBLEM "https://www.spoj.com/problems/MULTQ3/"

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/residue_count_monoid.hpp"

constexpr AddResidueCountMonoidAct<int, 3> residue_count_action{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int query_count;
    std::cin >> size >> query_count;
    std::vector<ResidueCountAggregate<int, 3>> initial(
        static_cast<std::size_t>(size),
        residue_count_singleton<int, 3>(0)
    );
    using Tree = LazySegtree<residue_count_action, 100000>;
    auto tree = std::make_unique<Tree>(initial);

    while(query_count-- != 0){
        int type;
        int left;
        int right;
        std::cin >> type >> left >> right;
        if(type == 0){
            tree->apply(left, right + 1, ResidueShift<3>{1});
        }else{
            std::cout << tree->prod(left, right + 1).count[0] << '\n';
        }
    }
}
