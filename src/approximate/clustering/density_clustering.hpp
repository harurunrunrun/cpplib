#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <deque>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::clustering {

struct DBSCANResult {
    // -1 denotes noise. Clusters are numbered by first core-point discovery.
    std::vector<int> labels;
    std::size_t cluster_count = 0;
};

template<class Real>
[[nodiscard]] DBSCANResult dbscan(
    const DensePoints<Real>& points,
    long double epsilon,
    std::size_t minimum_points
){
    detail::validate_dense_points(points, true);
    if(!std::isfinite(epsilon) || epsilon < 0.0L || minimum_points == 0){
        throw std::invalid_argument(
            "DBSCAN requires nonnegative finite epsilon and positive min_points"
        );
    }
    if(points.empty()) return {};
    if(points.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error("too many points for int DBSCAN labels");
    }
    std::vector<std::vector<std::size_t>> neighbors(points.size());
    for(std::size_t left = 0; left < points.size(); ++left){
        for(std::size_t right = left; right < points.size(); ++right){
            if(detail::within_euclidean_radius(
                points[left], points[right], epsilon
            )){
                neighbors[left].push_back(right);
                if(left != right) neighbors[right].push_back(left);
            }
        }
    }
    for(auto& list: neighbors) std::sort(list.begin(), list.end());

    constexpr int unvisited = -2;
    DBSCANResult result;
    result.labels.assign(points.size(), unvisited);
    for(std::size_t start = 0; start < points.size(); ++start){
        if(result.labels[start] != unvisited) continue;
        if(neighbors[start].size() < minimum_points){
            result.labels[start] = -1;
            continue;
        }
        const int cluster = static_cast<int>(result.cluster_count);
        ++result.cluster_count;
        result.labels[start] = cluster;
        std::deque<std::size_t> queue(neighbors[start].begin(), neighbors[start].end());
        std::vector<bool> queued(points.size(), false);
        for(const std::size_t point: neighbors[start]) queued[point] = true;
        while(!queue.empty()){
            const std::size_t point = queue.front();
            queue.pop_front();
            if(result.labels[point] == -1) result.labels[point] = cluster;
            if(result.labels[point] != unvisited) continue;
            result.labels[point] = cluster;
            if(neighbors[point].size() < minimum_points) continue;
            for(const std::size_t next: neighbors[point]){
                if(!queued[next]){
                    queued[next] = true;
                    queue.push_back(next);
                }
            }
        }
    }
    return result;
}

struct MeanShiftResult {
    DensePoints<long double> modes;
    std::vector<std::size_t> labels;
    std::vector<std::size_t> iterations;
};

namespace detail {

class ClusteringDisjointSet {
    std::vector<std::size_t> parent_;
    std::vector<std::size_t> size_;

public:
    explicit ClusteringDisjointSet(std::size_t size): parent_(size), size_(size, 1){
        std::iota(parent_.begin(), parent_.end(), std::size_t{0});
    }

    std::size_t leader(std::size_t vertex){
        while(parent_[vertex] != vertex){
            parent_[vertex] = parent_[parent_[vertex]];
            vertex = parent_[vertex];
        }
        return vertex;
    }

    void merge(std::size_t left, std::size_t right){
        left = leader(left);
        right = leader(right);
        if(left == right) return;
        if(size_[left] < size_[right]) std::swap(left, right);
        parent_[right] = left;
        size_[left] += size_[right];
    }
};

}  // namespace detail

template<class Real>
[[nodiscard]] MeanShiftResult mean_shift(
    const DensePoints<Real>& points,
    long double bandwidth,
    std::size_t maximum_iterations = 100,
    long double tolerance = 1.0e-9L,
    long double merge_radius = -1.0L
){
    detail::validate_dense_points(points);
    if(!std::isfinite(bandwidth) || bandwidth <= 0.0L ||
       !std::isfinite(tolerance) || tolerance < 0.0L ||
       !std::isfinite(merge_radius)){
        throw std::invalid_argument("invalid mean-shift radius or tolerance");
    }
    if(merge_radius < 0.0L) merge_radius = tolerance;
    DensePoints<long double> shifted;
    shifted.reserve(points.size());
    for(const auto& point: points) shifted.emplace_back(point.begin(), point.end());
    std::vector<std::size_t> iteration_counts(points.size(), 0);
    for(std::size_t index = 0; index < points.size(); ++index){
        for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
            DensePoint<long double> next(points.front().size(), 0.0L);
            std::size_t count = 0;
            for(const auto& point: points){
                if(!detail::within_euclidean_radius(
                    shifted[index], point, bandwidth
                )) continue;
                ++count;
                const long double rate = 1.0L / static_cast<long double>(count);
                for(std::size_t coordinate = 0; coordinate < point.size(); ++coordinate){
                    next[coordinate] = std::lerp(
                        next[coordinate],
                        static_cast<long double>(point[coordinate]),
                        rate
                    );
                }
            }
            if(count == 0) break;
            ++iteration_counts[index];
            const bool converged = detail::within_euclidean_radius(
                shifted[index], next, tolerance
            );
            shifted[index] = std::move(next);
            if(converged) break;
        }
    }

    detail::ClusteringDisjointSet dsu(points.size());
    for(std::size_t left = 0; left < points.size(); ++left){
        for(std::size_t right = left + 1; right < points.size(); ++right){
            if(detail::within_euclidean_radius(
                shifted[left], shifted[right], merge_radius
            )){
                dsu.merge(left, right);
            }
        }
    }
    std::vector<std::size_t> root_to_label(points.size(), points.size());
    std::vector<std::size_t> labels(points.size());
    std::size_t mode_count = 0;
    for(std::size_t point = 0; point < points.size(); ++point){
        const std::size_t root = dsu.leader(point);
        if(root_to_label[root] == points.size()) root_to_label[root] = mode_count++;
        labels[point] = root_to_label[root];
    }
    DensePoints<long double> modes(
        mode_count, DensePoint<long double>(points.front().size(), 0.0L)
    );
    std::vector<std::size_t> counts(mode_count, 0);
    for(std::size_t point = 0; point < points.size(); ++point){
        ++counts[labels[point]];
        const long double rate =
            1.0L / static_cast<long double>(counts[labels[point]]);
        for(std::size_t coordinate = 0; coordinate < points.front().size();
            ++coordinate){
            modes[labels[point]][coordinate] = std::lerp(
                modes[labels[point]][coordinate],
                shifted[point][coordinate],
                rate
            );
        }
    }
    return {std::move(modes), std::move(labels), std::move(iteration_counts)};
}

}  // namespace approximate::clustering
