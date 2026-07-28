#ifndef CPPLIB_SRC_ALGORITHM_FFT_ONLINE_ONLINE_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_ONLINE_ONLINE_CONVOLUTION_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../transform/convolution.hpp"

template<int MOD>
class OnlineConvolution{
    using mint = Modint<MOD>;

    std::vector<mint> kernel_;
    std::vector<mint> values_;
    std::vector<mint> result_;
    std::vector<std::vector<mint>> kernel_transforms_;
    std::size_t maximum_level_ = 0;

    const std::vector<mint>& kernel_transform(const std::size_t level){
        while(kernel_transforms_.size() <= level){
            const std::size_t current_level = kernel_transforms_.size();
            const std::size_t transform_size =
                std::size_t{1} << (current_level + 1);
            std::vector<mint> transform(transform_size);
            std::copy_n(
                kernel_.begin(),
                std::min(kernel_.size(), transform_size),
                transform.begin()
            );
            number_theoretic_transform(transform);
            kernel_transforms_.push_back(std::move(transform));
        }
        return kernel_transforms_[level];
    }

public:
    explicit OnlineConvolution(std::vector<mint> kernel):
        kernel_(std::move(kernel)){
        while(maximum_level_ + 1 <
              std::numeric_limits<std::size_t>::digits
              && (std::size_t{1} << maximum_level_) < kernel_.size()){
            ++maximum_level_;
        }
    }

    [[nodiscard]] std::size_t size() const{ return values_.size(); }
    [[nodiscard]] const std::vector<mint>& kernel() const{ return kernel_; }

    mint append(const mint value){
        values_.push_back(value);
        const std::size_t count = values_.size();
        const std::size_t level = std::min<std::size_t>(
            maximum_level_, std::countr_zero(count)
        );
        const std::size_t block_size = std::size_t{1} << level;
        const std::size_t required = count - 1 + block_size;
        if(result_.size() < required) result_.resize(required);

        if(level <= 4){
            const std::size_t first = count - block_size;
            for(std::size_t index = first; index < count; ++index){
                const std::size_t end = std::min(
                    required, index + kernel_.size()
                );
                for(std::size_t target = count - 1;
                    target < end; ++target){
                    result_[target] += values_[index]
                        * kernel_[target - index];
                }
            }
        }else{
            const std::size_t transform_size = block_size * 2;
            std::vector<mint> transform(transform_size);
            std::copy_n(
                values_.end() - static_cast<std::ptrdiff_t>(block_size),
                block_size, transform.begin()
            );
            number_theoretic_transform(transform);
            const std::vector<mint>& fixed = kernel_transform(level);
            for(std::size_t index = 0; index < transform_size; ++index){
                transform[index] *= fixed[index];
            }
            number_theoretic_transform(transform, true);
            for(std::size_t offset = 0; offset < block_size; ++offset){
                result_[count - 1 + offset] +=
                    transform[block_size - 1 + offset];
            }
        }
        return result_[count - 1];
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_FFT_ONLINE_ONLINE_CONVOLUTION_HPP_INCLUDED
