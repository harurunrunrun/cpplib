#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_OPTICS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_OPTICS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

#include "common.hpp"
#include "detail/stable_numeric.hpp"

namespace approximate::clustering {

struct OPTICSResult {
    std::vector<std::size_t> ordering;
    std::vector<long double> reachability_distance;
    std::vector<long double> core_distance;
};

namespace detail {

struct OPTICSQueueEntry {
    long double distance;
    std::size_t point;
};

struct OPTICSQueueCompare {
    [[nodiscard]] bool operator()(const OPTICSQueueEntry& left, const OPTICSQueueEntry& right) const {
        if(left.distance != right.distance) return left.distance > right.distance;
        return left.point > right.point;
    }
};

}  // namespace detail

template<class Real>
[[nodiscard]] OPTICSResult optics(
    const DensePoints<Real>& points,
    long double maximum_radius,
    std::size_t minimum_points
){
    detail::validate_dense_points(points, true);
    if(!std::isfinite(maximum_radius) || maximum_radius < 0.0L || minimum_points == 0){
        throw std::invalid_argument(
            "OPTICS requires a finite nonnegative radius and positive minimum_points"
        );
    }
    const std::size_t point_count = points.size();
    const long double infinity = std::numeric_limits<long double>::infinity();
    OPTICSResult result;
    result.reachability_distance.assign(point_count, infinity);
    result.core_distance.assign(point_count, infinity);
    result.ordering.reserve(point_count);
    if(point_count == 0) return result;

    std::vector<std::vector<long double>> distance(
        point_count, std::vector<long double>(point_count, 0.0L)
    );
    for(std::size_t left = 0; left < point_count; ++left){
        for(std::size_t right = left + 1; right < point_count; ++right){
            const long double value = detail::stable_euclidean_distance(points[left], points[right]);
            distance[left][right] = value;
            distance[right][left] = value;
        }
    }
    for(std::size_t point = 0; point < point_count; ++point){
        std::vector<long double> neighbor_distances;
        neighbor_distances.reserve(point_count);
        for(const long double value : distance[point]){
            if(value <= maximum_radius) neighbor_distances.push_back(value);
        }
        if(neighbor_distances.size() >= minimum_points){
            const auto nth = neighbor_distances.begin()
                + static_cast<std::ptrdiff_t>(minimum_points - 1);
            std::nth_element(neighbor_distances.begin(), nth, neighbor_distances.end());
            result.core_distance[point] = *nth;
        }
    }

    std::vector<bool> processed(point_count, false);
    using Queue = std::priority_queue<
        detail::OPTICSQueueEntry,
        std::vector<detail::OPTICSQueueEntry>,
        detail::OPTICSQueueCompare
    >;
    for(std::size_t start = 0; start < point_count; ++start){
        if(processed[start]) continue;
        Queue queue;
        const auto process = [&](std::size_t point, Queue& candidates){
            processed[point] = true;
            result.ordering.push_back(point);
            if(!std::isfinite(result.core_distance[point])) return;
            for(std::size_t neighbor = 0; neighbor < point_count; ++neighbor){
                if(processed[neighbor] || distance[point][neighbor] > maximum_radius) continue;
                const long double candidate = std::max(
                    result.core_distance[point], distance[point][neighbor]
                );
                if(candidate < result.reachability_distance[neighbor]){
                    result.reachability_distance[neighbor] = candidate;
                    candidates.push({candidate, neighbor});
                }
            }
        };
        process(start, queue);
        while(!queue.empty()){
            const detail::OPTICSQueueEntry entry = queue.top();
            queue.pop();
            if(processed[entry.point] || entry.distance != result.reachability_distance[entry.point]){
                continue;
            }
            process(entry.point, queue);
        }
    }
    return result;
}

}  // namespace approximate::clustering

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_OPTICS_HPP_INCLUDED
