#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_GRID_FOREST_DIAMETER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_GRID_FOREST_DIAMETER_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

inline int grid_forest_diameter(
    const std::vector<std::string>& grid,
    char traversable_cell = '.',
    char blocked_cell = '#'
){
    if(traversable_cell == blocked_cell)[[unlikely]]{
        throw std::invalid_argument(
            "grid_forest_diameter cell symbols must be distinct"
        );
    }
    if(grid.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("grid_forest_diameter grid is too large");
    }
    const int row_count = static_cast<int>(grid.size());
    const std::size_t raw_column_count =
        grid.empty() ? 0 : grid.front().size();
    if(raw_column_count > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("grid_forest_diameter grid is too large");
    }
    const int column_count = static_cast<int>(raw_column_count);
    for(const auto& row: grid){
        if(row.size() != raw_column_count)[[unlikely]]{
            throw std::invalid_argument(
                "grid_forest_diameter grid is ragged"
            );
        }
        for(const char cell: row){
            if(cell != traversable_cell && cell != blocked_cell)[[unlikely]]{
                throw std::invalid_argument(
                    "grid_forest_diameter contains an unknown cell"
                );
            }
        }
    }
    if(row_count == 0 || column_count == 0) return 0;
    if(static_cast<std::size_t>(row_count) >
       static_cast<std::size_t>(std::numeric_limits<int>::max()) /
           static_cast<std::size_t>(column_count))[[unlikely]]{
        throw std::length_error("grid_forest_diameter grid is too large");
    }

    const int vertex_count = row_count * column_count;
    std::vector<unsigned char> component_seen(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<int> distance(static_cast<std::size_t>(vertex_count), -1);
    constexpr int DELTA_ROW[4] = {-1, 1, 0, 0};
    constexpr int DELTA_COLUMN[4] = {0, 0, -1, 1};

    struct SweepResult{
        int farthest;
        int farthest_distance;
        int reached_count;
        long long directed_edge_count;
    };
    const auto sweep = [&](int start, bool mark_component){
        std::queue<int> queue;
        std::vector<int> reached;
        distance[static_cast<std::size_t>(start)] = 0;
        if(mark_component){
            component_seen[static_cast<std::size_t>(start)] = 1;
        }
        queue.push(start);
        reached.push_back(start);
        int farthest = start;
        long long directed_edge_count = 0;

        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            if(distance[static_cast<std::size_t>(vertex)] >
               distance[static_cast<std::size_t>(farthest)]){
                farthest = vertex;
            }
            const int row = vertex / column_count;
            const int column = vertex % column_count;
            for(int direction = 0; direction < 4; ++direction){
                const int next_row =
                    row + DELTA_ROW[static_cast<std::size_t>(direction)];
                const int next_column =
                    column + DELTA_COLUMN[static_cast<std::size_t>(direction)];
                if(next_row < 0 || row_count <= next_row ||
                   next_column < 0 || column_count <= next_column){
                    continue;
                }
                if(grid[static_cast<std::size_t>(next_row)]
                       [static_cast<std::size_t>(next_column)] ==
                   blocked_cell){
                    continue;
                }
                ++directed_edge_count;
                const int next = next_row * column_count + next_column;
                if(distance[static_cast<std::size_t>(next)] != -1) continue;
                distance[static_cast<std::size_t>(next)] =
                    distance[static_cast<std::size_t>(vertex)] + 1;
                if(mark_component){
                    component_seen[static_cast<std::size_t>(next)] = 1;
                }
                queue.push(next);
                reached.push_back(next);
            }
        }

        const SweepResult result{
            farthest,
            distance[static_cast<std::size_t>(farthest)],
            static_cast<int>(reached.size()),
            directed_edge_count,
        };
        for(const int vertex: reached){
            distance[static_cast<std::size_t>(vertex)] = -1;
        }
        return result;
    };

    int answer = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const int row = vertex / column_count;
        const int column = vertex % column_count;
        if(grid[static_cast<std::size_t>(row)]
               [static_cast<std::size_t>(column)] == blocked_cell ||
           component_seen[static_cast<std::size_t>(vertex)]){
            continue;
        }
        const SweepResult first = sweep(vertex, true);
        if(first.directed_edge_count !=
           2LL * (static_cast<long long>(first.reached_count) - 1))
            [[unlikely]]{
            throw std::invalid_argument(
                "grid_forest_diameter traversable cells must form a forest"
            );
        }
        answer = std::max(
            answer, sweep(first.farthest, false).farthest_distance
        );
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_GRID_FOREST_DIAMETER_HPP_INCLUDED
