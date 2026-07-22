#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_METRIC_K_CENTER_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_METRIC_K_CENTER_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::clustering {

struct KCenterResult {
    // Centers are stored in Gonzalez selection order.
    std::vector<std::size_t> centers;
    std::vector<std::size_t> labels;
    long double radius = 0.0L;
};

template<class Distance>
[[nodiscard]] KCenterResult gonzalez_k_center(
    std::size_t point_count,
    std::size_t center_count,
    Distance distance,
    std::size_t first_center = 0
){
    detail::validate_cluster_count(point_count, center_count);
    if(first_center >= point_count){
        throw std::out_of_range("first center is outside the point set");
    }
    KCenterResult result;
    result.centers.reserve(center_count);
    result.centers.push_back(first_center);
    std::vector<bool> selected(point_count, false);
    selected[first_center] = true;
    std::vector<long double> nearest(point_count);
    for(std::size_t point = 0; point < point_count; ++point){
        nearest[point] = detail::checked_distance(static_cast<long double>(
            std::invoke(distance, point, first_center)
        ));
    }
    while(result.centers.size() < center_count){
        std::size_t farthest = point_count;
        long double farthest_distance = -1.0L;
        for(std::size_t point = 0; point < point_count; ++point){
            if(selected[point]) continue;
            if(nearest[point] > farthest_distance){
                farthest_distance = nearest[point];
                farthest = point;
            }
        }
        result.centers.push_back(farthest);
        selected[farthest] = true;
        for(std::size_t point = 0; point < point_count; ++point){
            nearest[point] = std::min(
                nearest[point],
                detail::checked_distance(static_cast<long double>(
                    std::invoke(distance, point, farthest)
                ))
            );
        }
    }

    result.labels.resize(point_count);
    result.radius = 0.0L;
    for(std::size_t point = 0; point < point_count; ++point){
        std::size_t best_position = 0;
        long double best_distance = detail::checked_distance(
            static_cast<long double>(
                std::invoke(distance, point, result.centers[0])
            )
        );
        for(std::size_t position = 1; position < result.centers.size(); ++position){
            const long double candidate = detail::checked_distance(
                static_cast<long double>(
                    std::invoke(distance, point, result.centers[position])
                )
            );
            if(candidate < best_distance ||
               (candidate == best_distance &&
                result.centers[position] < result.centers[best_position])){
                best_distance = candidate;
                best_position = position;
            }
        }
        result.labels[point] = best_position;
        result.radius = std::max(result.radius, best_distance);
    }
    return result;
}

}  // namespace approximate::clustering

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_METRIC_K_CENTER_HPP_INCLUDED
