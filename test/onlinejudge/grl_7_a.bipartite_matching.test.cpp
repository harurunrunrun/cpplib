// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_7_A

#include <iostream>

#include "../../src/algorithm/matching/bipartite/bipartite_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int left_size, right_size, edge_count;
    std::cin >> left_size >> right_size >> edge_count;
    BipartiteMatching matching(left_size, right_size);
    while(edge_count--){
        int left, right;
        std::cin >> left >> right;
        matching.add_edge(left, right);
    }
    std::cout << matching.solve().size << '\n';
}
