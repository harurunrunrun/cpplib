#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::clustering {

struct KMeansResult {
    DensePoints<long double> centers;
    std::vector<std::size_t> labels;
    long double squared_error = 0.0L;
    std::size_t iterations = 0;
};

namespace detail {

template<class Real>
std::pair<std::vector<std::size_t>, long double> assign_to_centers(
    const DensePoints<Real>& points,
    const DensePoints<long double>& centers
){
    std::vector<std::size_t> labels(points.size());
    long double error = 0.0L;
    for(std::size_t point = 0; point < points.size(); ++point){
        std::size_t best = 0;
        long double best_distance = squared_euclidean_distance(points[point], centers[0]);
        for(std::size_t center = 1; center < centers.size(); ++center){
            const long double distance =
                squared_euclidean_distance(points[point], centers[center]);
            if(distance < best_distance){
                best = center;
                best_distance = distance;
            }
        }
        labels[point] = best;
        error = checked_cost_sum(error, best_distance, "k-means objective overflowed");
        if(!std::isfinite(error)){
            throw std::overflow_error("k-means objective overflowed");
        }
    }
    return {std::move(labels), error};
}

inline void validate_kmeans_parameters(
    std::size_t point_count,
    const DensePoints<long double>& centers,
    long double tolerance
){
    validate_cluster_count(point_count, centers.size());
    validate_dense_points(centers);
    if(!std::isfinite(tolerance) || tolerance < 0.0L){
        throw std::invalid_argument("tolerance must be finite and nonnegative");
    }
}

}  // namespace detail

template<class Real>
[[nodiscard]] KMeansResult lloyd_k_means(
    const DensePoints<Real>& points,
    DensePoints<long double> initial_centers,
    std::size_t maximum_iterations = 100,
    long double tolerance = 1.0e-12L
){
    detail::validate_dense_points(points);
    detail::validate_kmeans_parameters(points.size(), initial_centers, tolerance);
    if(points.front().size() != initial_centers.front().size()){
        throw std::invalid_argument("points and centers must have the same dimension");
    }

    const std::size_t cluster_count = initial_centers.size();
    const std::size_t dimension = points.front().size();
    KMeansResult result;
    result.centers = std::move(initial_centers);
    std::vector<std::size_t> previous_labels(
        points.size(), std::numeric_limits<std::size_t>::max()
    );

    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        auto [labels, ignored_error] = detail::assign_to_centers(points, result.centers);
        (void)ignored_error;
        DensePoints<long double> next_centers(
            cluster_count, DensePoint<long double>(dimension, 0.0L)
        );
        std::vector<std::size_t> counts(cluster_count, 0);
        for(std::size_t point = 0; point < points.size(); ++point){
            const std::size_t cluster = labels[point];
            ++counts[cluster];
            const long double rate =
                1.0L / static_cast<long double>(counts[cluster]);
            for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                next_centers[cluster][coordinate] = std::lerp(
                    next_centers[cluster][coordinate],
                    static_cast<long double>(points[point][coordinate]),
                    rate
                );
            }
        }
        bool centers_stable = true;
        for(std::size_t cluster = 0; cluster < cluster_count; ++cluster){
            if(counts[cluster] == 0){
                next_centers[cluster] = result.centers[cluster];
            }
            if(!detail::within_euclidean_radius(
                result.centers[cluster], next_centers[cluster], tolerance
            )) centers_stable = false;
        }
        const bool assignments_unchanged = labels == previous_labels;
        previous_labels = std::move(labels);
        result.centers = std::move(next_centers);
        result.iterations = iteration + 1;
        if(assignments_unchanged || centers_stable) break;
    }

    auto [labels, error] = detail::assign_to_centers(points, result.centers);
    result.labels = std::move(labels);
    result.squared_error = error;
    return result;
}

