// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE2/

#include <iostream>
#include <string>
#include "../../src/algorithm/tree/qtree_distance_kth_vertex.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests-- != 0){
        int n;
        std::cin >> n;
        QTreeDistanceKthVertex<long long, 10000> tree(n);
        for(int edge = 1; edge < n; ++edge){
            int u, v;
            long long weight;
            std::cin >> u >> v >> weight;
            tree.add_edge(u - 1, v - 1, weight);
        }
        tree.build();
        std::string command;
        while(std::cin >> command && command != "DONE"){
            int u, v;
            std::cin >> u >> v;
            if(command == "DIST") std::cout << tree.distance(u - 1, v - 1) << '\n';
            else{
                int order;
                std::cin >> order;
                std::cout << tree.kth_vertex(u - 1, v - 1, order - 1) + 1 << '\n';
            }
        }
        std::cout << '\n';
    }
}
