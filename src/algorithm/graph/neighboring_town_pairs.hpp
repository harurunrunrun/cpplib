#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<std::pair<int, int>> neighboring_town_pairs(
    const std::vector<std::vector<long long>>& distances
){
    const int town_count = static_cast<int>(distances.size());
    for(const auto& row: distances){
        if(row.size() != static_cast<std::size_t>(town_count))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch (neighboring_town_pairs)."
            );
        }
    }
    for(int town = 0; town < town_count; town++){
        if(distances[static_cast<std::size_t>(town)][static_cast<std::size_t>(town)] != 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid distance (neighboring_town_pairs)."
            );
        }
        for(int other = 0; other < town_count; other++){
            if(distances[static_cast<std::size_t>(town)][static_cast<std::size_t>(other)] < 0 ||
               distances[static_cast<std::size_t>(town)][static_cast<std::size_t>(other)] !=
               distances[static_cast<std::size_t>(other)][static_cast<std::size_t>(town)])[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid distance (neighboring_town_pairs)."
                );
            }
        }
    }

    std::vector<std::pair<int, int>> result;
    for(int left = 0; left < town_count; left++){
        for(int right = left + 1; right < town_count; right++){
            bool neighboring = true;
            for(int middle = 0; middle < town_count; middle++){
                if(middle == left || middle == right) continue;
                const __int128 through_middle =
                    static_cast<__int128>(
                        distances[static_cast<std::size_t>(left)][static_cast<std::size_t>(middle)]
                    ) + distances[static_cast<std::size_t>(middle)][static_cast<std::size_t>(right)];
                if(through_middle == distances[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)]){
                    neighboring = false;
                    break;
                }
            }
            if(neighboring) result.emplace_back(left, right);
        }
    }
    return result;
}
