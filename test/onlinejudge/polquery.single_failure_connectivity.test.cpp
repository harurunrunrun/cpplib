#define PROBLEM "https://www.spoj.com/problems/POLQUERY/"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/single_failure_connectivity.hpp"

namespace{

std::uint64_t edge_key(int first, int second){
    if(second < first) std::swap(first, second);
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(first)) << 32) |
           static_cast<std::uint32_t>(second);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    int edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    std::unordered_map<std::uint64_t, int> edge_id;
    edge_id.reserve(static_cast<std::size_t>(edge_count) * 2 + 1);
    for(int id = 0; id < edge_count; ++id){
        auto& [from, to] = edges[static_cast<std::size_t>(id)];
        std::cin >> from >> to;
        --from;
        --to;
        edge_id.emplace(edge_key(from, to), id);
    }

    SingleFailureConnectivity<100000, 500000> oracle(vertex_count, edges);
    int query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        int type;
        int first;
        int second;
        std::cin >> type >> first >> second;
        --first;
        --second;
        bool connected;
        if(type == 1){
            int edge_from;
            int edge_to;
            std::cin >> edge_from >> edge_to;
            --edge_from;
            --edge_to;
            connected = oracle.connected_without_edge(
                first,
                second,
                edge_id.at(edge_key(edge_from, edge_to))
            );
        }else{
            int removed;
            std::cin >> removed;
            --removed;
            connected = oracle.connected_without_vertex(first, second, removed);
        }
        std::cout << (connected ? "da\n" : "ne\n");
    }
}
