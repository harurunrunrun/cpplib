#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_FUNCTIONAL_GRID_CYCLE_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_FUNCTIONAL_GRID_CYCLE_COUNT_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

inline std::size_t functional_grid_cycle_count(
    const std::vector<std::string>& directions
){
    const std::size_t row_count = directions.size();
    if(row_count == 0) return 0;
    const std::size_t column_count = directions.front().size();
    for(const auto& row: directions){
        if(row.size() != column_count)[[unlikely]]{
            throw std::invalid_argument(
                "functional_grid_cycle_count nonrectangular grid"
            );
        }
    }
    if(column_count == 0) return 0;
    if(row_count >
       std::numeric_limits<std::size_t>::max() / column_count)[[unlikely]]{
        throw std::length_error("functional_grid_cycle_count grid too large");
    }

    const std::size_t vertex_count = row_count * column_count;
    std::vector<std::size_t> next(vertex_count);
    for(std::size_t row = 0; row < row_count; ++row){
        for(std::size_t column = 0; column < column_count; ++column){
            const std::size_t vertex = row * column_count + column;
            switch(directions[row][column]){
                case 'N':
                    if(row == 0)[[unlikely]]{
                        throw std::invalid_argument(
                            "functional_grid_cycle_count direction leaves grid"
                        );
                    }
                    next[vertex] = vertex - column_count;
                    break;
                case 'S':
                    if(row + 1 == row_count)[[unlikely]]{
                        throw std::invalid_argument(
                            "functional_grid_cycle_count direction leaves grid"
                        );
                    }
                    next[vertex] = vertex + column_count;
                    break;
                case 'W':
                    if(column == 0)[[unlikely]]{
                        throw std::invalid_argument(
                            "functional_grid_cycle_count direction leaves grid"
                        );
                    }
                    next[vertex] = vertex - 1;
                    break;
                case 'E':
                    if(column + 1 == column_count)[[unlikely]]{
                        throw std::invalid_argument(
                            "functional_grid_cycle_count direction leaves grid"
                        );
                    }
                    next[vertex] = vertex + 1;
                    break;
                default:
                    throw std::invalid_argument(
                        "functional_grid_cycle_count invalid direction"
                    );
            }
        }
    }

    std::vector<unsigned char> state(vertex_count, 0);
    std::size_t cycle_count = 0;
    for(std::size_t start = 0; start < vertex_count; ++start){
        if(state[start] != 0) continue;
        std::size_t vertex = start;
        while(state[vertex] == 0){
            state[vertex] = 1;
            vertex = next[vertex];
        }
        if(state[vertex] == 1) ++cycle_count;
        vertex = start;
        while(state[vertex] == 1){
            state[vertex] = 2;
            vertex = next[vertex];
        }
    }
    return cycle_count;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_FUNCTIONAL_GRID_CYCLE_COUNT_HPP_INCLUDED
