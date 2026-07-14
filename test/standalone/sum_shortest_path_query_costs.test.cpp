// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/sum_shortest_path_query_costs.hpp"

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
}

int main(){
    self_test();

    constexpr long long inf = (1LL << 60);
    int vertex_count, query_count;
    if(!(std::cin >> vertex_count >> query_count)) return 0;
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
}
