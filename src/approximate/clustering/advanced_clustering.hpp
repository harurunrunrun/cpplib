#ifndef CPPLIB_SRC_APPROXIMATE_CLUSTERING_ADVANCED_CLUSTERING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CLUSTERING_ADVANCED_CLUSTERING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"
#include "kmeans.hpp"

namespace approximate::clustering {

struct SpectralClusteringResult {
    std::vector<std::size_t> labels;
    DensePoints<long double> embedding;
    std::vector<long double> eigenvalues;
    long double squared_error = 0.0L;
    std::size_t eigensolver_sweeps = 0;
    std::size_t kmeans_iterations = 0;
};

struct IsodataResult {
    DensePoints<long double> centers;
    std::vector<std::size_t> labels;
    long double squared_error = 0.0L;
    std::size_t iterations = 0;
    std::size_t splits = 0;
    std::size_t merges = 0;
};

namespace advanced_clustering_internal {

struct SymmetricEigenResult {
    std::vector<long double> values;
    DensePoints<long double> vectors;
    std::size_t sweeps = 0;
};

inline SymmetricEigenResult jacobi_eigenvectors(
    DensePoints<long double> matrix,
    std::size_t maximum_sweeps,
    long double tolerance
) {
    const std::size_t size = matrix.size();
    DensePoints<long double> vectors(
        size, DensePoint<long double>(size, 0.0L)
    );
    for(std::size_t index = 0; index < size; ++index){
        vectors[index][index] = 1.0L;
    }
    SymmetricEigenResult result;
    for(std::size_t sweep = 0; sweep < maximum_sweeps; ++sweep){
        bool changed = false;
        long double maximum_off_diagonal = 0.0L;
        for(std::size_t first = 0; first < size; ++first){
            for(std::size_t second = first + 1; second < size; ++second){
                const long double off_diagonal =
                    std::fabs(matrix[first][second]);
                maximum_off_diagonal =
                    std::max(maximum_off_diagonal, off_diagonal);
                if(off_diagonal <= tolerance) continue;
                changed = true;
                const long double angle = 0.5L * std::atan2(
                    2.0L * matrix[first][second],
                    matrix[second][second] - matrix[first][first]
                );
                const long double cosine = std::cos(angle);
                const long double sine = std::sin(angle);
                const long double first_diagonal = matrix[first][first];
                const long double second_diagonal = matrix[second][second];
                const long double cross = matrix[first][second];
                matrix[first][first] =
                    cosine * cosine * first_diagonal
                    - 2.0L * sine * cosine * cross
                    + sine * sine * second_diagonal;
                matrix[second][second] =
                    sine * sine * first_diagonal
                    + 2.0L * sine * cosine * cross
                    + cosine * cosine * second_diagonal;
                matrix[first][second] = 0.0L;
                matrix[second][first] = 0.0L;
                for(std::size_t index = 0; index < size; ++index){
                    if(index == first || index == second) continue;
                    const long double to_first = matrix[index][first];
                    const long double to_second = matrix[index][second];
                    matrix[index][first] =
                        cosine * to_first - sine * to_second;
                    matrix[first][index] = matrix[index][first];
                    matrix[index][second] =
                        sine * to_first + cosine * to_second;
                    matrix[second][index] = matrix[index][second];
                }
                for(std::size_t index = 0; index < size; ++index){
                    const long double first_vector = vectors[index][first];
                    const long double second_vector = vectors[index][second];
                    vectors[index][first] =
                        cosine * first_vector - sine * second_vector;
                    vectors[index][second] =
                        sine * first_vector + cosine * second_vector;
                }
            }
        }
        result.sweeps = sweep + 1;
        if(!changed || maximum_off_diagonal <= tolerance) break;
    }
    result.values.resize(size);
    for(std::size_t index = 0; index < size; ++index){
        result.values[index] = matrix[index][index];
    }
    result.vectors = std::move(vectors);
    return result;
}

inline void recompute_isodata_centers(
    const DensePoints<long double>& points,
    const std::vector<std::size_t>& labels,
    DensePoints<long double>& centers,
    std::vector<std::size_t>& counts
) {
    const std::size_t dimension = points.front().size();
    counts.assign(centers.size(), 0);
    DensePoints<long double> sums(
        centers.size(), DensePoint<long double>(dimension, 0.0L)
    );
    for(std::size_t point = 0; point < points.size(); ++point){
        ++counts[labels[point]];
        for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
            sums[labels[point]][coordinate] += points[point][coordinate];
        }
    }
    for(std::size_t cluster = 0; cluster < centers.size(); ++cluster){
        if(counts[cluster] == 0) continue;
        for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
            centers[cluster][coordinate] = sums[cluster][coordinate]
                / static_cast<long double>(counts[cluster]);
        }
    }
}

}  // namespace advanced_clustering_internal

