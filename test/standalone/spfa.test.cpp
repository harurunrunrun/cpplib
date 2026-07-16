// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/spfa.hpp"

using Weight = long long;
using Graph = std::vector<std::vector<SpfaEdge<Weight>>>;

void test_shortest_paths(){
    Graph graph(5);
    graph[0].push_back({1, 3});
    graph[0].push_back({2, 10});
    graph[1].push_back({2, -5});
    graph[2].push_back({3, 2});
    const auto result = spfa(graph, 0);
    assert(!result.has_reachable_negative_cycle);
    assert((result.reachable == std::vector<char>{1, 1, 1, 1, 0}));
    assert((result.negative_cycle_affected ==
        std::vector<char>{0, 0, 0, 0, 0}));
    assert(result.distance[0] == 0);
    assert(result.distance[1] == 3);
    assert(result.distance[2] == -2);
    assert(result.distance[3] == 0);
    assert(result.parent[2] == 1);
}

void test_negative_cycle(){
    Graph graph(6);
    graph[0].push_back({1, 0});
    graph[1].push_back({2, -2});
    graph[2].push_back({1, 1});
    graph[2].push_back({3, 4});
    graph[4].push_back({5, -1});
    graph[5].push_back({4, -1});
    const auto result = spfa(graph, 0);
    assert(result.has_reachable_negative_cycle);
    assert((result.reachable == std::vector<char>{1, 1, 1, 1, 0, 0}));
    assert((result.negative_cycle_affected ==
        std::vector<char>{0, 1, 1, 1, 0, 0}));
}

void test_multiple_sources(){
    Graph graph(4);
    graph[0].push_back({2, 5});
    graph[1].push_back({2, -1});
    graph[2].push_back({3, 2});
    const auto result = spfa(graph, std::vector<int>{0, 1, 1});
    assert((result.distance ==
        std::vector<Weight>{0, 0, -1, 1}));
}

void test_validation(){
    bool thrown = false;
    try{
        Graph graph(1);
        graph[0].push_back({1, 0});
        (void)spfa(graph, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)spfa(Graph(1), 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int case_count;
    if(std::cin >> case_count){
        while(case_count--){
            int vertex_count, edge_count, source_count;
            std::cin >> vertex_count >> edge_count >> source_count;
            std::vector<int> sources(static_cast<std::size_t>(source_count));
            for(int& source: sources) std::cin >> source;
            Graph graph(static_cast<std::size_t>(vertex_count));
            for(int index = 0; index < edge_count; ++index){
                int from, to;
                Weight cost;
                std::cin >> from >> to >> cost;
                graph[static_cast<std::size_t>(from)].push_back({to, cost});
            }
            const auto result = spfa(graph, sources);
            std::cout << result.has_reachable_negative_cycle;
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                const std::size_t index = static_cast<std::size_t>(vertex);
                if(result.negative_cycle_affected[index]){
                    std::cout << " N";
                }else if(!result.reachable[index]){
                    std::cout << " U";
                }else{
                    std::cout << ' ' << result.distance[index];
                }
            }
            std::cout << '\n';
        }
        return 0;
    }
    test_shortest_paths();
    test_negative_cycle();
    test_multiple_sources();
    test_validation();
}
