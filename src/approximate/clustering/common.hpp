#pragma once

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace approximate::clustering {

template<class Real>
using DensePoint = std::vector<Real>;

template<class Real>
using DensePoints = std::vector<DensePoint<Real>>;

namespace detail {

template<class Real>
void validate_dense_points(const DensePoints<Real>& points, bool allow_empty = false){
    static_assert(std::is_arithmetic_v<Real>, "coordinates must be arithmetic");
    if(points.empty()){
        if(allow_empty) return;
        throw std::invalid_argument("clustering requires at least one point");
    }
    const std::size_t dimension = points.front().size();
    if(dimension == 0){
        throw std::invalid_argument("points must have positive dimension");
    }
    for(const auto& point: points){
        if(point.size() != dimension){
            throw std::invalid_argument("all points must have the same dimension");
        }
        for(const Real coordinate: point){
            if constexpr(std::is_floating_point_v<Real>){
                if(!std::isfinite(coordinate)){
                    throw std::invalid_argument("coordinates must be finite");
                }
            }
        }
    }
}

inline void validate_cluster_count(std::size_t point_count, std::size_t cluster_count){
    if(cluster_count == 0 || cluster_count > point_count){
        throw std::invalid_argument("cluster count must be in [1, point count]");
    }
}

inline long double checked_distance(long double value){
    if(!std::isfinite(value) || value < 0.0L){
        throw std::invalid_argument("distances must be finite and nonnegative");
    }
    return value;
}

inline long double checked_cost_sum(
    long double total,
    long double value,
    const char* message
){
    if(value > std::numeric_limits<long double>::max() - total){
        throw std::overflow_error(message);
    }
    return total + value;
}

template<class LeftReal, class RightReal>
[[nodiscard]] bool within_euclidean_radius(
    const DensePoint<LeftReal>& left,
    const DensePoint<RightReal>& right,
    long double radius
){
    if(left.size() != right.size()){
        throw std::invalid_argument("points must have the same dimension");
    }
    if(!std::isfinite(radius) || radius < 0.0L){
        throw std::invalid_argument("radius must be finite and nonnegative");
    }
    if(radius == 0.0L){
        for(std::size_t coordinate = 0; coordinate < left.size(); ++coordinate){
            if(static_cast<long double>(left[coordinate]) !=
               static_cast<long double>(right[coordinate])) return false;
        }
        return true;
    }
    long double normalized_squared_distance = 0.0L;
    for(std::size_t coordinate = 0; coordinate < left.size(); ++coordinate){
        const long double left_value = static_cast<long double>(left[coordinate]);
        const long double right_value = static_cast<long double>(right[coordinate]);
        if(!std::isfinite(left_value) || !std::isfinite(right_value)){
            throw std::invalid_argument("coordinates must be finite");
        }
        long double difference = 0.0L;
        if(std::signbit(left_value) != std::signbit(right_value)){
            const long double left_absolute = std::fabs(left_value);
            const long double right_absolute = std::fabs(right_value);
            if(left_absolute > radius || right_absolute > radius ||
               left_absolute > radius - right_absolute) return false;
            difference = left_absolute + right_absolute;
        }else{
            difference = std::fabs(left_value - right_value);
            if(difference > radius) return false;
        }
        const long double normalized = difference / radius;
        normalized_squared_distance += normalized * normalized;
        if(normalized_squared_distance > 1.0L) return false;
    }
    return true;
}

}  // namespace detail

template<class LeftReal, class RightReal>
[[nodiscard]] long double squared_euclidean_distance(
    const DensePoint<LeftReal>& left,
    const DensePoint<RightReal>& right
){
    if(left.size() != right.size()){
        throw std::invalid_argument("points must have the same dimension");
    }
    long double result = 0.0L;
    for(std::size_t coordinate = 0; coordinate < left.size(); ++coordinate){
        const long double difference = static_cast<long double>(left[coordinate]) -
                                       static_cast<long double>(right[coordinate]);
        result += difference * difference;
    }
    if(!std::isfinite(result)){
        throw std::overflow_error("squared Euclidean distance overflowed");
    }
    return result;
}

template<class LeftReal, class RightReal>
[[nodiscard]] long double euclidean_distance(
    const DensePoint<LeftReal>& left,
    const DensePoint<RightReal>& right
){
    return std::sqrt(squared_euclidean_distance(left, right));
}

}  // namespace approximate::clustering