template<class Real, class URBG>
[[nodiscard]] SpectralClusteringResult spectral_clustering(
    const DensePoints<Real>& points,
    std::size_t cluster_count,
    long double bandwidth,
    URBG& random,
    std::size_t maximum_eigensolver_sweeps = 50,
    std::size_t maximum_kmeans_iterations = 100,
    long double tolerance = 1.0e-12L
) {
    detail::validate_dense_points(points);
    detail::validate_cluster_count(points.size(), cluster_count);
    if(!(bandwidth > 0.0L) || !std::isfinite(bandwidth)){
        throw std::invalid_argument(
            "spectral clustering bandwidth must be positive and finite"
        );
    }
    if(maximum_eigensolver_sweeps == 0){
        throw std::invalid_argument(
            "spectral clustering requires an eigensolver sweep"
        );
    }
    if(tolerance < 0.0L || !std::isfinite(tolerance)){
        throw std::invalid_argument(
            "spectral clustering tolerance must be nonnegative and finite"
        );
    }
    const std::size_t point_count = points.size();
    DensePoints<long double> affinity(
        point_count, DensePoint<long double>(point_count, 0.0L)
    );
    std::vector<long double> degree(point_count, 1.0L);
    for(std::size_t point = 0; point < point_count; ++point){
        affinity[point][point] = 1.0L;
    }
    const long double denominator = 2.0L * bandwidth * bandwidth;
    if(!std::isfinite(denominator)){
        throw std::overflow_error("spectral clustering bandwidth overflowed");
    }
    for(std::size_t first = 0; first < point_count; ++first){
        for(std::size_t second = first + 1; second < point_count; ++second){
            const long double distance =
                squared_euclidean_distance(points[first], points[second]);
            const long double similarity = std::exp(-distance / denominator);
            affinity[first][second] = similarity;
            affinity[second][first] = similarity;
            degree[first] += similarity;
            degree[second] += similarity;
        }
    }
    DensePoints<long double> normalized = affinity;
    for(std::size_t first = 0; first < point_count; ++first){
        for(std::size_t second = 0; second < point_count; ++second){
            normalized[first][second] /=
                std::sqrt(degree[first] * degree[second]);
        }
    }
    auto eigen = advanced_clustering_internal::jacobi_eigenvectors(
        std::move(normalized), maximum_eigensolver_sweeps, tolerance
    );
    std::vector<std::size_t> order(point_count);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(
        order.begin(), order.end(),
        [&](std::size_t first, std::size_t second){
            return eigen.values[second] < eigen.values[first];
        }
    );
    SpectralClusteringResult result;
    result.embedding.assign(
        point_count, DensePoint<long double>(cluster_count, 0.0L)
    );
    result.eigenvalues.resize(cluster_count);
    for(std::size_t coordinate = 0; coordinate < cluster_count; ++coordinate){
        result.eigenvalues[coordinate] = eigen.values[order[coordinate]];
        for(std::size_t point = 0; point < point_count; ++point){
            result.embedding[point][coordinate] =
                eigen.vectors[point][order[coordinate]];
        }
    }
    for(auto& point : result.embedding){
        long double norm = 0.0L;
        for(const long double value : point) norm += value * value;
        norm = std::sqrt(norm);
        if(norm != 0.0L){
            for(long double& value : point) value /= norm;
        }
    }
    const KMeansResult clustered = k_means(
        result.embedding, cluster_count, random,
        maximum_kmeans_iterations, tolerance
    );
    result.labels = clustered.labels;
    result.squared_error = clustered.squared_error;
    result.kmeans_iterations = clustered.iterations;
    result.eigensolver_sweeps = eigen.sweeps;
    return result;
}

