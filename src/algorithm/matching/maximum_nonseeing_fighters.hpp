#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "maximum_bipartite_pair_count.hpp"

inline int maximum_nonseeing_fighters(const std::vector<std::string>& rooms){
    const int row_count = static_cast<int>(rooms.size());
    const int column_count = row_count == 0
        ? 0 : static_cast<int>(rooms.front().size());
    for(const auto& row: rooms){
        if(static_cast<int>(row.size()) != column_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch "
                "(maximum_nonseeing_fighters)."
            );
        }
        for(char room: row){
            if(room != 'H' && room != 'A' && room != 'D')[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid room "
                    "(maximum_nonseeing_fighters)."
                );
            }
        }
    }

    std::vector<std::vector<int>> horizontal(
        static_cast<std::size_t>(row_count),
        std::vector<int>(static_cast<std::size_t>(column_count), -1)
    );
    std::vector<std::vector<int>> vertical = horizontal;
    int horizontal_count = 0;
    for(int row = 0; row < row_count; row++){
        int segment = -1;
        for(int column = 0; column < column_count; column++){
            if(rooms[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] == 'A'){
                segment = -1;
                continue;
            }
            if(segment == -1) segment = horizontal_count++;
            horizontal[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] = segment;
        }
    }
    int vertical_count = 0;
    for(int column = 0; column < column_count; column++){
        int segment = -1;
        for(int row = 0; row < row_count; row++){
            if(rooms[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] == 'A'){
                segment = -1;
                continue;
            }
            if(segment == -1) segment = vertical_count++;
            vertical[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] = segment;
        }
    }
    std::vector<std::pair<int, int>> vacant_rooms;
    for(int row = 0; row < row_count; row++){
        for(int column = 0; column < column_count; column++){
            if(rooms[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] != 'H') continue;
            vacant_rooms.emplace_back(
                horizontal[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)],
                vertical[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)]
            );
        }
    }
    return maximum_bipartite_pair_count(
        horizontal_count, vertical_count, vacant_rooms
    );
}
