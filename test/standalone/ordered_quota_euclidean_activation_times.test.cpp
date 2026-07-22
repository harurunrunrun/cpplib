// competitive-verifier: STANDALONE

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/specialized/ordered_quota_euclidean_activation_times.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout << std::fixed << std::setprecision(6);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, initial_vertex;
        std::cin >> vertex_count >> initial_vertex;
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
            }
        }

        const auto times = ordered_quota_euclidean_activation_times(
            activators, initial_vertex
        );
        for(std::size_t vertex = 0; vertex < times.size(); ++vertex){
            if(vertex != 0) std::cout << ' ';
            if(std::isinf(times[vertex])){
                std::cout << "INF";
            }else{
                std::cout << times[vertex];
            }
        }
        std::cout << '\n';
    }
}
