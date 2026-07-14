// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE/

#include <iostream>
#include <string>
#include "../../src/structure/tree/qtree_edge_maximum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests-- != 0){
        int n;
        std::cin >> n;
        QTreeEdgeMaximum<int, 10000> tree(n);
        for(int edge = 1; edge < n; ++edge){
            int u, v, weight;
            std::cin >> u >> v >> weight;
            tree.add_edge(u - 1, v - 1, weight);
        }
        tree.build();
        std::string command;
        while(std::cin >> command && command != "DONE"){
            int first, second;
            std::cin >> first >> second;
            if(command == "QUERY") std::cout << tree.path_maximum(first - 1, second - 1) << '\n';
            else tree.change_edge(first - 1, second);
        }
    }
}
