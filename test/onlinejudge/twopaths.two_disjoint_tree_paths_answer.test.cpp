// competitive-verifier: PROBLEM https://www.spoj.com/problems/TWOPATHS/

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/tree/two_disjoint_tree_paths_answer.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(n - 1));
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u - 1, v - 1);
    }
    std::cout << two_disjoint_tree_paths_answer<100000>(n, edges).product << '\n';
}
