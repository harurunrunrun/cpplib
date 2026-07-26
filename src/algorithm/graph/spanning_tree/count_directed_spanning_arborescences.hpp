#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_COUNT_DIRECTED_SPANNING_ARBORESCENCES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_COUNT_DIRECTED_SPANNING_ARBORESCENCES_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../../structure/modint/modint.hpp"

namespace count_directed_spanning_arborescences_internal{

inline __int128_t exact_determinant(
    std::vector<std::vector<__int128_t>> matrix
){
    const int size = static_cast<int>(matrix.size());
    if(size == 0) return 1;

    __int128_t previous_pivot = 1;
    int sign = 1;
    for(int column = 0; column + 1 < size; column++){
        int pivot = column;
        while(pivot < size &&
              matrix[static_cast<std::size_t>(pivot)]
                    [static_cast<std::size_t>(column)] == 0){
            pivot++;
        }
        if(pivot == size) return 0;
        if(pivot != column){
            std::swap(
                matrix[static_cast<std::size_t>(pivot)],
                matrix[static_cast<std::size_t>(column)]
            );
            sign = -sign;
        }

        const __int128_t current_pivot =
            matrix[static_cast<std::size_t>(column)]
                  [static_cast<std::size_t>(column)];
        for(int row = column + 1; row < size; row++){
            for(int next = column + 1; next < size; next++){
                const __int128_t numerator =
                    matrix[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(next)] * current_pivot -
                    matrix[static_cast<std::size_t>(row)]
                          [static_cast<std::size_t>(column)] *
                    matrix[static_cast<std::size_t>(column)]
                          [static_cast<std::size_t>(next)];
                matrix[static_cast<std::size_t>(row)]
                      [static_cast<std::size_t>(next)] =
                    numerator / previous_pivot;
            }
            matrix[static_cast<std::size_t>(row)]
                  [static_cast<std::size_t>(column)] = 0;
        }
        previous_pivot = current_pivot;
    }
    return sign * matrix.back().back();
}

template<class T>
void validate_square_matrix(const std::vector<std::vector<T>>& matrix){
    if(matrix.empty())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: empty matrix "
            "(count_directed_spanning_arborescences)."
        );
    }
    const std::size_t size = matrix.size();
    for(const auto& row: matrix){
        if(row.size() != size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-square matrix "
                "(count_directed_spanning_arborescences)."
            );
        }
    }
}

template<class Field>
Field field_determinant(std::vector<std::vector<Field>> matrix){
    const std::size_t size = matrix.size();
    Field determinant = Field(1);
    for(std::size_t column = 0; column < size; ++column){
        std::size_t pivot = column;
        while(pivot < size && matrix[pivot][column] == Field{}) ++pivot;
        if(pivot == size) return Field{};
        if(pivot != column){
            std::swap(matrix[pivot], matrix[column]);
            determinant = -determinant;
        }
        const Field pivot_value = matrix[column][column];
        determinant *= pivot_value;
        const Field inverse_pivot = Field(1) / pivot_value;
        for(std::size_t row = column + 1; row < size; ++row){
            if(matrix[row][column] == Field{}) continue;
            const Field factor = matrix[row][column] * inverse_pivot;
            matrix[row][column] = Field{};
            for(std::size_t next = column + 1; next < size; ++next){
                matrix[row][next] -= factor * matrix[column][next];
            }
        }
    }
    return determinant;
}

constexpr bool is_prime(int value){
    if(value < 2) return false;
    if(value % 2 == 0) return value == 2;
    for(long long divisor = 3;
        divisor * divisor <= value;
        divisor += 2){
        if(value % divisor == 0) return false;
    }
    return true;
}

template<int MOD, class Integer>
Modint<MOD> modular_weight(Integer value){
    static_assert(std::is_integral_v<Integer>);
    static_assert(sizeof(Integer) <= sizeof(std::uintmax_t));
    if constexpr(std::is_signed_v<Integer>){
        const std::intmax_t reduced =
            static_cast<std::intmax_t>(value) % static_cast<std::intmax_t>(MOD);
        return Modint<MOD>(static_cast<long long>(reduced));
    }else{
        const std::uintmax_t reduced = static_cast<std::uintmax_t>(value)
            % static_cast<std::uintmax_t>(MOD);
        return Modint<MOD>(static_cast<long long>(reduced));
    }
}

} // namespace count_directed_spanning_arborescences_internal

