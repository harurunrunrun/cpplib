// competitive-verifier: PROBLEM https://www.spoj.com/problems/MKBUDGET/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/minimum_temporary_staff_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_number = 1;
    int month_count;
    while(std::cin >> month_count && month_count != 0){
        long long hiring_cost, monthly_salary, severance_cost;
        std::cin >> hiring_cost >> monthly_salary >> severance_cost;
        std::vector<int> required_workers(static_cast<std::size_t>(month_count));
        for(int& required: required_workers) std::cin >> required;
        std::cout << "Case " << case_number++ << ", cost = $"
                  << minimum_temporary_staff_cost(
                         required_workers,
                         hiring_cost,
                         monthly_salary,
                         severance_cost
                     )
                  << '\n';
    }
}
