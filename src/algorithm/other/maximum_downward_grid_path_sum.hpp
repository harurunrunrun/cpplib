#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_DOWNWARD_GRID_PATH_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_DOWNWARD_GRID_PATH_SUM_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long maximum_downward_grid_path_sum(
    const std::vector<std::vector<int>>& grid
){
    if(grid.empty()) return 0;
    const std::size_t width = grid.front().size();
    if(width == 0) return 0;
    for(const auto& row: grid){
        if(row.size() != width)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: nonrectangular grid "
                "(maximum_downward_grid_path_sum)."
            );
        }
    }
    std::vector<long long> previous(width), current(width);
    for(std::size_t column = 0; column < width; column++){
        previous[column] = grid.front()[column];
    }
    for(std::size_t row = 1; row < grid.size(); row++){
        for(std::size_t column = 0; column < width; column++){
            long long best = previous[column];
            if(column > 0) best = std::max(best, previous[column - 1]);
            if(column + 1 < width) best = std::max(best, previous[column + 1]);
            current[column] = best + grid[row][column];
        }
        previous.swap(current);
    }
    return *std::max_element(previous.begin(), previous.end());
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_DOWNWARD_GRID_PATH_SUM_HPP_INCLUDED
