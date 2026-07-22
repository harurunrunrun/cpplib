#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_IRREDUCIBLE_PAIRS_FROM_SMALL_INTEGER_DISTANCE_MATRIX_NTT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_IRREDUCIBLE_PAIRS_FROM_SMALL_INTEGER_DISTANCE_MATRIX_NTT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../math/linear_algebra/fast_matrix_multiply_mod.hpp"

namespace irreducible_pairs_small_integer_ntt_detail{

inline constexpr std::int64_t modulus = 998244353;
inline constexpr std::int64_t primitive_root = 3;
inline constexpr long long maximum_supported_distance = (1LL << 22) - 1;

inline std::int64_t modular_power(std::int64_t base, std::uint64_t exponent){
    std::int64_t result = 1;
    while(exponent != 0){
        if((exponent & 1U) != 0){
            result = static_cast<std::int64_t>(
                static_cast<__int128_t>(result) * base % modulus
            );
        }
        base = static_cast<std::int64_t>(
            static_cast<__int128_t>(base) * base % modulus
        );
        exponent >>= 1;
    }
    return result;
}

inline std::int64_t modular_product(std::int64_t left, std::int64_t right){
    return static_cast<std::int64_t>(
        static_cast<__int128_t>(left) * right % modulus
    );
}

inline std::size_t checked_area(std::size_t side){
    if(side != 0
        && side > std::numeric_limits<std::size_t>::max() / side)[[unlikely]]{
        throw std::length_error(
            "small-integer NTT irreducible-pair matrix is too large"
        );
    }
    return side * side;
}

inline std::size_t transform_length(long long maximum_distance){
    const std::uint64_t required =
        static_cast<std::uint64_t>(maximum_distance) * 2 + 1;
    std::size_t result = 1;
    while(result < required) result <<= 1;
    return result;
}

}  // namespace irreducible_pairs_small_integer_ntt_detail

inline std::vector<std::pair<int, int>>
irreducible_pairs_from_small_integer_distance_matrix_ntt(
    const std::vector<std::vector<long long>>& distances,
    long long maximum_distance
){
    using namespace irreducible_pairs_small_integer_ntt_detail;

    if(maximum_distance < 0
        || maximum_distance > maximum_supported_distance)[[unlikely]]{
        throw std::invalid_argument(
            "small-integer NTT irreducible pairs require 0 <= bound < 2^22"
        );
    }
    if(distances.size() >= static_cast<std::size_t>(modulus)
        || distances.size()
            > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        [[unlikely]]{
        throw std::length_error(
            "small-integer NTT irreducible-pair matrix has too many vertices"
        );
    }
    const std::size_t vertex_count = distances.size();
    const std::size_t area = checked_area(vertex_count);
    for(const auto& row: distances){
        if(row.size() != vertex_count)[[unlikely]]{
            throw std::invalid_argument(
                "small-integer NTT irreducible pairs require a square matrix"
            );
        }
    }
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        if(distances[vertex][vertex] != 0)[[unlikely]]{
            throw std::invalid_argument(
                "small-integer NTT irreducible pairs require a zero diagonal"
            );
        }
        for(std::size_t other = 0; other < vertex_count; ++other){
            const long long distance = distances[vertex][other];
            if(distance < 0 || distance > maximum_distance)[[unlikely]]{
                throw std::invalid_argument(
                    "small-integer NTT distance is outside the declared bound"
                );
            }
            if(distance != distances[other][vertex])[[unlikely]]{
                throw std::invalid_argument(
                    "small-integer NTT irreducible pairs require symmetry"
                );
            }
        }
    }
    if(vertex_count < 2) return {};

    const std::size_t length = transform_length(maximum_distance);
    const std::int64_t root = modular_power(
        primitive_root,
        static_cast<std::uint64_t>(modulus - 1)
            / static_cast<std::uint64_t>(length)
    );
    const std::int64_t inverse_root =
        modular_power(root, static_cast<std::uint64_t>(modulus - 2));
    const std::int64_t inverse_length = modular_power(
        static_cast<std::int64_t>(length),
        static_cast<std::uint64_t>(modulus - 2)
    );

    std::vector<std::int64_t> evaluated(area, 1);
    std::vector<std::int64_t> evaluation_multiplier(area);
    std::vector<std::int64_t> coefficient_phase(area, 1);
    std::vector<std::int64_t> phase_multiplier(area);
    std::vector<std::int64_t> accumulated(area, 0);
    for(std::size_t left = 0; left < vertex_count; ++left){
        for(std::size_t right = 0; right < vertex_count; ++right){
            const std::size_t index = left * vertex_count + right;
            const auto distance =
                static_cast<std::uint64_t>(distances[left][right]);
            evaluation_multiplier[index] = modular_power(root, distance);
            phase_multiplier[index] = modular_power(inverse_root, distance);
        }
    }

    for(std::size_t frequency = 0; frequency < length; ++frequency){
        const auto product = math::fast_matrix_multiply_mod(
            evaluated,
            vertex_count,
            vertex_count,
            evaluated,
            vertex_count,
            modulus
        );
        for(std::size_t index = 0; index < area; ++index){
            const std::int64_t contribution = modular_product(
                product[index], coefficient_phase[index]
            );
            accumulated[index] += contribution;
            if(accumulated[index] >= modulus) accumulated[index] -= modulus;
        }
        if(frequency + 1 == length) break;
        for(std::size_t index = 0; index < area; ++index){
            evaluated[index] = modular_product(
                evaluated[index], evaluation_multiplier[index]
            );
            coefficient_phase[index] = modular_product(
                coefficient_phase[index], phase_multiplier[index]
            );
        }
    }

    std::vector<std::pair<int, int>> result;
    for(std::size_t left = 0; left < vertex_count; ++left){
        for(std::size_t right = left + 1; right < vertex_count; ++right){
            const std::int64_t witness_count = modular_product(
                accumulated[left * vertex_count + right],
                inverse_length
            );
            if(witness_count < 2
                || witness_count > static_cast<std::int64_t>(vertex_count))
                [[unlikely]]{
                throw std::logic_error(
                    "small-integer NTT witness count is inconsistent"
                );
            }
            if(witness_count == 2){
                result.emplace_back(
                    static_cast<int>(left), static_cast<int>(right)
                );
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_IRREDUCIBLE_PAIRS_FROM_SMALL_INTEGER_DISTANCE_MATRIX_NTT_HPP_INCLUDED
