// competitive-verifier: PROBLEM https://www.spoj.com/problems/POLQUERY/

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/endpoint_failure_connectivity.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [u, v]: edges){
        std::cin >> u >> v;
        --u;
        --v;
    }
    EndpointFailureConnectivity<100000, 500000> oracle(n, edges);
    int q;
    std::cin >> q;
    while(q-- != 0){
        int type, first, second;
        std::cin >> type >> first >> second;
        --first;
        --second;
        bool connected;
        if(type == 1){
            int edge_left, edge_right;
            std::cin >> edge_left >> edge_right;
            connected = oracle.connected_without_edge(first, second, edge_left - 1, edge_right - 1);
        }else{
            int removed;
            std::cin >> removed;
            connected = oracle.connected_without_vertex(first, second, removed - 1);
        }
        std::cout << (connected ? "da\n" : "ne\n");
    }
}
