// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/approximate/routing/insertion_heuristics.hpp"
#include "../../src/approximate/routing/tour_cost.hpp"

int main(){
    using namespace approximate::routing;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::string command;
        std::size_t size = 0;
        std::cin >> command >> size;
        std::size_t start = 0;
        std::size_t regret_k = 2;
        if(command != "RNN") std::cin >> start;
        if(command == "REGRET") std::cin >> regret_k;
        std::vector<std::vector<long long>> distance(
            size,
            std::vector<long long>(size)
        );
        for(auto& row : distance){
            for(long long& value : row) std::cin >> value;
        }

        std::vector<int> tour;
        if(command == "NN"){
            tour = nearest_neighbor_tour(distance, start);
        }else if(command == "RNN"){
            tour = repeated_nearest_neighbor_tour(distance);
        }else if(command == "NEAREST"){
            tour = nearest_insertion_tour(distance, start);
        }else if(command == "CHEAPEST"){
            tour = cheapest_insertion_tour(distance, start);
        }else if(command == "FARTHEST"){
            tour = farthest_insertion_tour(distance, start);
        }else{
            tour = regret_insertion_tour(distance, start, regret_k);
        }
        std::cout << tour.size();
        for(const int vertex : tour) std::cout << ' ' << vertex;
        std::cout << " | " << tour_cost(distance, tour) << '\n';
    }
}
