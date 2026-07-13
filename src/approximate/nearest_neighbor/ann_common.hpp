#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace approximate::nearest_neighbor {

struct AnnNeighbor {
    std::size_t index = 0;
    long double squared_distance = 0.0L;
};

namespace detail {

inline bool neighbor_less(const AnnNeighbor& first, const AnnNeighbor& second) {
    return first.squared_distance < second.squared_distance
        || (first.squared_distance == second.squared_distance && first.index < second.index);
}

template<class T, std::size_t Dimension>
void validate_ann_point(const std::array<T, Dimension>& point) {
    static_assert(Dimension > 0, "Dimension must be positive");
    static_assert(std::is_arithmetic_v<T>, "coordinates must be arithmetic");
    for(const T& coordinate: point){
        if(!std::isfinite(static_cast<long double>(coordinate))){
            throw std::invalid_argument("point contains a non-finite coordinate");
        }
    }
}

template<class LeftT, class RightT, std::size_t Dimension>
long double ann_squared_distance(
    const std::array<LeftT, Dimension>& first,
    const std::array<RightT, Dimension>& second
) {
    long double result = 0.0L;
    for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
        const long double difference = static_cast<long double>(first[coordinate])
            - static_cast<long double>(second[coordinate]);
        result += difference * difference;
    }
    if(!std::isfinite(result)) throw std::overflow_error("nearest-neighbor distance overflow");
    return result;
}

template<class PointT, class QueryT, std::size_t Dimension>
std::vector<AnnNeighbor> exact_knn(
    const std::vector<std::array<PointT, Dimension>>& points,
    const std::array<QueryT, Dimension>& query,
    std::size_t k,
    std::optional<std::size_t> excluded = std::nullopt
) {
    validate_ann_point(query);
    if(k == 0) throw std::invalid_argument("k must be positive");
    if(excluded.has_value() && *excluded >= points.size()){
        throw std::out_of_range("excluded point index is out of range");
    }
    const std::size_t available = points.size() - (excluded.has_value() ? 1U : 0U);
    if(k > available) throw std::invalid_argument("k exceeds the available point count");
    std::vector<AnnNeighbor> result;
    result.reserve(available);
    for(std::size_t index = 0; index < points.size(); ++index){
        if(excluded.has_value() && index == *excluded) continue;
        result.push_back(AnnNeighbor{index, ann_squared_distance(points[index], query)});
    }
    std::sort(result.begin(), result.end(), neighbor_less);
    result.resize(k);
    return result;
}

inline void sort_unique_indices(std::vector<std::size_t>& indices) {
    std::sort(indices.begin(), indices.end());
    indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
}

}  // namespace detail

}  // namespace approximate::nearest_neighbor
