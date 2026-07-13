#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

namespace approximate_randomized_detail {

template<class Matrix>
bool is_rectangular(const Matrix& matrix, std::size_t width) {
    for(const auto& row: matrix){
        if(row.size() != width) return false;
    }
    return true;
}

}  // namespace approximate_randomized_detail

template<class T>
bool freivalds_verify(const std::vector<std::vector<T>>& first,
                      const std::vector<std::vector<T>>& second,
                      const std::vector<std::vector<T>>& product,
                      std::size_t rounds = 20, std::uint64_t seed = 0) {
    if(rounds == 0) throw std::invalid_argument("rounds must be positive");
    const std::size_t rows = first.size();
    const std::size_t inner = rows == 0 ? second.size() : first.front().size();
    const std::size_t columns = second.empty() ? 0 : second.front().size();
    if(!approximate_randomized_detail::is_rectangular(first, inner)
       || second.size() != inner
       || !approximate_randomized_detail::is_rectangular(second, columns)
       || product.size() != rows
       || !approximate_randomized_detail::is_rectangular(product, columns)){
        throw std::invalid_argument("incompatible matrix dimensions");
    }

    std::mt19937_64 engine(seed);
    std::vector<T> random_vector(columns);
    std::vector<T> second_times(inner);
    std::vector<T> left(rows);
    std::vector<T> right(rows);
    for(std::size_t round = 0; round < rounds; ++round){
        for(T& value: random_vector) value = (engine() & 1ULL) == 0ULL ? T{} : T{1};
        std::fill(second_times.begin(), second_times.end(), T{});
        std::fill(left.begin(), left.end(), T{});
        std::fill(right.begin(), right.end(), T{});
        for(std::size_t i = 0; i < inner; ++i){
            for(std::size_t j = 0; j < columns; ++j){
                second_times[i] = second_times[i] + second[i][j] * random_vector[j];
            }
        }
        for(std::size_t i = 0; i < rows; ++i){
            for(std::size_t j = 0; j < inner; ++j){
                left[i] = left[i] + first[i][j] * second_times[j];
            }
            for(std::size_t j = 0; j < columns; ++j){
                right[i] = right[i] + product[i][j] * random_vector[j];
            }
            if(left[i] != right[i]) return false;
        }
    }
    return true;
}

template<class T, std::size_t Rows, std::size_t Inner, std::size_t Columns>
bool freivalds_verify(const std::array<std::array<T, Inner>, Rows>& first,
                      const std::array<std::array<T, Columns>, Inner>& second,
                      const std::array<std::array<T, Columns>, Rows>& product,
                      std::size_t rounds = 20, std::uint64_t seed = 0) {
    if(rounds == 0) throw std::invalid_argument("rounds must be positive");
    std::mt19937_64 engine(seed);
    std::array<T, Columns> random_vector{};
    std::array<T, Inner> second_times{};
    for(std::size_t round = 0; round < rounds; ++round){
        for(T& value: random_vector) value = (engine() & 1ULL) == 0ULL ? T{} : T{1};
        second_times.fill(T{});
        for(std::size_t i = 0; i < Inner; ++i){
            for(std::size_t j = 0; j < Columns; ++j){
                second_times[i] = second_times[i] + second[i][j] * random_vector[j];
            }
        }
        for(std::size_t i = 0; i < Rows; ++i){
            T left{};
            T right{};
            for(std::size_t j = 0; j < Inner; ++j) left = left + first[i][j] * second_times[j];
            for(std::size_t j = 0; j < Columns; ++j) right = right + product[i][j] * random_vector[j];
            if(left != right) return false;
        }
    }
    return true;
}
