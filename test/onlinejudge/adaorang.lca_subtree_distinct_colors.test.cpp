// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADAORANG/

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/lca_subtree_distinct_colors.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests-- != 0){
        int n, q, root;
        std::cin >> n >> q >> root;
        std::vector<int> colors(static_cast<std::size_t>(n));
        for(int& color: colors) std::cin >> color;
        LcaSubtreeDistinctColors<int, 450000> tree(colors);
        for(int edge = 1; edge < n; ++edge){
            int u, v;
            std::cin >> u >> v;
            tree.add_edge(u, v);
        }
        tree.build(root);
        while(q-- != 0){
            int u, v;
            std::cin >> u >> v;
            std::cout << tree.count_at_lca_subtree(u, v) << '\n';
        }
    }
}
