// competitive-verifier: PROBLEM https://www.spoj.com/problems/ROHAAN/

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/sum_shortest_path_query_costs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long INF = (1LL << 60);
    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int test_case = 1; test_case <= test_count; test_case++){
        int station_count;
        std::cin >> station_count;
        std::vector<std::vector<long long>> distance(
            static_cast<std::size_t>(station_count),
            std::vector<long long>(static_cast<std::size_t>(station_count))
        );
        for(auto& row: distance){
            for(long long& value: row) std::cin >> value;
        }

        int order_count;
        std::cin >> order_count;
        std::vector<ShortestPathCostQuery> orders(
            static_cast<std::size_t>(order_count)
        );
        for(auto& order: orders){
            std::cin >> order.from >> order.to;
            --order.from;
            --order.to;
        }
        std::cout << "Case #" << test_case << ": "
                  << sum_shortest_path_query_costs(
                         std::move(distance), orders, INF
                     )
                  << '\n';
    }
}
