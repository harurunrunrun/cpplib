// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/knapsack_max_value_min_cost.hpp"

int main(){
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
        const auto answer = knapsack_max_value_min_cost(capacity, items);
        std::cout << answer.minimum_cost << ' '
                  << answer.maximum_value << '\n';
    }
}
