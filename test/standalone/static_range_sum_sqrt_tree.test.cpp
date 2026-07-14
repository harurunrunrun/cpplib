// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/static_range_sum_sqrt_tree.hpp"

void self_test(){
    StaticRangeSumSqrtTree<long long, 16> tree({3, -1, 4});
    assert(tree.size() == 3);
    assert(!tree.empty());
    assert(tree.get(1) == -1);
    assert(tree.sum(0, 0) == 0);
    assert(tree.sum(0, 3) == 6);
    assert(tree.sum(1, 3) == 3);
    assert(tree.all_sum() == 6);
    bool thrown = false;
    try{
        (void)tree.sum(-1, -1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    tree.build({});
    assert(tree.empty());
    assert(tree.all_sum() == 0);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    StaticRangeSumSqrtTree<long long, 512> tree(values);
    while(q-- != 0){
        int left, right;
        std::cin >> left >> right;
        std::cout << tree.sum(left, right) << '\n';
    }
}
