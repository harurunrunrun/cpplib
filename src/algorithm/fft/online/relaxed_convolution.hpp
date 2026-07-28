#ifndef CPPLIB_SRC_ALGORITHM_FFT_ONLINE_RELAXED_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_ONLINE_RELAXED_CONVOLUTION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../transform/convolution.hpp"

template<int MOD>
class RelaxedConvolution{
    using mint = Modint<MOD>;

    std::size_t capacity_ = 0;
    std::size_t processed_ = 0;
    std::vector<mint> left_;
    std::vector<mint> right_;
    std::vector<mint> result_;
    std::vector<std::vector<mint>> left_prefix_transforms_;
    std::vector<std::vector<mint>> right_prefix_transforms_;

    void prepare_prefix_transform(
        const std::size_t level,
        const std::size_t block_size
    ){
        if(left_prefix_transforms_.size() <= level){
            left_prefix_transforms_.resize(level + 1);
            right_prefix_transforms_.resize(level + 1);
        }
        if(!left_prefix_transforms_[level].empty()) return;
        const std::size_t transform_size = block_size * 2;
        std::vector<mint> left(transform_size);
        std::vector<mint> right(transform_size);
        std::copy_n(left_.begin(), transform_size, left.begin());
        std::copy_n(right_.begin(), transform_size, right.begin());
        number_theoretic_transform(left);
        number_theoretic_transform(right);
        left_prefix_transforms_[level] = std::move(left);
        right_prefix_transforms_[level] = std::move(right);
    }

public:
    explicit RelaxedConvolution(const std::size_t capacity):
        capacity_(capacity),
        left_(capacity),
        right_(capacity),
        result_(capacity){}

    [[nodiscard]] std::size_t size() const{ return processed_; }
    [[nodiscard]] std::size_t capacity() const{ return capacity_; }

    mint append(const mint left_value, const mint right_value){
        if(processed_ == capacity_)[[unlikely]]{
            throw std::length_error("relaxed convolution capacity exceeded");
        }
        const std::size_t index = processed_;
        left_[index] = left_value;
        right_[index] = right_value;
        result_[index] += left_value * right_[0];
        if(index != 0) result_[index] += right_value * left_[0];
        ++processed_;
        if(processed_ == capacity_) return result_[index];

        for(std::size_t block_size = 1, level = 0;
            block_size <= processed_; block_size *= 2, ++level){
            if(block_size >
               std::numeric_limits<std::size_t>::max() / 2)[[unlikely]]{
                break;
            }
            const std::size_t period = block_size * 2;
            if(processed_ % period != block_size) continue;
            std::vector<mint> left(period);
            std::vector<mint> right(period);
            if(processed_ == block_size){
                std::copy_n(left_.begin(), block_size, left.begin());
                std::copy_n(right_.begin(), block_size, right.begin());
                number_theoretic_transform(left);
                number_theoretic_transform(right);
                for(std::size_t position = 0; position < period; ++position){
                    left[position] *= right[position];
                }
            }else{
                prepare_prefix_transform(level, block_size);
                std::copy_n(
                    left_.begin() + static_cast<std::ptrdiff_t>(
                        processed_ - block_size
                    ),
                    block_size, left.begin()
                );
                std::copy_n(
                    right_.begin() + static_cast<std::ptrdiff_t>(
                        processed_ - block_size
                    ),
                    block_size, right.begin()
                );
                number_theoretic_transform(left);
                number_theoretic_transform(right);
                const auto& left_prefix = left_prefix_transforms_[level];
                const auto& right_prefix = right_prefix_transforms_[level];
                for(std::size_t position = 0; position < period; ++position){
                    left[position] = left[position] * right_prefix[position]
                        + right[position] * left_prefix[position];
                }
            }
            number_theoretic_transform(left, true);
            const std::size_t end = std::min(
                capacity_, processed_ + block_size
            );
            for(std::size_t target = processed_; target < end; ++target){
                result_[target] += left[
                    block_size + target - processed_
                ];
            }
        }
        return result_[index];
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_FFT_ONLINE_RELAXED_CONVOLUTION_HPP_INCLUDED
