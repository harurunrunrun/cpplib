#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_GRID_ROW_COLUMN_SELECTION_SCORE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_GRID_ROW_COLUMN_SELECTION_SCORE_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../flow/burn_bury.hpp"

inline long long maximum_grid_row_column_selection_score(
    const std::vector<std::vector<long long>>& grid
){
    const std::size_t height_size = grid.size();
    const std::size_t width_size = grid.empty() ? 0 : grid.front().size();
    for(const auto& row: grid){
        if(row.size() != width_size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-rectangular grid "
                "(maximum_grid_row_column_selection_score)."
            );
        }
    }
    if(height_size > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       width_size > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       height_size + width_size >
           static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size overflow "
            "(maximum_grid_row_column_selection_score)."
        );
    }
    if(height_size == 0 || width_size == 0) return 0;

    __int128 absolute_sum = 0;
    for(const auto& row: grid){
        for(long long value: row){
            absolute_sum += value < 0 ? -static_cast<__int128>(value) : value;
        }
    }
    constexpr long long LIMIT = (std::numeric_limits<long long>::max() - 1) / 2;
    if(absolute_sum > LIMIT)[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: cost overflow "
            "(maximum_grid_row_column_selection_score)."
        );
    }

    const int height = static_cast<int>(height_size);
    const int width = static_cast<int>(width_size);
    const long long infinity = static_cast<long long>(2 * absolute_sum + 1);
    BurnBury<long long> solver(height + width);
    for(int row = 0; row < height; ++row){
        for(int column = 0; column < width; ++column){
            const long long value =
                grid[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)];
            if(value >= 0){
                solver.add_pair_cost(row, height + column, -value, 0, -value, -value);
            }else{
                const long long penalty = -value;
                solver.add_pair_cost(
                    row,
                    height + column,
                    penalty,
                    0,
                    infinity,
                    penalty
                );
            }
        }
    }
    return -solver.solve().cost;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_GRID_ROW_COLUMN_SELECTION_SCORE_HPP_INCLUDED
