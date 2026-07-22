#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_IRREDUCIBLE_PAIRS_FROM_BOUNDED_INTEGER_DISTANCE_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_IRREDUCIBLE_PAIRS_FROM_BOUNDED_INTEGER_DISTANCE_MATRIX_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../math/fast_matrix_multiply_mod.hpp"

namespace irreducible_pairs_bounded_internal{

inline constexpr std::int64_t counting_modulus =
    2'305'843'009'213'693'951LL;

inline std::size_t checked_square_area(std::size_t side){
    if(side != 0
        && side > std::numeric_limits<std::size_t>::max() / side)[[unlikely]]{
        throw std::length_error(
            "irreducible_pairs_from_bounded_integer_distance_matrix "
            "matrix is too large"
        );
    }
    return side * side;
}

inline std::vector<std::int64_t> distance_indicator_matrix(
    const std::vector<std::vector<long long>>& distances,
    long long selected_distance,
    std::size_t area
){
    const std::size_t vertex_count = distances.size();
    std::vector<std::int64_t> result(area, 0);
    for(std::size_t row = 0; row < vertex_count; ++row){
        for(std::size_t column = 0; column < vertex_count; ++column){
            result[row * vertex_count + column] =
                distances[row][column] == selected_distance ? 1 : 0;
        }
    }
    return result;
}

inline std::uint64_t checked_count(std::int64_t value, std::size_t vertex_count){
    if(value < 0
        || static_cast<std::uint64_t>(value) > vertex_count)[[unlikely]]{
        throw std::logic_error(
            "irreducible_pairs_from_bounded_integer_distance_matrix "
            "internal witness count is invalid"
        );
    }
    return static_cast<std::uint64_t>(value);
}

}  // namespace irreducible_pairs_bounded_internal

inline std::vector<std::pair<int, int>>
irreducible_pairs_from_bounded_integer_distance_matrix(
    const std::vector<std::vector<long long>>& distances,
    long long maximum_distance
){
    using namespace irreducible_pairs_bounded_internal;

    if(maximum_distance < 0
        || maximum_distance == std::numeric_limits<long long>::max())
        [[unlikely]]{
        throw std::invalid_argument(
            "irreducible_pairs_from_bounded_integer_distance_matrix "
            "requires a finite nonnegative distance bound"
        );
    }
    if(distances.size() >
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(
            "irreducible_pairs_from_bounded_integer_distance_matrix "
            "matrix is too large"
        );
    }
    const std::size_t vertex_count = distances.size();
    const std::size_t area = checked_square_area(vertex_count);
    for(const auto& row: distances){
        if(row.size() != vertex_count)[[unlikely]]{
            throw std::invalid_argument(
                "irreducible_pairs_from_bounded_integer_distance_matrix "
                "requires a square matrix"
            );
        }
    }

    std::vector<long long> distinct_distances;
    distinct_distances.reserve(area);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        if(distances[vertex][vertex] != 0)[[unlikely]]{
            throw std::invalid_argument(
                "irreducible_pairs_from_bounded_integer_distance_matrix "
                "requires a zero diagonal"
            );
        }
        for(std::size_t other = 0; other < vertex_count; ++other){
            const long long distance = distances[vertex][other];
            if(distance < 0 || distance > maximum_distance)[[unlikely]]{
                throw std::invalid_argument(
                    "irreducible_pairs_from_bounded_integer_distance_matrix "
                    "distance is outside the finite bound"
                );
            }
            if(distance != distances[other][vertex])[[unlikely]]{
                throw std::invalid_argument(
                    "irreducible_pairs_from_bounded_integer_distance_matrix "
                    "requires a symmetric matrix"
                );
            }
            distinct_distances.push_back(distance);
        }
    }
    std::sort(distinct_distances.begin(), distinct_distances.end());
    distinct_distances.erase(
        std::unique(distinct_distances.begin(), distinct_distances.end()),
        distinct_distances.end()
    );

    std::vector<unsigned char> reducible(area, 0);
    const long long largest_distance =
        distinct_distances.empty() ? 0 : distinct_distances.back();
    for(std::size_t left_index = 0;
        left_index < distinct_distances.size(); ++left_index){
        const long long left_distance = distinct_distances[left_index];
        const auto left_indicator = distance_indicator_matrix(
            distances, left_distance, area
        );
        for(std::size_t right_index = left_index;
            right_index < distinct_distances.size(); ++right_index){
            const long long right_distance = distinct_distances[right_index];
            const __int128_t distance_sum =
                static_cast<__int128_t>(left_distance) + right_distance;
            if(distance_sum > largest_distance) break;

            const auto right_indicator =
                left_index == right_index
                    ? left_indicator
                    : distance_indicator_matrix(
                        distances, right_distance, area
                    );
            const auto witness_counts = math::fast_matrix_multiply_mod(
                left_indicator,
                vertex_count,
                vertex_count,
                right_indicator,
                vertex_count,
                counting_modulus
            );
            for(std::size_t left = 0; left < vertex_count; ++left){
                for(std::size_t right = left + 1;
                    right < vertex_count; ++right){
                    std::uint64_t count = checked_count(
                        witness_counts[left * vertex_count + right],
                        vertex_count
                    );
                    if(left_index != right_index){
                        count += checked_count(
                            witness_counts[right * vertex_count + left],
                            vertex_count
                        );
                        if(count > vertex_count)[[unlikely]]{
                            throw std::logic_error(
                                "irreducible_pairs_from_bounded_integer_"
                                "distance_matrix internal witness overlap"
                            );
                        }
                    }

                    const long long direct = distances[left][right];
                    if(distance_sum < direct && count != 0)[[unlikely]]{
                        throw std::invalid_argument(
                            "irreducible_pairs_from_bounded_integer_"
                            "distance_matrix triangle inequality violation"
                        );
                    }
                    if(distance_sum != direct) continue;

                    const std::uint64_t endpoint_count =
                        left_distance == 0 && right_distance == direct ? 2 : 0;
                    if(count < endpoint_count)[[unlikely]]{
                        throw std::logic_error(
                            "irreducible_pairs_from_bounded_integer_"
                            "distance_matrix lost endpoint witnesses"
                        );
                    }
                    if(count > endpoint_count){
                        reducible[left * vertex_count + right] = 1;
                    }
                }
            }
        }
    }

    std::vector<std::pair<int, int>> result;
    for(std::size_t left = 0; left < vertex_count; ++left){
        for(std::size_t right = left + 1; right < vertex_count; ++right){
            if(reducible[left * vertex_count + right] == 0){
                result.emplace_back(
                    static_cast<int>(left), static_cast<int>(right)
                );
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_IRREDUCIBLE_PAIRS_FROM_BOUNDED_INTEGER_DISTANCE_MATRIX_HPP_INCLUDED