template<class Field>
Field count_directed_spanning_arborescences(
    const std::vector<std::vector<Field>>& adjacency
){
    static_assert(
        !std::is_integral_v<Field>,
        "integral weights require count_directed_spanning_arborescences_mod"
    );
    count_directed_spanning_arborescences_internal::validate_square_matrix(
        adjacency
    );
    const std::size_t vertex_count = adjacency.size();
    std::vector<std::vector<Field>> laplacian(
        vertex_count, std::vector<Field>(vertex_count, Field{})
    );
    for(std::size_t from = 0; from < vertex_count; ++from){
        for(std::size_t to = 0; to < vertex_count; ++to){
            if(from == to) continue;
            const Field& weight = adjacency[from][to];
            laplacian[to][to] += weight;
            laplacian[to][from] -= weight;
        }
    }
    // det(L + 1 e_0^T) is the sum of all rooted arborescence weights.
    // This rank-one update avoids division by the vertex count.
    for(std::size_t row = 0; row < vertex_count; ++row){
        laplacian[row][0] += Field(1);
    }
    return count_directed_spanning_arborescences_internal::field_determinant(
        std::move(laplacian)
    );
}

template<int MOD, class Integer>
int count_directed_spanning_arborescences_mod(
    const std::vector<std::vector<Integer>>& adjacency
){
    static_assert(std::is_integral_v<Integer>);
    static_assert(
        count_directed_spanning_arborescences_internal::is_prime(MOD),
        "MOD must be prime"
    );
    std::vector<std::vector<Modint<MOD>>> reduced;
    reduced.reserve(adjacency.size());
    for(const auto& row: adjacency){
        std::vector<Modint<MOD>> reduced_row;
        reduced_row.reserve(row.size());
        for(const Integer weight: row){
            reduced_row.push_back(
                count_directed_spanning_arborescences_internal::
                    modular_weight<MOD>(weight)
            );
        }
        reduced.push_back(std::move(reduced_row));
    }
    return count_directed_spanning_arborescences(reduced).val();
}

inline long long count_directed_spanning_arborescences(
    const std::vector<std::string>& adjacency
){
    const int vertex_count = static_cast<int>(adjacency.size());
    if(vertex_count <= 0 || 8 < vertex_count)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(count_directed_spanning_arborescences)."
        );
    }
    for(const auto& row: adjacency){
        if(static_cast<int>(row.size()) != vertex_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-square matrix "
                "(count_directed_spanning_arborescences)."
            );
        }
        for(const char edge: row){
            if(edge != '0' && edge != '1')[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: non-binary matrix "
                    "(count_directed_spanning_arborescences)."
                );
            }
        }
    }

    std::vector<std::vector<__int128_t>> laplacian(
        static_cast<std::size_t>(vertex_count),
        std::vector<__int128_t>(static_cast<std::size_t>(vertex_count), 0)
    );
    for(int from = 0; from < vertex_count; from++){
        for(int to = 0; to < vertex_count; to++){
            if(from == to ||
               adjacency[static_cast<std::size_t>(from)]
                        [static_cast<std::size_t>(to)] == '0'){
                continue;
            }
            laplacian[static_cast<std::size_t>(to)]
                     [static_cast<std::size_t>(to)]++;
            laplacian[static_cast<std::size_t>(to)]
                     [static_cast<std::size_t>(from)]--;
        }
    }

    // The single-column rank-one update returns the sum for every root
    // directly and does not require division by the vertex count.
    for(auto& row: laplacian) row[0]++;
    const __int128_t answer =
        count_directed_spanning_arborescences_internal::exact_determinant(
            std::move(laplacian)
        );
    if(answer < 0 || answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "count_directed_spanning_arborescences overflow"
        );
    }
    return static_cast<long long>(answer);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_COUNT_DIRECTED_SPANNING_ARBORESCENCES_HPP_INCLUDED
