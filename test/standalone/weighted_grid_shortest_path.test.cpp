// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/weighted_grid_shortest_path.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        int row_count, column_count;
        int start_row, start_column, target_row, target_column;
        std::cin >> row_count >> column_count
                 >> start_row >> start_column >> target_row >> target_column;
        std::vector<std::vector<long long>> cost(
            static_cast<std::size_t>(row_count),
            std::vector<long long>(static_cast<std::size_t>(column_count))
        );
        for(auto& row: cost) for(long long& value: row) std::cin >> value;
        std::cout << weighted_grid_shortest_path(
            cost, start_row, start_column, target_row, target_column
        ) << '\n';
    }
}
