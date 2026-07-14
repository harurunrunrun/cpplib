// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DSL_1_B

#include <iostream>

#include "../../src/structure/dsu/weighted_dsu.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;
    static WeightedDSU<long long, 1000000> dsu(vertex_count);
    while(query_count--){
        int type, u, v;
        std::cin >> type >> u >> v;
        if(type == 0){
            long long weight;
            std::cin >> weight;
            dsu.merge(u, v, weight);
        }else{
            const auto difference = dsu.diff_if_connected(u, v);
            if(difference) std::cout << *difference << '\n';
            else std::cout << "?\n";
        }
    }
}
