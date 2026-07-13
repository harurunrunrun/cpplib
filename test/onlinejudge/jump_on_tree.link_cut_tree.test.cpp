// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/jump_on_tree

#include <iostream>

#include "../../src/structure/tree/link_cut_tree.hpp"
#include "../../src/structure/types/monoid.hpp"

int add_int(int left, int right){ return left + right; }
int zero_int(){ return 0; }
constexpr Monoid<add_int, zero_int> sum_monoid;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    std::cin >> n >> query_count;
    LinkCutTree<sum_monoid, 500000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int left, right;
        std::cin >> left >> right;
        tree.link(left, right);
    }
    while(query_count-- > 0){
        int start, goal, index;
        std::cin >> start >> goal >> index;
        std::cout << tree.jump(start, goal, index) << '\n';
    }
}