template<class Real, class URBG>
[[nodiscard]] IsodataResult isodata_clustering(
    const DensePoints<Real>& input_points,
    std::size_t initial_cluster_count,
    std::size_t minimum_cluster_count,
    std::size_t maximum_cluster_count,
    std::size_t minimum_cluster_size,
    long double split_standard_deviation,
    long double merge_distance,
    URBG& random,
    std::size_t maximum_iterations = 100
) {
    detail::validate_dense_points(input_points);
    detail::validate_cluster_count(
        input_points.size(), initial_cluster_count
    );
    if(minimum_cluster_count == 0
       || minimum_cluster_count > initial_cluster_count
       || initial_cluster_count > maximum_cluster_count
       || maximum_cluster_count > input_points.size()){
        throw std::invalid_argument("invalid ISODATA cluster-count bounds");
    }
    if(minimum_cluster_size == 0){
        throw std::invalid_argument(
            "ISODATA minimum cluster size must be positive"
        );
    }
    if(split_standard_deviation < 0.0L
       || !std::isfinite(split_standard_deviation)
       || merge_distance < 0.0L || !std::isfinite(merge_distance)){
        throw std::invalid_argument(
            "ISODATA split and merge thresholds must be finite and nonnegative"
        );
    }
    DensePoints<long double> points;
    points.reserve(input_points.size());
    for(const auto& point : input_points){
        points.emplace_back(point.begin(), point.end());
    }
    DensePoints<long double> centers = k_means_plus_plus_centers(
        points, initial_cluster_count, random
    );
    IsodataResult result;
    std::vector<std::size_t> previous_labels;
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        auto [labels, ignored_error] = detail::assign_to_centers(points, centers);
        static_cast<void>(ignored_error);
        std::vector<std::size_t> counts;
        advanced_clustering_internal::recompute_isodata_centers(
            points, labels, centers, counts
        );
        bool structural_change = false;

        std::vector<std::size_t> keep;
        for(std::size_t cluster = 0; cluster < centers.size(); ++cluster){
            if(counts[cluster] >= minimum_cluster_size
               || keep.size() + (centers.size() - cluster - 1)
                    < minimum_cluster_count){
                keep.push_back(cluster);
            }else{
                structural_change = true;
            }
        }
        if(keep.size() != centers.size()){
            DensePoints<long double> reduced;
            reduced.reserve(keep.size());
            for(const std::size_t cluster : keep){
                reduced.push_back(centers[cluster]);
            }
            centers = std::move(reduced);
            std::tie(labels, ignored_error) =
                detail::assign_to_centers(points, centers);
            advanced_clustering_internal::recompute_isodata_centers(
                points, labels, centers, counts
            );
        }

        while(centers.size() > minimum_cluster_count){
            std::size_t first_best = centers.size();
            std::size_t second_best = centers.size();
            long double best_distance =
                std::numeric_limits<long double>::infinity();
            for(std::size_t first = 0; first < centers.size(); ++first){
                for(std::size_t second = first + 1;
                    second < centers.size(); ++second){
                    const long double distance = std::sqrt(
                        squared_euclidean_distance(
                            centers[first], centers[second]
                        )
                    );
                    if(distance < best_distance){
                        best_distance = distance;
                        first_best = first;
                        second_best = second;
                    }
                }
            }
            if(first_best == centers.size()
               || !(best_distance < merge_distance)) break;
            const std::size_t total = counts[first_best] + counts[second_best];
            for(std::size_t coordinate = 0;
                coordinate < centers[first_best].size(); ++coordinate){
                centers[first_best][coordinate] =
                    (centers[first_best][coordinate]
                        * static_cast<long double>(counts[first_best])
                     + centers[second_best][coordinate]
                        * static_cast<long double>(counts[second_best]))
                    / static_cast<long double>(std::max<std::size_t>(total, 1));
            }
            centers.erase(
                centers.begin() + static_cast<std::ptrdiff_t>(second_best)
            );
            ++result.merges;
            structural_change = true;
            std::tie(labels, ignored_error) =
                detail::assign_to_centers(points, centers);
            advanced_clustering_internal::recompute_isodata_centers(
                points, labels, centers, counts
            );
        }

        const std::size_t split_candidate_count = centers.size();
        for(std::size_t cluster = 0;
            cluster < split_candidate_count
            && centers.size() < maximum_cluster_count; ++cluster){
            if(counts[cluster] < 2 * minimum_cluster_size) continue;
            std::size_t split_coordinate = 0;
            long double largest_variance = 0.0L;
            for(std::size_t coordinate = 0;
                coordinate < centers[cluster].size(); ++coordinate){
                long double variance = 0.0L;
                for(std::size_t point = 0; point < points.size(); ++point){
                    if(labels[point] != cluster) continue;
                    const long double difference =
                        points[point][coordinate]
                        - centers[cluster][coordinate];
                    variance += difference * difference;
                }
                variance /= static_cast<long double>(counts[cluster]);
                if(largest_variance < variance){
                    largest_variance = variance;
                    split_coordinate = coordinate;
                }
            }
            const long double deviation = std::sqrt(largest_variance);
            if(!(split_standard_deviation < deviation)) continue;
            DensePoint<long double> sibling = centers[cluster];
            centers[cluster][split_coordinate] -= deviation * 0.5L;
            sibling[split_coordinate] += deviation * 0.5L;
            centers.push_back(std::move(sibling));
            ++result.splits;
            structural_change = true;
        }
        ++result.iterations;
        if(!structural_change && labels == previous_labels) break;
        previous_labels = std::move(labels);
    }
    auto [labels, error] = detail::assign_to_centers(points, centers);
    result.centers = std::move(centers);
    result.labels = std::move(labels);
    result.squared_error = error;
    return result;
}

}  // namespace approximate::clustering

#endif  // CPPLIB_SRC_APPROXIMATE_CLUSTERING_ADVANCED_CLUSTERING_HPP_INCLUDED
