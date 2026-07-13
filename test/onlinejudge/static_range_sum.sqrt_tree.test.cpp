// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_sum

#include <iostream>
#include <vector>

#include "../../src/structure/other/sqrt_tree.hpp"

struct AddSemigroup{
    using S = long long;
    S op(S a, S b) const{ return a + b; }
};

constexpr AddSemigroup add_semigroup{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    SqrtTree<add_semigroup, 500000> tree(values);
    while(q--){
        int left, right;
        std::cin >> left >> right;
        std::cout << (left == right ? 0 : tree.prod(left, right)) << '\n';
    }
}
