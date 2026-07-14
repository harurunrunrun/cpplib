// competitive-verifier: PROBLEM https://www.spoj.com/problems/DYNACON2/

#include <iostream>
#include <string>

#include "../../src/structure/graph/online_dynamic_connectivity.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;
    OnlineDynamicConnectivity<100000> graph(vertex_count);
    while(query_count-- != 0){
        std::string command;
        int left, right;
        std::cin >> command >> left >> right;
        --left;
        --right;
        if(command == "add"){
            (void)graph.add_edge(left, right);
        }else if(command == "rem"){
            (void)graph.erase_edge(left, right);
        }else{
            std::cout << (graph.same(left, right) ? "YES\n" : "NO\n");
        }
    }
}