template<class Real, class URBG>
[[nodiscard]] DensePoints<long double> k_means_plus_plus_centers(
    const DensePoints<Real>& points,
    std::size_t cluster_count,
    URBG& random
){
    detail::validate_dense_points(points);
    detail::validate_cluster_count(points.size(), cluster_count);

    std::uniform_int_distribution<std::size_t> first_distribution(0, points.size() - 1);
    std::vector<std::size_t> selected{first_distribution(random)};
    std::vector<bool> used(points.size(), false);
    used[selected.front()] = true;
    std::vector<long double> nearest(points.size(),
                                     std::numeric_limits<long double>::infinity());

    while(selected.size() < cluster_count){
        const std::size_t newest = selected.back();
        for(std::size_t point = 0; point < points.size(); ++point){
            nearest[point] = std::min(
                nearest[point],
                squared_euclidean_distance(points[point], points[newest])
            );
        }
        std::vector<double> weights(points.size(), 0.0);
        long double scale = 0.0L;
        for(std::size_t point = 0; point < points.size(); ++point){
            if(!used[point]) scale = std::max(scale, nearest[point]);
        }
        std::size_t next = points.size();
        if(scale > 0.0L){
            for(std::size_t point = 0; point < points.size(); ++point){
                if(!used[point]){
                    weights[point] = static_cast<double>(nearest[point] / scale);
                }
            }
            std::discrete_distribution<std::size_t> distribution(
                weights.begin(), weights.end()
            );
            next = distribution(random);
            if(used[next]) next = points.size();
        }
        if(next == points.size()){
            for(std::size_t point = 0; point < points.size(); ++point){
                if(!used[point]){
                    next = point;
                    break;
                }
            }
        }
        selected.push_back(next);
        used[next] = true;
    }

    DensePoints<long double> centers;
    centers.reserve(cluster_count);
    for(const std::size_t index: selected){
        centers.emplace_back(points[index].begin(), points[index].end());
    }
    return centers;
}

template<class Real, class URBG>
[[nodiscard]] KMeansResult k_means(
    const DensePoints<Real>& points,
    std::size_t cluster_count,
    URBG& random,
    std::size_t maximum_iterations = 100,
    long double tolerance = 1.0e-12L
){
    return lloyd_k_means(
        points,
        k_means_plus_plus_centers(points, cluster_count, random),
        maximum_iterations,
        tolerance
    );
}

template<class Real, class URBG>
[[nodiscard]] KMeansResult mini_batch_k_means(
    const DensePoints<Real>& points,
    std::size_t cluster_count,
    std::size_t batch_size,
    std::size_t iterations,
    URBG& random
){
    detail::validate_dense_points(points);
    detail::validate_cluster_count(points.size(), cluster_count);
    if(batch_size == 0){
        throw std::invalid_argument("batch size must be positive");
    }
    KMeansResult result;
    result.centers = k_means_plus_plus_centers(points, cluster_count, random);
    std::vector<std::size_t> counts(cluster_count, 0);
    std::uniform_int_distribution<std::size_t> distribution(0, points.size() - 1);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        for(std::size_t sample = 0; sample < batch_size; ++sample){
            const std::size_t point = distribution(random);
            std::size_t closest = 0;
            long double best = squared_euclidean_distance(
                points[point], result.centers[0]
            );
            for(std::size_t cluster = 1; cluster < cluster_count; ++cluster){
                const long double distance = squared_euclidean_distance(
                    points[point], result.centers[cluster]
                );
                if(distance < best){
                    best = distance;
                    closest = cluster;
                }
            }
            ++counts[closest];
            if(counts[closest] == 0){
                throw std::overflow_error("mini-batch update count overflowed");
            }
            const long double rate = 1.0L / static_cast<long double>(counts[closest]);
            for(std::size_t coordinate = 0; coordinate < points.front().size();
                ++coordinate){
                result.centers[closest][coordinate] = std::lerp(
                    result.centers[closest][coordinate],
                    static_cast<long double>(points[point][coordinate]),
                    rate
                );
            }
        }
    }
    result.iterations = iterations;
    auto [labels, error] = detail::assign_to_centers(points, result.centers);
    result.labels = std::move(labels);
    result.squared_error = error;
    return result;
}

}  // namespace approximate::clustering
