// competitive-verifier: PROBLEM https://www.spoj.com/problems/GONDOR/

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/ordered_quota_euclidean_activation_times.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    if(!(std::cin >> vertex_count)) return 0;
    std::vector<OrderedQuotaEuclideanActivator> activators(
        static_cast<std::size_t>(vertex_count)
    );
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        auto& activator = activators[static_cast<std::size_t>(vertex)];
        std::cin >> activator.x >> activator.y >> activator.quota;
        activator.priority_order.resize(
            static_cast<std::size_t>(vertex_count - 1)
        );
        for(int& target: activator.priority_order){
            std::cin >> target;
            --target;
        }
    }

    const auto answer =
        ordered_quota_euclidean_activation_times(activators, 0);
    std::cout << std::fixed << std::setprecision(6);
    for(const long double time: answer){
        std::cout << time << '\n';
    }
}
