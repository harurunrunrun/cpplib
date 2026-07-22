#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_DETAIL_STABLE_NUMERIC_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_DETAIL_STABLE_NUMERIC_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>

#include "../common.hpp"

namespace approximate::clustering::detail {

[[nodiscard]] inline long double stable_absolute_difference(
    long double left,
    long double right
){
    if(!std::isfinite(left) || !std::isfinite(right)){
        throw std::invalid_argument("coordinates must be finite");
    }
    if(std::signbit(left) != std::signbit(right)){
        const long double left_absolute = std::fabs(left);
        const long double right_absolute = std::fabs(right);
        if(left_absolute > std::numeric_limits<long double>::max() - right_absolute){
            throw std::overflow_error("Euclidean distance overflowed");
        }
        return left_absolute + right_absolute;
    }
    return std::fabs(left - right);
}

template<class LeftReal, class RightReal>
[[nodiscard]] long double stable_euclidean_distance(
    const DensePoint<LeftReal>& left,
    const DensePoint<RightReal>& right
){
    if(left.size() != right.size()){
        throw std::invalid_argument("points must have the same dimension");
    }
    long double result = 0.0L;
    for(std::size_t coordinate = 0; coordinate < left.size(); ++coordinate){
        result = std::hypot(result, stable_absolute_difference(
            static_cast<long double>(left[coordinate]),
            static_cast<long double>(right[coordinate])
        ));
        if(!std::isfinite(result)){
            throw std::overflow_error("Euclidean distance overflowed");
        }
    }
    return result;
}

[[nodiscard]] inline long double checked_nonnegative_product(
    long double left,
    long double right,
    const char* message
){
    if(!std::isfinite(left) || !std::isfinite(right) || left < 0.0L || right < 0.0L){
        throw std::invalid_argument("product operands must be finite and nonnegative");
    }
    if(left != 0.0L && right > std::numeric_limits<long double>::max() / left){
        throw std::overflow_error(message);
    }
    return left * right;
}

[[nodiscard]] inline long double checked_finite_sum(
    long double left,
    long double right,
    const char* message
){
    if(!std::isfinite(left) || !std::isfinite(right)){
        throw std::overflow_error(message);
    }
    if((right > 0.0L && left > std::numeric_limits<long double>::max() - right)
       || (right < 0.0L && left < -std::numeric_limits<long double>::max() - right)){
        throw std::overflow_error(message);
    }
    return left + right;
}

}  // namespace approximate::clustering::detail

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_DETAIL_STABLE_NUMERIC_HPP_INCLUDED
