#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_BIRCH_FEATURES_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_BIRCH_FEATURES_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"
#include "detail/stable_numeric.hpp"

namespace approximate::clustering {

struct ClusteringFeature {
    std::size_t count = 0;
    DensePoint<long double> centroid;
    DensePoint<long double> coordinate_variance;
    long double squared_radius = 0.0L;
};

struct BirchFeatureResult {
    std::vector<ClusteringFeature> features;
    std::vector<std::size_t> labels;
};

namespace detail {

[[nodiscard]] inline long double feature_squared_radius(
    const DensePoint<long double>& coordinate_variance
){
    long double result = 0.0L;
    for(const long double value : coordinate_variance){
        result = checked_cost_sum(result, value, "BIRCH feature radius overflowed");
    }
    return result;
}

template<class Real>
[[nodiscard]] ClusteringFeature singleton_feature(const DensePoint<Real>& point){
    ClusteringFeature result;
    result.count = 1;
    result.centroid.assign(point.begin(), point.end());
    result.coordinate_variance.assign(point.size(), 0.0L);
    return result;
}

template<class Real>
[[nodiscard]] ClusteringFeature append_to_feature(
    const ClusteringFeature& feature,
    const DensePoint<Real>& point
){
    if(feature.count == std::numeric_limits<std::size_t>::max()){
        throw std::overflow_error("BIRCH feature count overflowed");
    }
    ClusteringFeature result = feature;
    result.count = feature.count + 1;
    const long double new_count = static_cast<long double>(result.count);
    const long double old_weight = static_cast<long double>(feature.count) / new_count;
    for(std::size_t coordinate = 0; coordinate < point.size(); ++coordinate){
        const long double value = static_cast<long double>(point[coordinate]);
        const long double next_mean = std::lerp(
            feature.centroid[coordinate], value, 1.0L / new_count
        );
        const long double left_difference = stable_absolute_difference(
            value, feature.centroid[coordinate]
        );
        const long double right_difference = stable_absolute_difference(value, next_mean);
        const long double innovation = checked_nonnegative_product(
            left_difference / new_count,
            right_difference,
            "BIRCH coordinate variance overflowed"
        );
        const long double retained = feature.coordinate_variance[coordinate] * old_weight;
        result.coordinate_variance[coordinate] = checked_cost_sum(
            retained, innovation, "BIRCH coordinate variance overflowed"
        );
        result.centroid[coordinate] = next_mean;
    }
    result.squared_radius = feature_squared_radius(result.coordinate_variance);
    return result;
}

[[nodiscard]] inline ClusteringFeature merge_features(
    const ClusteringFeature& left,
    const ClusteringFeature& right
){
    if(right.count > std::numeric_limits<std::size_t>::max() - left.count){
        throw std::overflow_error("BIRCH feature count overflowed");
    }
    ClusteringFeature result;
    result.count = left.count + right.count;
    const long double left_weight = static_cast<long double>(left.count)
        / static_cast<long double>(result.count);
    const long double right_weight = static_cast<long double>(right.count)
        / static_cast<long double>(result.count);
    const long double coefficient = std::sqrt(left_weight * right_weight);
    result.centroid.resize(left.centroid.size());
    result.coordinate_variance.resize(left.centroid.size());
    for(std::size_t coordinate = 0; coordinate < left.centroid.size(); ++coordinate){
        result.centroid[coordinate] = std::lerp(
            left.centroid[coordinate], right.centroid[coordinate], right_weight
        );
        const long double base = std::lerp(
            left.coordinate_variance[coordinate],
            right.coordinate_variance[coordinate],
            right_weight
        );
        const long double scaled_difference = stable_absolute_difference(
            left.centroid[coordinate], right.centroid[coordinate]
        ) * coefficient;
        const long double between = checked_nonnegative_product(
            scaled_difference, scaled_difference, "BIRCH merged variance overflowed"
        );
        result.coordinate_variance[coordinate] = checked_cost_sum(
            base, between, "BIRCH merged variance overflowed"
        );
    }
    result.squared_radius = feature_squared_radius(result.coordinate_variance);
    return result;
}

}  // namespace detail

template<class Real>
[[nodiscard]] BirchFeatureResult birch_clustering_features(
    const DensePoints<Real>& points,
    long double threshold,
    std::size_t maximum_features
){
    detail::validate_dense_points(points, true);
    if(!std::isfinite(threshold) || threshold < 0.0L || maximum_features == 0){
        throw std::invalid_argument("invalid BIRCH threshold or feature limit");
    }
    BirchFeatureResult result;
    if(points.empty()) return result;
    result.features.reserve(std::min(maximum_features, points.size()));
    for(const auto& point : points){
        if(result.features.empty()){
            result.features.push_back(detail::singleton_feature(point));
            continue;
        }
        std::size_t nearest = 0;
        long double nearest_distance = detail::stable_euclidean_distance(
            point, result.features.front().centroid
        );
        for(std::size_t feature = 1; feature < result.features.size(); ++feature){
            const long double distance = detail::stable_euclidean_distance(
                point, result.features[feature].centroid
            );
            if(distance < nearest_distance){
                nearest = feature;
                nearest_distance = distance;
            }
        }
        ClusteringFeature appended = detail::append_to_feature(result.features[nearest], point);
        if(std::sqrt(appended.squared_radius) <= threshold){
            result.features[nearest] = std::move(appended);
        }else{
            result.features.push_back(detail::singleton_feature(point));
        }
        if(result.features.size() > maximum_features){
            std::size_t best_left = 0;
            std::size_t best_right = 1;
            long double best_distance = detail::stable_euclidean_distance(
                result.features[0].centroid, result.features[1].centroid
            );
            for(std::size_t left = 0; left < result.features.size(); ++left){
                for(std::size_t right = left + 1; right < result.features.size(); ++right){
                    const long double distance = detail::stable_euclidean_distance(
                        result.features[left].centroid, result.features[right].centroid
                    );
                    if(distance < best_distance ||
                       (distance == best_distance
                        && std::pair{left, right} < std::pair{best_left, best_right})){
                        best_distance = distance;
                        best_left = left;
                        best_right = right;
                    }
                }
            }
            result.features[best_left] = detail::merge_features(
                result.features[best_left], result.features[best_right]
            );
            result.features.erase(result.features.begin() + static_cast<std::ptrdiff_t>(best_right));
        }
    }

    result.labels.resize(points.size());
    for(std::size_t point = 0; point < points.size(); ++point){
        std::size_t nearest = 0;
        long double nearest_distance = detail::stable_euclidean_distance(
            points[point], result.features.front().centroid
        );
        for(std::size_t feature = 1; feature < result.features.size(); ++feature){
            const long double distance = detail::stable_euclidean_distance(
                points[point], result.features[feature].centroid
            );
            if(distance < nearest_distance){
                nearest = feature;
                nearest_distance = distance;
            }
        }
        result.labels[point] = nearest;
    }
    return result;
}

}  // namespace approximate::clustering

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_BIRCH_FEATURES_HPP_INCLUDED
