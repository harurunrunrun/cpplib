#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "zero_one_bfs.hpp"

inline int minimum_grid_character_changes(
    const std::vector<std::string>& grid
){
    if(grid.empty() || grid.front().empty())[[unlikely]]{
        throw std::invalid_argument("minimum_grid_character_changes empty grid");
    }
    const std::size_t row_count = grid.size();
    const std::size_t column_count = grid.front().size();
    for(const auto& row: grid){
        if(row.size() != column_count)[[unlikely]]{
            throw std::invalid_argument(
                "minimum_grid_character_changes ragged grid"
            );
        }
    }
    if(row_count > static_cast<std::size_t>(std::numeric_limits<int>::max()) /
        column_count)[[unlikely]]{
        throw std::length_error("minimum_grid_character_changes grid too large");
    }
    const int rows = static_cast<int>(row_count);
    const int columns = static_cast<int>(column_count);
    const int vertex_count = rows * columns;
    const auto result = zero_one_bfs_implicit(
        vertex_count,
        0,
        [&](int vertex, const auto& relax){
            const int row = vertex / columns;
            const int column = vertex % columns;
            constexpr int DR[] = {-1, 0, 1, 0};
            constexpr int DC[] = {0, 1, 0, -1};
            for(int direction = 0; direction < 4; ++direction){
                const int next_row = row + DR[direction];
                const int next_column = column + DC[direction];
                if(next_row < 0 || rows <= next_row || next_column < 0 ||
                   columns <= next_column){
                    continue;
                }
                const int next_vertex = next_row * columns + next_column;
                const int cost = grid[static_cast<std::size_t>(row)]
                                     [static_cast<std::size_t>(column)] !=
                    grid[static_cast<std::size_t>(next_row)]
                        [static_cast<std::size_t>(next_column)];
                relax(next_vertex, cost);
            }
        }
    );
    return result.dist.back();
}
