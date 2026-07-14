// competitive-verifier: PROBLEM https://www.spoj.com/problems/GRASSPLA/

#include <iostream>
#include "../../src/algorithm/tree/grass_planting_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    GrassPlantingTree<100000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u - 1, v - 1);
    }
    tree.build();
    while(q-- != 0){
        char operation;
        int u, v;
        std::cin >> operation >> u >> v;
        --u;
        --v;
        if(operation == 'P') tree.plant_path(u, v);
        else std::cout << tree.edge_plant_count(u, v) << '\n';
    }
}
