#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::clustering {

struct KMedoidsResult {
    std::vector<std::size_t> medoids;
    std::vector<std::size_t> labels;
    long double cost = 0.0L;
    std::size_t iterations = 0;
};

namespace detail {

template<class Distance>
std::vector<std::vector<long double>> make_distance_matrix(
    std::size_t point_count,
    Distance& distance
){
    std::vector<std::vector<long double>> matrix(
        point_count, std::vector<long double>(point_count)
    );
    for(std::size_t left = 0; left < point_count; ++left){
        for(std::size_t right = 0; right < point_count; ++right){
            matrix[left][right] = checked_distance(static_cast<long double>(
                std::invoke(distance, left, right)
            ));
        }
    }
    return matrix;
}

inline std::pair<std::vector<std::size_t>, long double> medoid_assignment(
    const std::vector<std::vector<long double>>& distance,
    const std::vector<std::size_t>& medoids
){
    std::vector<std::size_t> labels(distance.size());
    long double cost = 0.0L;
    for(std::size_t point = 0; point < distance.size(); ++point){
        std::size_t best_position = 0;
        long double best_distance = distance[point][medoids[0]];
        for(std::size_t position = 1; position < medoids.size(); ++position){
            const long double candidate = distance[point][medoids[position]];
            if(candidate < best_distance ||
               (candidate == best_distance &&
                medoids[position] < medoids[best_position])){
                best_distance = candidate;
                best_position = position;
            }
        }
        labels[point] = best_position;
        cost = checked_cost_sum(
            cost, best_distance, "k-medoids objective overflowed"
        );
    }
    return {std::move(labels), cost};
}

inline KMedoidsResult pam_from_matrix(
    const std::vector<std::vector<long double>>& distance,
    std::size_t cluster_count,
    std::size_t maximum_iterations
){
    const std::size_t point_count = distance.size();
    std::vector<std::size_t> medoids;
    medoids.reserve(cluster_count);
    std::vector<bool> selected(point_count, false);
    std::vector<long double> nearest(
        point_count, std::numeric_limits<long double>::infinity()
    );

    while(medoids.size() < cluster_count){
        std::size_t best_candidate = point_count;
        long double best_cost = std::numeric_limits<long double>::infinity();
        for(std::size_t candidate = 0; candidate < point_count; ++candidate){
            if(selected[candidate]) continue;
            long double cost = 0.0L;
            for(std::size_t point = 0; point < point_count; ++point){
                cost = checked_cost_sum(
                    cost,
                    std::min(nearest[point], distance[point][candidate]),
                    "k-medoids objective overflowed"
                );
            }
            if(cost < best_cost || (cost == best_cost && candidate < best_candidate)){
                best_cost = cost;
                best_candidate = candidate;
            }
        }
        medoids.push_back(best_candidate);
        selected[best_candidate] = true;
        for(std::size_t point = 0; point < point_count; ++point){
            nearest[point] = std::min(
                nearest[point], distance[point][best_candidate]
            );
        }
    }

    long double current_cost = 0.0L;
    for(const long double value: nearest){
        current_cost = checked_cost_sum(
            current_cost, value, "k-medoids objective overflowed"
        );
    }
    std::size_t iterations = 0;
    for(; iterations < maximum_iterations; ++iterations){
        std::vector<std::size_t> nearest_position(point_count, 0);
        std::vector<long double> nearest_distance(point_count, 0.0L);
        std::vector<long double> second_distance(
            point_count, std::numeric_limits<long double>::infinity()
        );
        for(std::size_t point = 0; point < point_count; ++point){
            std::size_t best = 0;
            std::size_t second = cluster_count;
            for(std::size_t position = 1; position < cluster_count; ++position){
                const auto better = [&](std::size_t left, std::size_t right){
                    return distance[point][medoids[left]] <
                               distance[point][medoids[right]] ||
                           (distance[point][medoids[left]] ==
                                distance[point][medoids[right]] &&
                            medoids[left] < medoids[right]);
                };
                if(better(position, best)){
                    second = best;
                    best = position;
                }else if(second == cluster_count || better(position, second)){
                    second = position;
                }
            }
            nearest_position[point] = best;
            nearest_distance[point] = distance[point][medoids[best]];
            if(second != cluster_count){
                second_distance[point] = distance[point][medoids[second]];
            }
        }
        std::size_t best_position = cluster_count;
        std::size_t best_candidate = point_count;
        long double best_cost = current_cost;
        for(std::size_t position = 0; position < cluster_count; ++position){
            for(std::size_t candidate = 0; candidate < point_count; ++candidate){
                if(selected[candidate]) continue;
                long double cost = 0.0L;
                for(std::size_t point = 0; point < point_count; ++point){
                    const long double without_removed =
                        nearest_position[point] == position ?
                        second_distance[point] : nearest_distance[point];
                    cost = checked_cost_sum(
                        cost,
                        std::min(
                            without_removed, distance[point][candidate]
                        ),
                        "k-medoids objective overflowed"
                    );
                }
                if(cost < best_cost ||
                   (cost == best_cost && cost < current_cost &&
                    std::pair{medoids[position], candidate} <
                    std::pair{medoids[best_position], best_candidate})){
                    best_cost = cost;
                    best_position = position;
                    best_candidate = candidate;
                }
            }
        }
        if(best_position == cluster_count) break;
        selected[medoids[best_position]] = false;
        medoids[best_position] = best_candidate;
        selected[best_candidate] = true;
        current_cost = best_cost;
    }
    std::sort(medoids.begin(), medoids.end());
    auto [labels, cost] = medoid_assignment(distance, medoids);
    return {std::move(medoids), std::move(labels), cost, iterations};
}

}  // namespace detail

