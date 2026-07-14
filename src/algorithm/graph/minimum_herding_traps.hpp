#pragma once

#include <stdexcept>
#include <string>
#include <vector>

inline int minimum_herding_traps(const std::vector<std::string>& directions){
    const int row_count = static_cast<int>(directions.size());
    const int column_count = row_count == 0
        ? 0 : static_cast<int>(directions.front().size());
    if(row_count == 0 || column_count == 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: empty grid (minimum_herding_traps)."
        );
    }
    const int vertex_count = row_count * column_count;
    std::vector<int> next(static_cast<std::size_t>(vertex_count));
    for(int row = 0; row < row_count; row++){
        if(static_cast<int>(directions[static_cast<std::size_t>(row)].size()) != column_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch (minimum_herding_traps)."
            );
        }
        for(int column = 0; column < column_count; column++){
            int next_row = row;
            int next_column = column;
            switch(directions[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)]){
                case 'N': --next_row; break;
                case 'S': ++next_row; break;
                case 'W': --next_column; break;
                case 'E': ++next_column; break;
                default:
                    throw std::runtime_error(
                        "library assertion fault: invalid direction "
                        "(minimum_herding_traps)."
                    );
            }
            if(next_row < 0 || row_count <= next_row ||
               next_column < 0 || column_count <= next_column)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: direction leaves grid "
                    "(minimum_herding_traps)."
                );
            }
            const int vertex = row * column_count + column;
            next[static_cast<std::size_t>(vertex)] =
                next_row * column_count + next_column;
        }
    }

    std::vector<unsigned char> state(static_cast<std::size_t>(vertex_count), 0);
    int cycle_count = 0;
    for(int start = 0; start < vertex_count; start++){
        if(state[static_cast<std::size_t>(start)] != 0) continue;
        int vertex = start;
        while(state[static_cast<std::size_t>(vertex)] == 0){
            state[static_cast<std::size_t>(vertex)] = 1;
            vertex = next[static_cast<std::size_t>(vertex)];
        }
        if(state[static_cast<std::size_t>(vertex)] == 1) cycle_count++;
        vertex = start;
        while(state[static_cast<std::size_t>(vertex)] == 1){
            state[static_cast<std::size_t>(vertex)] = 2;
            vertex = next[static_cast<std::size_t>(vertex)];
        }
    }
    return cycle_count;
}
