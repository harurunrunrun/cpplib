#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>

inline std::optional<long long> weighted_grid_escape(
    const std::vector<std::vector<long long>>& cost,
    int target_row,
    int target_column,
    long long budget
){
    if(cost.empty())[[unlikely]]{
        throw std::invalid_argument("weighted_grid_escape empty grid");
    }
    if(cost.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("weighted_grid_escape too many rows");
    }
    const int row_count = static_cast<int>(cost.size());
    if(cost.front().empty())[[unlikely]]{
        throw std::invalid_argument("weighted_grid_escape empty row");
    }
    if(cost.front().size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("weighted_grid_escape too many columns");
    }
    const int column_count = static_cast<int>(cost.front().size());
    for(const auto& row: cost){
        if(row.size() != static_cast<std::size_t>(column_count))[[unlikely]]{
            throw std::invalid_argument("weighted_grid_escape ragged grid");
        }
        for(const long long value: row){
            if(value < 0)[[unlikely]]{
                throw std::invalid_argument("weighted_grid_escape negative cost");
            }
        }
    }
    if(target_row < 0 || row_count <= target_row || target_column < 0 ||
       column_count <= target_column)[[unlikely]]{
        throw std::out_of_range("weighted_grid_escape target is out of range");
    }

    constexpr long long INF = std::numeric_limits<long long>::max();
    const std::size_t rows = static_cast<std::size_t>(row_count);
    const std::size_t columns = static_cast<std::size_t>(column_count);
    if(rows > std::numeric_limits<std::size_t>::max() / columns)[[unlikely]]{
        throw std::length_error("weighted_grid_escape grid is too large");
    }
    const std::size_t vertex_count = rows * columns;
    std::vector<long long> distance(vertex_count, INF);
    using State = std::tuple<long long, int, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    distance[0] = cost[0][0];
    queue.emplace(cost[0][0], 0, 0);
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
            if(current > INF - add)[[unlikely]]{
                throw std::overflow_error("weighted_grid_escape distance overflow");
            }
            const long long candidate = current + add;
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
    if(distance[target] == INF || budget < distance[target]) return std::nullopt;
    return budget - distance[target];
}
