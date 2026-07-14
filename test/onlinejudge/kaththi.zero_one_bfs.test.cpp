// competitive-verifier: PROBLEM https://www.spoj.com/problems/KATHTHI/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/zero_one_bfs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int row_count, column_count;
        std::cin >> row_count >> column_count;
        std::vector<std::string> grid(static_cast<std::size_t>(row_count));
        for(auto& row: grid) std::cin >> row;

        const int vertex_count = row_count * column_count;
        const auto result = zero_one_bfs_implicit(
            vertex_count,
            0,
            [&](int vertex, const auto& relax){
                const int row = vertex / column_count;
                const int column = vertex % column_count;
                constexpr int delta_row[] = {-1, 0, 1, 0};
                constexpr int delta_column[] = {0, 1, 0, -1};
                for(int direction = 0; direction < 4; ++direction){
                    const int next_row = row + delta_row[direction];
                    const int next_column = column + delta_column[direction];
                    if(
                        next_row < 0 || row_count <= next_row ||
                        next_column < 0 || column_count <= next_column
                    ){
                        continue;
                    }
                    const int next_vertex = next_row * column_count + next_column;
                    const int cost = grid[static_cast<std::size_t>(row)]
                                         [static_cast<std::size_t>(column)] !=
                                     grid[static_cast<std::size_t>(next_row)]
                                         [static_cast<std::size_t>(next_column)];
                    relax(next_vertex, cost);
                }
            }
        );
        std::cout << result.dist[static_cast<std::size_t>(vertex_count - 1)] << '\n';
    }
}
