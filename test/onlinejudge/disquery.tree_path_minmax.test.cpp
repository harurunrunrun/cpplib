// competitive-verifier: PROBLEM https://www.spoj.com/problems/DISQUERY/

#include <iostream>
#include "../../src/algorithm/tree/tree_path_minmax.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    TreePathMinmax<int, 100000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int u, v, weight;
        std::cin >> u >> v >> weight;
        tree.add_edge(u - 1, v - 1, weight);
    }
    tree.build();
    int q;
    std::cin >> q;
    while(q-- != 0){
        int u, v;
        std::cin >> u >> v;
        const auto [minimum, maximum] = tree.path_minmax(u - 1, v - 1);
        std::cout << minimum << ' ' << maximum << '\n';
    }
}
