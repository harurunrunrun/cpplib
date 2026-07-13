// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/structure/other/sqrt_tree.hpp"

struct AffineSemigroup{
    using S = std::pair<long long, long long>;
    static constexpr long long mod = 1000000007;

    S op(const S& left, const S& right) const{
        return {
            right.first * left.first % mod,
            (right.first * left.second + right.second) % mod
        };
    }
};

constexpr AffineSemigroup affine_semigroup{};

void self_test(){
    using S = AffineSemigroup::S;
    std::vector<S> values{
        {2, 1}, {3, 4}, {5, 9}, {7, 2}, {11, 8}, {13, 6}, {17, 5}
    };
    SqrtTree<affine_semigroup, 16> tree;
    tree.build(values);
    assert(tree.size() == static_cast<int>(values.size()));
    for(int left = 0; left < static_cast<int>(values.size()); left++){
        S expected = values[static_cast<std::size_t>(left)];
        for(int right = left + 1; right <= static_cast<int>(values.size()); right++){
            if(right > left + 1){
                expected = affine_semigroup.op(
                    expected,
                    values[static_cast<std::size_t>(right - 1)]
                );
            }
            assert(tree.prod(left, right) == expected);
        }
    }
    assert(tree.all_prod() == tree.prod(0, tree.size()));
    for(int i = 0; i < tree.size(); i++) assert(tree.get(i) == values[static_cast<std::size_t>(i)]);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<AffineSemigroup::S> values(static_cast<std::size_t>(n));
    for(auto& [a, b]: values) std::cin >> a >> b;
    SqrtTree<affine_semigroup, 512> tree(values);

    auto all = tree.all_prod();
    std::cout << tree.size() << ' ' << tree.empty() << ' '
              << all.first << ' ' << all.second << '\n';
    for(int i = 0; i < n; i++){
        auto value = tree.get(i);
        if(i) std::cout << ' ';
        std::cout << value.first << ':' << value.second;
    }
    std::cout << '\n';
    while(q--){
        int left, right;
        std::cin >> left >> right;
        auto answer = tree.prod(left, right);
        std::cout << answer.first << ' ' << answer.second << '\n';
    }
}
