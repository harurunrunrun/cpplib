#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_NONATTACKING_ROOKS_WITH_BLOCKERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_NONATTACKING_ROOKS_WITH_BLOCKERS_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "maximum_bipartite_pair_count.hpp"

inline int maximum_nonattacking_rooks_with_blockers(
    const std::vector<std::string>& grid,
    char placeable_cell = '.',
    char blocker_cell = '#'
){
    if(placeable_cell == blocker_cell)[[unlikely]]{
        throw std::invalid_argument(
            "maximum_nonattacking_rooks_with_blockers "
            "cell symbols must be distinct"
        );
    }
    if(grid.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "maximum_nonattacking_rooks_with_blockers grid is too large"
        );
    }
    const int row_count = static_cast<int>(grid.size());
    const std::size_t raw_column_count =
        grid.empty() ? 0 : grid.front().size();
    if(raw_column_count > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "maximum_nonattacking_rooks_with_blockers grid is too large"
        );
    }
    const int column_count = static_cast<int>(raw_column_count);
    for(const auto& row: grid){
        if(row.size() != raw_column_count)[[unlikely]]{
            throw std::invalid_argument(
                "maximum_nonattacking_rooks_with_blockers grid is ragged"
            );
        }
    }
    if(row_count != 0 && column_count != 0 &&
       static_cast<std::size_t>(row_count) >
           static_cast<std::size_t>(std::numeric_limits<int>::max()) /
               static_cast<std::size_t>(column_count))[[unlikely]]{
        throw std::length_error(
            "maximum_nonattacking_rooks_with_blockers grid is too large"
        );
    }

    std::vector<std::vector<int>> horizontal_segment(
        static_cast<std::size_t>(row_count),
        std::vector<int>(static_cast<std::size_t>(column_count), -1)
    );
    std::vector<std::vector<int>> vertical_segment = horizontal_segment;

    int horizontal_count = 0;
    for(int row = 0; row < row_count; ++row){
        int segment = -1;
        for(int column = 0; column < column_count; ++column){
            if(grid[static_cast<std::size_t>(row)]
                   [static_cast<std::size_t>(column)] == blocker_cell){
                segment = -1;
                continue;
            }
            if(segment == -1) segment = horizontal_count++;
            horizontal_segment[static_cast<std::size_t>(row)]
                              [static_cast<std::size_t>(column)] = segment;
        }
    }

    int vertical_count = 0;
    for(int column = 0; column < column_count; ++column){
        int segment = -1;
        for(int row = 0; row < row_count; ++row){
            if(grid[static_cast<std::size_t>(row)]
                   [static_cast<std::size_t>(column)] == blocker_cell){
                segment = -1;
                continue;
            }
            if(segment == -1) segment = vertical_count++;
            vertical_segment[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(column)] = segment;
        }
    }

    std::vector<std::pair<int, int>> allowed_placements;
    for(int row = 0; row < row_count; ++row){
        for(int column = 0; column < column_count; ++column){
            if(grid[static_cast<std::size_t>(row)]
                   [static_cast<std::size_t>(column)] != placeable_cell){
                continue;
            }
            allowed_placements.emplace_back(
                horizontal_segment[static_cast<std::size_t>(row)]
                                  [static_cast<std::size_t>(column)],
                vertical_segment[static_cast<std::size_t>(row)]
                                [static_cast<std::size_t>(column)]
            );
        }
    }
    return maximum_bipartite_pair_count(
        horizontal_count, vertical_count, allowed_placements
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_NONATTACKING_ROOKS_WITH_BLOCKERS_HPP_INCLUDED