template<class Distance>
[[nodiscard]] KMedoidsResult pam_k_medoids(
    std::size_t point_count,
    std::size_t cluster_count,
    Distance distance,
    std::size_t maximum_iterations = 100
){
    detail::validate_cluster_count(point_count, cluster_count);
    return detail::pam_from_matrix(
        detail::make_distance_matrix(point_count, distance),
        cluster_count,
        maximum_iterations
    );
}

template<class Distance, class URBG>
[[nodiscard]] KMedoidsResult clara_k_medoids(
    std::size_t point_count,
    std::size_t cluster_count,
    Distance distance,
    std::size_t sample_size,
    std::size_t sample_count,
    URBG& random,
    std::size_t maximum_pam_iterations = 100
){
    detail::validate_cluster_count(point_count, cluster_count);
    if(sample_size < cluster_count || sample_size > point_count || sample_count == 0){
        throw std::invalid_argument(
            "CLARA requires k <= sample size <= n and a positive sample count"
        );
    }
    const auto full_distance = detail::make_distance_matrix(point_count, distance);
    std::vector<std::size_t> permutation(point_count);
    std::iota(permutation.begin(), permutation.end(), std::size_t{0});
    KMedoidsResult best;
    best.cost = std::numeric_limits<long double>::infinity();
    for(std::size_t sample_index = 0; sample_index < sample_count; ++sample_index){
        std::shuffle(permutation.begin(), permutation.end(), random);
        std::vector<std::size_t> sample(
            permutation.begin(),
            permutation.begin() + static_cast<std::ptrdiff_t>(sample_size)
        );
        std::sort(sample.begin(), sample.end());
        std::vector<std::vector<long double>> sample_distance(
            sample_size, std::vector<long double>(sample_size)
        );
        for(std::size_t left = 0; left < sample_size; ++left){
            for(std::size_t right = 0; right < sample_size; ++right){
                sample_distance[left][right] = full_distance[sample[left]][sample[right]];
            }
        }
        auto candidate = detail::pam_from_matrix(
            sample_distance, cluster_count, maximum_pam_iterations
        );
        for(std::size_t& medoid: candidate.medoids) medoid = sample[medoid];
        std::sort(candidate.medoids.begin(), candidate.medoids.end());
        auto [labels, cost] = detail::medoid_assignment(
            full_distance, candidate.medoids
        );
        candidate.labels = std::move(labels);
        candidate.cost = cost;
        if(candidate.cost < best.cost ||
           (candidate.cost == best.cost && candidate.medoids < best.medoids)){
            best = std::move(candidate);
        }
    }
    return best;
}

template<class Distance, class URBG>
[[nodiscard]] KMedoidsResult clarans_k_medoids(
    std::size_t point_count,
    std::size_t cluster_count,
    Distance distance,
    std::size_t maximum_neighbor_checks,
    std::size_t restart_count,
    URBG& random
){
    detail::validate_cluster_count(point_count, cluster_count);
    if(maximum_neighbor_checks == 0 || restart_count == 0){
        throw std::invalid_argument("CLARANS limits must be positive");
    }
    const auto matrix = detail::make_distance_matrix(point_count, distance);
    if(cluster_count == point_count){
        return detail::pam_from_matrix(matrix, cluster_count, 0);
    }
    std::vector<std::size_t> permutation(point_count);
    std::iota(permutation.begin(), permutation.end(), std::size_t{0});
    KMedoidsResult best;
    best.cost = std::numeric_limits<long double>::infinity();
    for(std::size_t restart = 0; restart < restart_count; ++restart){
        std::shuffle(permutation.begin(), permutation.end(), random);
        std::vector<std::size_t> medoids(
            permutation.begin(),
            permutation.begin() + static_cast<std::ptrdiff_t>(cluster_count)
        );
        std::sort(medoids.begin(), medoids.end());
        auto [ignored_labels, current_cost] = detail::medoid_assignment(matrix, medoids);
        (void)ignored_labels;
        std::size_t checked = 0;
        std::size_t accepted = 0;
        std::uniform_int_distribution<std::size_t> medoid_distribution(
            0, cluster_count - 1
        );
        std::uniform_int_distribution<std::size_t> nonmedoid_distribution(
            0, point_count - cluster_count - 1
        );
        while(checked < maximum_neighbor_checks){
            const std::size_t position = medoid_distribution(random);
            std::size_t rank = nonmedoid_distribution(random);
            std::size_t candidate = 0;
            for(; candidate < point_count; ++candidate){
                if(std::binary_search(
                    medoids.begin(), medoids.end(), candidate
                )) continue;
                if(rank == 0) break;
                --rank;
            }
            std::vector<std::size_t> trial = medoids;
            trial[position] = candidate;
            std::sort(trial.begin(), trial.end());
            const long double cost = detail::medoid_assignment(matrix, trial).second;
            if(cost < current_cost){
                medoids = std::move(trial);
                current_cost = cost;
                checked = 0;
                ++accepted;
            }else{
                ++checked;
            }
        }
        auto [labels, cost] = detail::medoid_assignment(matrix, medoids);
        KMedoidsResult result{
            std::move(medoids), std::move(labels), cost, accepted
        };
        if(result.cost < best.cost ||
           (result.cost == best.cost && result.medoids < best.medoids)){
            best = std::move(result);
        }
    }
    return best;
}

}  // namespace approximate::clustering
