#pragma once

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<std::pair<int, int>>
irreducible_pairs_from_distance_matrix(
    const std::vector<std::vector<long long>>& distances
){
    if(distances.size() >
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(
            "irreducible_pairs_from_distance_matrix matrix too large"
        );
    }
    const int vertex_count = static_cast<int>(distances.size());
    for(const auto& row: distances){
        if(row.size() != distances.size())[[unlikely]]{
            throw std::invalid_argument(
                "irreducible_pairs_from_distance_matrix non-square matrix"
            );
        }
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(distances[static_cast<std::size_t>(vertex)]
                    [static_cast<std::size_t>(vertex)] != 0)[[unlikely]]{
            throw std::invalid_argument(
                "irreducible_pairs_from_distance_matrix nonzero diagonal"
            );
        }
        for(int other = 0; other < vertex_count; ++other){
            if(distances[static_cast<std::size_t>(vertex)]
                        [static_cast<std::size_t>(other)] < 0 ||
               distances[static_cast<std::size_t>(vertex)]
                        [static_cast<std::size_t>(other)] !=
               distances[static_cast<std::size_t>(other)]
                        [static_cast<std::size_t>(vertex)])[[unlikely]]{
                throw std::invalid_argument(
                    "irreducible_pairs_from_distance_matrix invalid symmetry"
                );
            }
        }
    }

    std::vector<std::pair<int, int>> result;
    for(int left = 0; left < vertex_count; ++left){
        for(int right = left + 1; right < vertex_count; ++right){
            bool irreducible = true;
            for(int middle = 0; middle < vertex_count; ++middle){
                if(middle == left || middle == right) continue;
                const __int128_t through_middle =
                    static_cast<__int128_t>(
                        distances[static_cast<std::size_t>(left)]
                                 [static_cast<std::size_t>(middle)]
                    ) +
                    distances[static_cast<std::size_t>(middle)]
                             [static_cast<std::size_t>(right)];
                const long long direct =
                    distances[static_cast<std::size_t>(left)]
                             [static_cast<std::size_t>(right)];
                if(through_middle < direct)[[unlikely]]{
                    throw std::invalid_argument(
                        "irreducible_pairs_from_distance_matrix "
                        "triangle inequality violation"
                    );
                }
                if(through_middle == direct) irreducible = false;
            }
            if(irreducible) result.emplace_back(left, right);
        }
    }
    return result;
}
