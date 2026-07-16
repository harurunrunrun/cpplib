// competitive-verifier: PROBLEM https://www.spoj.com/problems/PARTY/
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/knapsack_max_value_min_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int capacity;
    int item_count;
    while(std::cin >> capacity >> item_count
        && (capacity != 0 || item_count != 0)){
        std::vector<KnapsackItem> items(
            static_cast<std::size_t>(item_count)
        );
        for(auto& item: items){
            std::cin >> item.cost >> item.value;
        }
        const auto optimum = knapsack_max_value_min_cost(capacity, items);
        std::cout << optimum.minimum_cost << ' '
                  << optimum.maximum_value << '\n';
    }
    return 0;
}
