#pragma once

#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
std::vector<T> subset_convolution(
    const std::vector<T>& left,
    const std::vector<T>& right
){
    if(left.size() != right.size() || left.empty() ||
       !std::has_single_bit(left.size()))[[unlikely]]{
        throw std::invalid_argument(
            "subset_convolution requires equally sized nonempty power-of-two arrays"
        );
    }
    const std::size_t subset_count = left.size();
    const std::size_t bit_count = std::bit_width(subset_count) - 1;
    const std::size_t rank_count = bit_count + 1;
    if(rank_count > std::numeric_limits<std::size_t>::max() / subset_count){
        throw std::length_error("subset_convolution input is too large");
    }

    std::vector<T> transformed_left(subset_count * rank_count);
    std::vector<T> transformed_right(subset_count * rank_count);
    for(std::size_t subset = 0; subset < subset_count; ++subset){
        const std::size_t rank = std::popcount(subset);
        transformed_left[subset * rank_count + rank] = left[subset];
        transformed_right[subset * rank_count + rank] = right[subset];
    }

    for(std::size_t bit = 0; bit < bit_count; ++bit){
        const std::size_t flag = std::size_t{1} << bit;
        for(std::size_t subset = 0; subset < subset_count; ++subset){
            if((subset & flag) == 0) continue;
            const std::size_t source = subset ^ flag;
            const std::size_t source_rank = std::popcount(source);
            const std::size_t target_offset = subset * rank_count;
            const std::size_t source_offset = source * rank_count;
            for(std::size_t rank = 0; rank <= source_rank; ++rank){
                transformed_left[target_offset + rank] +=
                    transformed_left[source_offset + rank];
                transformed_right[target_offset + rank] +=
                    transformed_right[source_offset + rank];
            }
        }
    }

    std::vector<T> product(rank_count);
    for(std::size_t subset = 0; subset < subset_count; ++subset){
        const std::size_t subset_rank = std::popcount(subset);
        const std::size_t rank_limit = std::min(bit_count, subset_rank * 2);
        const std::size_t offset = subset * rank_count;
        std::fill(product.begin(), product.begin() +
            static_cast<std::ptrdiff_t>(rank_limit + 1), T{});
        for(std::size_t rank = 0; rank <= rank_limit; ++rank){
            T value{};
            const std::size_t first_left_rank =
                rank > subset_rank ? rank - subset_rank : 0;
            const std::size_t last_left_rank = std::min(rank, subset_rank);
            for(std::size_t left_rank = first_left_rank;
                left_rank <= last_left_rank; ++left_rank){
                value += transformed_left[offset + left_rank] *
                    transformed_right[offset + rank - left_rank];
            }
            product[rank] = value;
        }
        for(std::size_t rank = 0; rank <= rank_limit; ++rank){
            transformed_left[offset + rank] = product[rank];
        }
    }

    for(std::size_t bit = 0; bit < bit_count; ++bit){
        const std::size_t flag = std::size_t{1} << bit;
        for(std::size_t subset = 0; subset < subset_count; ++subset){
            if((subset & flag) == 0) continue;
            const std::size_t source = subset ^ flag;
            const std::size_t source_rank = std::popcount(source);
            const std::size_t rank_limit =
                std::min(bit_count, source_rank * 2);
            const std::size_t target_offset = subset * rank_count;
            const std::size_t source_offset = source * rank_count;
            for(std::size_t rank = 0; rank <= rank_limit; ++rank){
                transformed_left[target_offset + rank] -=
                    transformed_left[source_offset + rank];
            }
        }
    }

    std::vector<T> result(subset_count);
    for(std::size_t subset = 0; subset < subset_count; ++subset){
        result[subset] = transformed_left[
            subset * rank_count + std::popcount(subset)
        ];
    }
    return result;
}
