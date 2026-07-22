// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/sum_shortest_path_query_costs.hpp"

void self_test(){
    constexpr long long inf = (1LL << 60);
    assert(sum_shortest_path_query_costs<long long>(
        {{0, 1, 1}, {1, 0, 1}, {1, 9, 0}},
        {{1, 0}, {2, 1}},
        inf
    ) == 3);

    bool thrown = false;
    try{
        (void)sum_shortest_path_query_costs<long long>(
            {{0, inf}, {inf, 0}}, {{0, 1}}, inf
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)sum_shortest_path_query_costs<long long>(
            {{0, 0}, {-1, 0}}, {{0, 1}}, inf
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    const std::vector<ShortestPathCostEdge<long long>> nonnegative_edges{
        {0, 1, 4}, {0, 2, 1}, {2, 1, 2}, {1, 3, 1}, {2, 3, 8},
    };
    const std::vector<ShortestPathCostQuery> sparse_queries{
        {0, 3}, {0, 1}, {2, 3},
    };
    assert(sum_nonnegative_shortest_path_query_costs(
        4, nonnegative_edges, sparse_queries, inf
    ) == 10);

    const std::vector<ShortestPathCostEdge<long long>> signed_edges{
        {0, 1, 2}, {0, 2, 5}, {1, 2, -4}, {2, 3, 3}, {1, 3, 8},
    };
    assert(sum_shortest_path_query_costs_johnson(
        4, signed_edges, sparse_queries, inf
    ) == 6);
    assert(sum_shortest_path_query_costs_johnson<long long>(
        0, {}, {}, inf
    ) == 0);

    thrown = false;
    try{
        (void)sum_nonnegative_shortest_path_query_costs(
            2,
            std::vector<ShortestPathCostEdge<long long>>{{0, 1, -1}},
            std::vector<ShortestPathCostQuery>{{0, 1}},
            inf
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)sum_shortest_path_query_costs_johnson(
            2,
            std::vector<ShortestPathCostEdge<long long>>{
                {0, 1, -1}, {1, 0, -1},
            },
            std::vector<ShortestPathCostQuery>{{0, 1}},
            inf
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    constexpr long long inf = (1LL << 60);
    std::string mode;
    if(!(std::cin >> mode)) return 0;
    int vertex_count, edge_count = 0, query_count;
    if(mode == "M"){
        std::cin >> vertex_count >> query_count;
        std::vector<std::vector<long long>> distance(
            static_cast<std::size_t>(vertex_count),
            std::vector<long long>(static_cast<std::size_t>(vertex_count))
        );
        for(auto& row: distance){
            for(auto& value: row) std::cin >> value;
        }
        std::vector<ShortestPathCostQuery> queries(
            static_cast<std::size_t>(query_count)
        );
        for(auto& query: queries) std::cin >> query.from >> query.to;
        std::cout << sum_shortest_path_query_costs(
                         std::move(distance), queries, inf
                     )
                  << '\n';
        return 0;
    }
    std::cin >> vertex_count >> edge_count >> query_count;
    std::vector<ShortestPathCostEdge<long long>> edges(
        static_cast<std::size_t>(edge_count)
    );
    for(auto& edge: edges) std::cin >> edge.from >> edge.to >> edge.cost;
    std::vector<ShortestPathCostQuery> queries(
        static_cast<std::size_t>(query_count)
    );
    for(auto& query: queries) std::cin >> query.from >> query.to;
    if(mode == "N"){
        std::cout << sum_nonnegative_shortest_path_query_costs(
                         vertex_count, edges, queries, inf
                     )
                  << '\n';
    }else{
        assert(mode == "J");
        std::cout << sum_shortest_path_query_costs_johnson(
                         vertex_count, edges, queries, inf
                     )
                  << '\n';
    }
}
