#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/polyline_common.hpp"

namespace approximate::geometry {

template<class T, std::size_t Dimension>
std::vector<std::size_t> douglas_peucker_indices(
    const std::vector<std::array<T, Dimension>>& polyline,
    long double tolerance
) {
    if(!std::isfinite(tolerance) || tolerance < 0.0L){
        throw std::invalid_argument("tolerance must be finite and non-negative");
    }
    for(const auto& point: polyline) detail::validate_polyline_point(point);
    if(polyline.size() <= 2){
        std::vector<std::size_t> indices(polyline.size());
        for(std::size_t index = 0; index < polyline.size(); ++index) indices[index] = index;
        return indices;
    }
    const long double squared_tolerance = tolerance * tolerance;
    if(!std::isfinite(squared_tolerance)){
        throw std::overflow_error("squared tolerance overflow");
    }
    std::vector<unsigned char> retained(polyline.size(), 0U);
    retained.front() = 1U;
    retained.back() = 1U;
    std::vector<std::pair<std::size_t, std::size_t>> stack;
    stack.emplace_back(0, polyline.size() - 1);
    while(!stack.empty()){
        const auto [first, last] = stack.back();
        stack.pop_back();
        long double maximum_distance = -1.0L;
        std::size_t maximum_index = first;
        for(std::size_t index = first + 1; index < last; ++index){
            const long double distance = detail::point_segment_squared_distance(
                polyline[index], polyline[first], polyline[last]
            );
            if(distance > maximum_distance){
                maximum_distance = distance;
                maximum_index = index;
            }
        }
        if(maximum_distance > squared_tolerance){
            retained[maximum_index] = 1U;
            stack.emplace_back(maximum_index, last);
            stack.emplace_back(first, maximum_index);
        }
    }
    std::vector<std::size_t> result;
    result.reserve(polyline.size());
    for(std::size_t index = 0; index < polyline.size(); ++index){
        if(retained[index] != 0U) result.push_back(index);
    }
    return result;
}

template<class T, std::size_t Dimension>
std::vector<std::array<T, Dimension>> douglas_peucker(
    const std::vector<std::array<T, Dimension>>& polyline,
    long double tolerance
) {
    const auto indices = douglas_peucker_indices(polyline, tolerance);
    std::vector<std::array<T, Dimension>> result;
    result.reserve(indices.size());
    for(std::size_t index: indices) result.push_back(polyline[index]);
    return result;
}

}  // namespace approximate::geometry
