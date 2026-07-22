// competitive-verifier: PROBLEM https://www.spoj.com/problems/COT2/

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/query/offline_tree_path_distinct.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    OfflineTreePathDistinct<int, 40000> tree(values);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u - 1, v - 1);
    }
    tree.build();
    for(int query = 0; query < q; ++query){
        int u, v;
        std::cin >> u >> v;
        tree.add_query(u - 1, v - 1);
    }
    for(int answer: tree.solve()) std::cout << answer << '\n';
}
