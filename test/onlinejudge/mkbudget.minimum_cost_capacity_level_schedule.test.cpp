// competitive-verifier: PROBLEM https://www.spoj.com/problems/MKBUDGET/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/scheduling/minimum_cost_capacity_level_schedule.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_number = 1;
    int period_count;
    while(std::cin >> period_count && period_count != 0){
        long long increase_cost, level_cost, decrease_cost;
        std::cin >> increase_cost >> level_cost >> decrease_cost;
        std::vector<int> minimum_levels(
            static_cast<std::size_t>(period_count)
        );
        for(int& level: minimum_levels) std::cin >> level;
        std::cout << "Case " << case_number++ << ", cost = $"
                  << minimum_cost_capacity_level_schedule(
                         minimum_levels,
                         increase_cost,
                         level_cost,
                         decrease_cost
                     )
                  << '\n';
    }
    return 0;
}
