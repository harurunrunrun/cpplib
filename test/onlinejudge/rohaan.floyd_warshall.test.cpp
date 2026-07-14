// competitive-verifier: PROBLEM https://www.spoj.com/problems/ROHAAN/

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/floyd_warshall.hpp"

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

        const auto shortest = floyd_warshall(std::move(distance), INF);
        int order_count;
        std::cin >> order_count;
        long long total = 0;
        for(int order = 0; order < order_count; order++){
            int source, destination;
            std::cin >> source >> destination;
            source--;
            destination--;
            total += shortest.dist[static_cast<std::size_t>(source)]
                                  [static_cast<std::size_t>(destination)];
        }
        std::cout << "Case #" << test_case << ": " << total << '\n';
    }
}
