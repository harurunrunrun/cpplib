// competitive-verifier: PROBLEM https://www.spoj.com/problems/DYNACON2/

#include <iostream>
#include <string>
#include "../../src/structure/graph/dynamic_graph_connectivity.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    DynamicGraphConnectivity<100000> graph(n);
    while(q-- != 0){
        std::string command;
        int u, v;
        std::cin >> command >> u >> v;
        --u;
        --v;
        if(command == "add") (void)graph.add_edge(u, v);
        else if(command == "rem") (void)graph.remove_edge(u, v);
        else std::cout << (graph.connected(u, v) ? "YES\n" : "NO\n");
    }
}
