// competitive-verifier: PROBLEM https://www.spoj.com/problems/DYNACON1/

#include <iostream>
#include <string>
#include "../../src/structure/tree/dynamic_forest_connectivity.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    DynamicForestConnectivity<100000> forest(n);
    while(q-- != 0){
        std::string command;
        int u, v;
        std::cin >> command >> u >> v;
        --u;
        --v;
        if(command == "add") (void)forest.add_edge(u, v);
        else if(command == "rem") (void)forest.remove_edge(u, v);
        else std::cout << (forest.connected(u, v) ? "YES\n" : "NO\n");
    }
}
