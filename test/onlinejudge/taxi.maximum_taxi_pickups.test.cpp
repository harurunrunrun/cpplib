// competitive-verifier: PROBLEM https://www.spoj.com/problems/TAXI/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/matching/maximum_taxi_pickups.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int passenger_count, taxi_count;
        long long speed, pickup_time;
        std::cin >> passenger_count >> taxi_count >> speed >> pickup_time;
        std::vector<std::pair<int, int>> passengers(
            static_cast<std::size_t>(passenger_count)
        );
        std::vector<std::pair<int, int>> taxis(
            static_cast<std::size_t>(taxi_count)
        );
        for(auto& [x, y]: passengers) std::cin >> x >> y;
        for(auto& [x, y]: taxis) std::cin >> x >> y;
        std::cout << maximum_taxi_pickups(
            passengers, taxis, speed, pickup_time
        ) << '\n';
    }
}
