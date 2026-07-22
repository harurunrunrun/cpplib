// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/single_failure_connectivity.hpp"

namespace{

void self_test(){
    {
        const std::vector<std::pair<int, int>> edges = {
            {0, 1}, {1, 2}, {1, 3}, {3, 4}
        };
        SingleFailureConnectivity<8, 16> oracle(5, edges);
        assert(oracle.vertex_count() == 5);
        assert(oracle.edge_count() == 4);
        assert(!oracle.connected_without_edge(0, 4, 0));
        assert(oracle.connected_without_edge(2, 4, 0));
        assert(!oracle.connected_without_vertex(0, 2, 1));
        assert(!oracle.connected_without_vertex(2, 4, 1));
        assert(oracle.connected_without_vertex(2, 4, 0));
        assert(!oracle.connected_without_vertex(0, 4, 0));
    }
    {
        const std::vector<std::pair<int, int>> edges = {
            {0, 1}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 2}
        };
        SingleFailureConnectivity<4, 8> oracle(3, edges);
        for(int edge_id = 0; edge_id < static_cast<int>(edges.size()); ++edge_id){
            assert(oracle.connected_without_edge(0, 2, edge_id));
        }
        for(int removed = 0; removed < 3; ++removed){
            const int first = (removed + 1) % 3;
            const int second = (removed + 2) % 3;
            assert(oracle.connected_without_vertex(first, second, removed));
            assert(!oracle.connected_without_vertex(removed, second, removed));
        }
    }
    {
        SingleFailureConnectivity<4, 4> oracle(2, {{0, 1}});
        bool threw = false;
        try{
            oracle.build(2, {{0, 2}});
        }catch(const std::runtime_error&){
            threw = true;
        }
        assert(threw);
        assert(oracle.vertex_count() == 0);
        assert(oracle.edge_count() == 0);
        threw = false;
        try{
            static_cast<void>(oracle.connected_without_edge(0, 1, 0));
        }catch(const std::runtime_error&){
            threw = true;
        }
        assert(threw);
    }
}

} // namespace

int main(){
    int vertex_count;
    int edge_count;
    int query_count;
    if(!(std::cin >> vertex_count >> edge_count >> query_count)){
        self_test();
        return 0;
    }

    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [from, to]: edges) std::cin >> from >> to;

    SingleFailureConnectivity<100000, 500000> oracle(vertex_count, edges);
    for(int query = 0; query < query_count; ++query){
        int type;
        int first;
        int second;
        int removed;
        std::cin >> type >> first >> second >> removed;
        const bool answer = type == 1
            ? oracle.connected_without_edge(first, second, removed)
            : oracle.connected_without_vertex(first, second, removed);
        std::cout << (answer ? 1 : 0) << '\n';
    }
}
