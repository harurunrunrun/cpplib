#ifndef CPPLIB_SRC_ALGORITHM_FFT_MIN_PLUS_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_MIN_PLUS_CONVOLUTION_HPP_INCLUDED

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace min_plus_convolution_internal{

template<std::signed_integral T>
struct ExactSum{
    using U = std::make_unsigned_t<T>;

    int sign = 0;
    bool high = false;
    U low = 0;

    static U magnitude(T value){
        const U raw = static_cast<U>(value);
        return value < 0 ? U(0) - raw : raw;
    }

    ExactSum(T left, T right): ExactSum(left, right, false){}

    ExactSum(T left, T right, bool subtract_right){
        const int left_sign = (T(0) < left) - (left < T(0));
        int right_sign = (T(0) < right) - (right < T(0));
        if(subtract_right) right_sign = -right_sign;
        const U left_magnitude = magnitude(left);
        const U right_magnitude = magnitude(right);
        if(left_sign == 0){
            sign = right_sign;
            low = right_magnitude;
        }else if(right_sign == 0){
            sign = left_sign;
            low = left_magnitude;
        }else if(left_sign == right_sign){
            sign = left_sign;
            low = left_magnitude + right_magnitude;
            high = low < left_magnitude;
        }else if(left_magnitude == right_magnitude){
            sign = 0;
        }else if(left_magnitude < right_magnitude){
            sign = right_sign;
            low = right_magnitude - left_magnitude;
        }else{
            sign = left_sign;
            low = left_magnitude - right_magnitude;
        }
    }

    static ExactSum difference(T left, T right){
        return ExactSum(left, right, true);
    }

    friend bool operator<(const ExactSum& left, const ExactSum& right){
        if(left.sign != right.sign) return left.sign < right.sign;
        if(left.sign == 0) return false;
        int magnitude_order = 0;
        if(left.high != right.high) magnitude_order = left.high ? 1 : -1;
        else if(left.low != right.low) magnitude_order = left.low < right.low ? -1 : 1;
        return left.sign > 0 ? magnitude_order < 0 : magnitude_order > 0;
    }

    T checked_value() const{
        if(sign == 0) return T(0);
        if(high) throw std::overflow_error("min-plus convolution result is outside the value type");
        const U positive_limit = static_cast<U>(std::numeric_limits<T>::max());
        if(sign > 0){
            if(positive_limit < low)
                throw std::overflow_error("min-plus convolution result is outside the value type");
            return static_cast<T>(low);
        }
        const U negative_limit = positive_limit + U(1);
        if(negative_limit < low)
            throw std::overflow_error("min-plus convolution result is outside the value type");
        if(low == negative_limit) return std::numeric_limits<T>::min();
        return -static_cast<T>(low);
    }
};

template<std::signed_integral T>
bool convex(const std::vector<T>& values){
    for(std::size_t i = 1; i + 1 < values.size(); ++i){
        const ExactSum<T> outer(values[i - 1], values[i + 1]);
        const ExactSum<T> middle(values[i], values[i]);
        if(outer < middle) return false;
    }
    return true;
}

template<std::signed_integral T>
class MonotoneMinima{
    using index_type = std::ptrdiff_t;

    const std::vector<T>& convex_values;
    const std::vector<T>& arbitrary_values;
    index_type convex_size;
    index_type arbitrary_size;
    std::vector<T> result;

    ExactSum<T> evaluate(index_type row, index_type arbitrary_index) const{
        const index_type convex_index = row - arbitrary_index;
        return ExactSum<T>(
            convex_values[static_cast<std::size_t>(convex_index)],
            arbitrary_values[static_cast<std::size_t>(arbitrary_index)]
        );
    }

    void solve(index_type row_left, index_type row_right,
               index_type candidate_left, index_type candidate_right){
        if(row_right <= row_left) return;
        const index_type row = row_left + (row_right - row_left) / 2;
        const index_type valid_left = std::max<index_type>(0, row - (convex_size - 1));
        const index_type valid_right = std::min(arbitrary_size - 1, row);
        const index_type scan_left = std::max(candidate_left, valid_left);
        const index_type scan_right = std::min(candidate_right, valid_right);

        index_type best_index = scan_left;
        ExactSum<T> best = evaluate(row, best_index);
        for(index_type candidate = scan_left + 1; candidate <= scan_right; ++candidate){
            const ExactSum<T> value = evaluate(row, candidate);
            if(value < best){
                best = value;
                best_index = candidate;
            }
        }
        result[static_cast<std::size_t>(row)] = best.checked_value();
        solve(row_left, row, candidate_left, best_index);
        solve(row + 1, row_right, best_index, candidate_right);
    }

public:
    MonotoneMinima(const std::vector<T>& convex_sequence,
                   const std::vector<T>& arbitrary_sequence):
        convex_values(convex_sequence),
        arbitrary_values(arbitrary_sequence),
        convex_size(static_cast<index_type>(convex_sequence.size())),
        arbitrary_size(static_cast<index_type>(arbitrary_sequence.size())),
        result(convex_sequence.size() + arbitrary_sequence.size() - 1){}

    std::vector<T> run(){
        solve(0, static_cast<index_type>(result.size()), 0, arbitrary_size - 1);
        return result;
    }
};

template<std::signed_integral T>
std::vector<T> convolution(const std::vector<T>& convex_values,
                           const std::vector<T>& arbitrary_values){
    if(convex_values.empty() || arbitrary_values.empty()) return {};
    constexpr std::size_t index_limit =
        static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max());
    if(index_limit < convex_values.size() || index_limit < arbitrary_values.size() ||
       index_limit - (convex_values.size() - 1) < arbitrary_values.size()){
        throw std::length_error("min-plus convolution input is too large");
    }
    return MonotoneMinima<T>(convex_values, arbitrary_values).run();
}

