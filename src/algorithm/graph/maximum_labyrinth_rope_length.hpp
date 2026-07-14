#pragma once

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

inline int maximum_labyrinth_rope_length(const std::vector<std::string>& labyrinth){
    const int row_count = static_cast<int>(labyrinth.size());
    const int column_count = row_count == 0
        ? 0 : static_cast<int>(labyrinth.front().size());
    for(const auto& row: labyrinth){
        if(static_cast<int>(row.size()) != column_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch "
                "(maximum_labyrinth_rope_length)."
            );
        }
        for(char cell: row){
            if(cell != '.' && cell != '#')[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid cell "
                    "(maximum_labyrinth_rope_length)."
                );
            }
        }
    }
    if(row_count == 0 || column_count == 0) return 0;
    const int vertex_count = row_count * column_count;
    std::vector<unsigned char> component_seen(static_cast<std::size_t>(vertex_count), 0);
    const int delta_row[] = {-1, 1, 0, 0};
    const int delta_column[] = {0, 0, -1, 1};
    auto sweep = [&](int start, bool mark_component){
        std::vector<int> distance(static_cast<std::size_t>(vertex_count), -1);
        std::queue<int> queue;
        distance[static_cast<std::size_t>(start)] = 0;
        if(mark_component) component_seen[static_cast<std::size_t>(start)] = 1;
        queue.push(start);
        int farthest = start;
        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            if(distance[static_cast<std::size_t>(vertex)] >
               distance[static_cast<std::size_t>(farthest)]) farthest = vertex;
            const int row = vertex / column_count;
            const int column = vertex % column_count;
            for(int direction = 0; direction < 4; direction++){
                const int next_row = row + delta_row[direction];
                const int next_column = column + delta_column[direction];
                if(next_row < 0 || row_count <= next_row ||
                   next_column < 0 || column_count <= next_column ||
                   labyrinth[static_cast<std::size_t>(next_row)][static_cast<std::size_t>(next_column)] == '#') continue;
                const int next = next_row * column_count + next_column;
                if(distance[static_cast<std::size_t>(next)] != -1) continue;
                distance[static_cast<std::size_t>(next)] =
                    distance[static_cast<std::size_t>(vertex)] + 1;
                if(mark_component) component_seen[static_cast<std::size_t>(next)] = 1;
                queue.push(next);
            }
        }
        return std::pair<int, int>{
            farthest, distance[static_cast<std::size_t>(farthest)]
        };
    };
    int answer = 0;
    for(int vertex = 0; vertex < vertex_count; vertex++){
        const int row = vertex / column_count;
        const int column = vertex % column_count;
        if(labyrinth[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] == '#' ||
           component_seen[static_cast<std::size_t>(vertex)]) continue;
        const int endpoint = sweep(vertex, true).first;
        answer = std::max(answer, sweep(endpoint, false).second);
    }
    return answer;
}
