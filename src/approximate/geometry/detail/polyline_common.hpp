#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace approximate::geometry::detail {

template<class T, std::size_t Dimension>
void validate_polyline_point(const std::array<T, Dimension>& point) {
    for(const T& coordinate: point){
        if(!std::isfinite(static_cast<long double>(coordinate))){
            throw std::invalid_argument("point contains a non-finite coordinate");
        }
    }
}

template<class T, std::size_t Dimension>
long double point_segment_squared_distance(
    const std::array<T, Dimension>& point,
    const std::array<T, Dimension>& first,
    const std::array<T, Dimension>& second
) {
    long double segment_squared_norm = 0.0L;
    long double projection_numerator = 0.0L;
    for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
        const long double delta = static_cast<long double>(second[coordinate])
            - static_cast<long double>(first[coordinate]);
        const long double offset = static_cast<long double>(point[coordinate])
            - static_cast<long double>(first[coordinate]);
        segment_squared_norm += delta * delta;
        projection_numerator += offset * delta;
    }
    long double ratio = 0.0L;
    if(segment_squared_norm > 0.0L){
        ratio = std::clamp(projection_numerator / segment_squared_norm, 0.0L, 1.0L);
    }
    long double result = 0.0L;
    for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
        const long double projected = static_cast<long double>(first[coordinate])
            + ratio * (static_cast<long double>(second[coordinate])
                       - static_cast<long double>(first[coordinate]));
        const long double difference = static_cast<long double>(point[coordinate]) - projected;
        result += difference * difference;
    }
    if(!std::isfinite(result)) throw std::overflow_error("squared distance overflow");
    return result;
}

template<class T>
long double doubled_triangle_area(
    const std::array<T, 2>& first,
    const std::array<T, 2>& middle,
    const std::array<T, 2>& last
) {
    const long double ax = static_cast<long double>(middle[0])
        - static_cast<long double>(first[0]);
    const long double ay = static_cast<long double>(middle[1])
        - static_cast<long double>(first[1]);
    const long double bx = static_cast<long double>(last[0])
        - static_cast<long double>(first[0]);
    const long double by = static_cast<long double>(last[1])
        - static_cast<long double>(first[1]);
    const long double area = std::fabs(ax * by - ay * bx);
    if(!std::isfinite(area)) throw std::overflow_error("triangle area overflow");
    return area;
}

}  // namespace approximate::geometry::detail
