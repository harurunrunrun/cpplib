#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_WEIGHTED_GRID_SHORTEST_PATH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_WEIGHTED_GRID_SHORTEST_PATH_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>

inline long long weighted_grid_shortest_path(
    const std::vector<std::vector<long long>>& cost,
    int start_row,
    int start_column,
    int target_row,
    int target_column
){
    if(cost.empty())[[unlikely]]{
        throw std::invalid_argument("weighted_grid_shortest_path empty grid");
    }
    if(cost.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("weighted_grid_shortest_path too many rows");
    }
    const int row_count = static_cast<int>(cost.size());
    if(cost.front().empty())[[unlikely]]{
        throw std::invalid_argument("weighted_grid_shortest_path empty row");
    }
    if(cost.front().size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "weighted_grid_shortest_path too many columns"
        );
    }
    const int column_count = static_cast<int>(cost.front().size());
    for(const auto& row: cost){
        if(row.size() != static_cast<std::size_t>(column_count))[[unlikely]]{
            throw std::invalid_argument(
                "weighted_grid_shortest_path ragged grid"
            );
        }
        for(const long long value: row){
            if(value < 0)[[unlikely]]{
                throw std::invalid_argument(
                    "weighted_grid_shortest_path negative cost"
                );
            }
        }
    }
    const auto inside = [row_count, column_count](int row, int column){
        return 0 <= row && row < row_count &&
            0 <= column && column < column_count;
    };
    if(!inside(start_row, start_column))[[unlikely]]{
        throw std::out_of_range(
            "weighted_grid_shortest_path start is out of range"
        );
    }
    if(!inside(target_row, target_column))[[unlikely]]{
        throw std::out_of_range(
            "weighted_grid_shortest_path target is out of range"
        );
    }

    constexpr __int128 infinity = static_cast<__int128>(1) << 126;
    const std::size_t rows = static_cast<std::size_t>(row_count);
    const std::size_t columns = static_cast<std::size_t>(column_count);
    if(rows > std::numeric_limits<std::size_t>::max() / columns)[[unlikely]]{
        throw std::length_error(
            "weighted_grid_shortest_path grid is too large"
        );
    }
    const std::size_t vertex_count = rows * columns;
    std::vector<__int128> distance(vertex_count, infinity);
    using State = std::tuple<__int128, int, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    const std::size_t start =
        static_cast<std::size_t>(start_row) * columns +
        static_cast<std::size_t>(start_column);
    distance[start] =
        cost[static_cast<std::size_t>(start_row)]
            [static_cast<std::size_t>(start_column)];
    queue.emplace(distance[start], start_row, start_column);
    constexpr std::array<int, 4> DR = {-1, 0, 1, 0};
    constexpr std::array<int, 4> DC = {0, 1, 0, -1};
    while(!queue.empty()){
        const auto [current, row, column] = queue.top();
        queue.pop();
        const std::size_t index = static_cast<std::size_t>(row) *
            static_cast<std::size_t>(column_count) +
            static_cast<std::size_t>(column);
        if(distance[index] != current) continue;
        if(row == target_row && column == target_column) break;
        for(int direction = 0; direction < 4; ++direction){
            const int next_row = row + DR[static_cast<std::size_t>(direction)];
            const int next_column =
                column + DC[static_cast<std::size_t>(direction)];
            if(next_row < 0 || row_count <= next_row || next_column < 0 ||
               column_count <= next_column){
                continue;
            }
            const long long add = cost[static_cast<std::size_t>(next_row)]
                [static_cast<std::size_t>(next_column)];
            const __int128 candidate = current + add;
            const std::size_t next_index =
                static_cast<std::size_t>(next_row) *
                    static_cast<std::size_t>(column_count) +
                static_cast<std::size_t>(next_column);
            if(candidate < distance[next_index]){
                distance[next_index] = candidate;
                queue.emplace(candidate, next_row, next_column);
            }
        }
    }
    const std::size_t target = static_cast<std::size_t>(target_row) *
        static_cast<std::size_t>(column_count) +
        static_cast<std::size_t>(target_column);
    if(distance[target] > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "weighted_grid_shortest_path distance overflow"
        );
    }
    return static_cast<long long>(distance[target]);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_WEIGHTED_GRID_SHORTEST_PATH_HPP_INCLUDED