template<std::signed_integral T>
std::vector<T> convex_convolution(const std::vector<T>& left,
                                  const std::vector<T>& right){
    if(left.empty() || right.empty()) return {};
    constexpr std::size_t index_limit =
        static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max());
    if(index_limit < left.size() || index_limit < right.size() ||
       index_limit - (left.size() - 1) < right.size()){
        throw std::length_error("min-plus convolution input is too large");
    }

    std::vector<T> result;
    result.reserve(left.size() + right.size() - 1);
    std::size_t left_index = 0, right_index = 0;
    result.push_back(ExactSum<T>(left[0], right[0]).checked_value());
    while(left_index + 1 < left.size() || right_index + 1 < right.size()){
        bool take_left = right_index + 1 == right.size();
        if(left_index + 1 < left.size() && right_index + 1 < right.size()){
            const ExactSum<T> left_slope =
                ExactSum<T>::difference(left[left_index + 1], left[left_index]);
            const ExactSum<T> right_slope =
                ExactSum<T>::difference(right[right_index + 1], right[right_index]);
            take_left = !(right_slope < left_slope);
        }
        if(take_left) ++left_index;
        else ++right_index;
        result.push_back(
            ExactSum<T>(left[left_index], right[right_index]).checked_value()
        );
    }
    return result;
}

} // namespace min_plus_convolution_internal

template<std::signed_integral T>
bool is_discrete_convex(const std::vector<T>& values){
    return min_plus_convolution_internal::convex(values);
}

template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_arbitrary(
    const std::vector<T>& convex_values,
    const std::vector<T>& arbitrary_values
){
    if(convex_values.empty() || arbitrary_values.empty()) return {};
    if(!is_discrete_convex(convex_values))[[unlikely]]
        throw std::invalid_argument("min_plus_convolution_convex_arbitrary requires a convex first sequence");
    return min_plus_convolution_internal::convolution(convex_values, arbitrary_values);
}

template<std::signed_integral T>
std::vector<T> min_plus_convolution_arbitrary_convex(
    const std::vector<T>& arbitrary_values,
    const std::vector<T>& convex_values
){
    if(arbitrary_values.empty() || convex_values.empty()) return {};
    if(!is_discrete_convex(convex_values))[[unlikely]]
        throw std::invalid_argument("min_plus_convolution_arbitrary_convex requires a convex second sequence");
    return min_plus_convolution_internal::convolution(convex_values, arbitrary_values);
}

template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_convex(
    const std::vector<T>& left_convex_values,
    const std::vector<T>& right_convex_values
){
    if(left_convex_values.empty() || right_convex_values.empty()) return {};
    if(!is_discrete_convex(left_convex_values))[[unlikely]]
        throw std::invalid_argument("min_plus_convolution_convex_convex requires a convex first sequence");
    if(!is_discrete_convex(right_convex_values))[[unlikely]]
        throw std::invalid_argument("min_plus_convolution_convex_convex requires a convex second sequence");
    return min_plus_convolution_internal::convex_convolution(
        left_convex_values, right_convex_values
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_MIN_PLUS_CONVOLUTION_HPP_INCLUDED
