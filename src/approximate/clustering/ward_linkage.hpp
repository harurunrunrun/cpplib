#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"
#include "detail/stable_numeric.hpp"

namespace approximate::clustering {

struct WardMerge {
    std::size_t left = 0;
    std::size_t right = 0;
    long double squared_error_increase = 0.0L;
    std::size_t size = 0;
};

struct WardClusteringResult {
    std::vector<WardMerge> merges;
};

template<class Real>
[[nodiscard]] WardClusteringResult ward_linkage(const DensePoints<Real>& points){
    detail::validate_dense_points(points, true);
    const std::size_t point_count = points.size();
    if(point_count <= 1) return {};
    if(point_count > (std::numeric_limits<std::size_t>::max() / 2) + 1){
        throw std::length_error("too many points for a Ward dendrogram");
    }
    const std::size_t node_count = point_count * 2 - 1;
    DensePoints<long double> centroid(
        node_count, DensePoint<long double>(points.front().size(), 0.0L)
    );
    for(std::size_t point = 0; point < point_count; ++point){
        centroid[point].assign(points[point].begin(), points[point].end());
    }
    std::vector<std::size_t> size(node_count, 0);
    std::fill(size.begin(), size.begin() + static_cast<std::ptrdiff_t>(point_count), 1);
    std::vector<bool> active(node_count, false);
    std::fill(active.begin(), active.begin() + static_cast<std::ptrdiff_t>(point_count), true);

    WardClusteringResult result;
    result.merges.reserve(point_count - 1);
    for(std::size_t step = 0; step + 1 < point_count; ++step){
        const std::size_t existing_nodes = point_count + step;
        std::size_t best_left = node_count;
        std::size_t best_right = node_count;
        long double best_cost = std::numeric_limits<long double>::infinity();
        for(std::size_t left = 0; left < existing_nodes; ++left){
            if(!active[left]) continue;
            for(std::size_t right = left + 1; right < existing_nodes; ++right){
                if(!active[right]) continue;
                const long double distance = detail::stable_euclidean_distance(
                    centroid[left], centroid[right]
                );
                const long double squared_distance = detail::checked_nonnegative_product(
                    distance, distance, "Ward squared centroid distance overflowed"
                );
                const std::size_t merged_size = size[left] + size[right];
                const long double factor = static_cast<long double>(size[left])
                    * (static_cast<long double>(size[right]) / static_cast<long double>(merged_size));
                const long double cost = detail::checked_nonnegative_product(
                    factor, squared_distance, "Ward merge cost overflowed"
                );
                if(cost < best_cost ||
                   (cost == best_cost && std::pair{left, right} < std::pair{best_left, best_right})){
                    best_cost = cost;
                    best_left = left;
                    best_right = right;
                }
            }
        }
        const std::size_t merged = point_count + step;
        const std::size_t merged_size = size[best_left] + size[best_right];
        result.merges.push_back({best_left, best_right, best_cost, merged_size});
        const long double right_weight = static_cast<long double>(size[best_right])
            / static_cast<long double>(merged_size);
        for(std::size_t coordinate = 0; coordinate < centroid[merged].size(); ++coordinate){
            centroid[merged][coordinate] = std::lerp(
                centroid[best_left][coordinate], centroid[best_right][coordinate], right_weight
            );
        }
        size[merged] = merged_size;
        active[best_left] = false;
        active[best_right] = false;
        active[merged] = true;
    }
    return result;
}

}  // namespace approximate::clustering
