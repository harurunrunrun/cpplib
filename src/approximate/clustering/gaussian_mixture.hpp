#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"
#include "detail/stable_numeric.hpp"

namespace approximate::clustering {

struct GaussianMixtureResult {
    std::vector<long double> weights;
    DensePoints<long double> means;
    DensePoints<long double> variances;
    std::vector<std::vector<long double>> responsibilities;
    std::vector<std::size_t> labels;
    long double log_likelihood = 0.0L;
    std::size_t iterations = 0;
};

namespace detail {

template<class Real>
[[nodiscard]] DensePoint<long double> global_diagonal_variance(
    const DensePoints<Real>& points,
    const DensePoint<long double>& mean,
    long double minimum_variance
){
    DensePoint<long double> variance(mean.size(), minimum_variance);
    for(std::size_t coordinate = 0; coordinate < mean.size(); ++coordinate){
        long double scale = 0.0L;
        for(const auto& point : points){
            scale = std::max(scale, stable_absolute_difference(
                static_cast<long double>(point[coordinate]), mean[coordinate]
            ));
        }
        if(scale == 0.0L) continue;
        long double normalized_mean = 0.0L;
        for(std::size_t point = 0; point < points.size(); ++point){
            const long double ratio = stable_absolute_difference(
                static_cast<long double>(points[point][coordinate]), mean[coordinate]
            ) / scale;
            const long double value = ratio * ratio;
            normalized_mean += (value - normalized_mean) / static_cast<long double>(point + 1);
        }
        const long double scaled = scale * std::sqrt(normalized_mean);
        const long double value = checked_nonnegative_product(
            scaled, scaled, "Gaussian mixture variance overflowed"
        );
        variance[coordinate] = std::max(value, minimum_variance);
    }
    return variance;
}

struct GMMExpectationResult {
    std::vector<std::vector<long double>> responsibility;
    long double log_likelihood = 0.0L;
};

[[nodiscard]] inline GMMExpectationResult gmm_expectation(
    const DensePoints<long double>& points,
    const std::vector<long double>& weights,
    const DensePoints<long double>& means,
    const DensePoints<long double>& variances
){
    const std::size_t point_count = points.size();
    const std::size_t component_count = weights.size();
    const std::size_t dimension = points.front().size();
    const long double log_two_pi = std::log(2.0L * std::numbers::pi_v<long double>);
    GMMExpectationResult result;
    result.responsibility.assign(point_count, std::vector<long double>(component_count, 0.0L));
    std::vector<long double> log_probability(component_count);
    for(std::size_t point = 0; point < point_count; ++point){
        long double maximum_log_probability = -std::numeric_limits<long double>::infinity();
        for(std::size_t component = 0; component < component_count; ++component){
            long double log_determinant = 0.0L;
            long double quadratic = 0.0L;
            bool quadratic_overflow = false;
            for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                log_determinant = checked_finite_sum(
                    log_determinant,
                    std::log(variances[component][coordinate]),
                    "Gaussian mixture log determinant overflowed"
                );
                const long double standard_deviation = std::sqrt(variances[component][coordinate]);
                const long double ratio = stable_absolute_difference(
                    points[point][coordinate], means[component][coordinate]
                ) / standard_deviation;
                if(ratio > std::sqrt(std::numeric_limits<long double>::max())){
                    quadratic_overflow = true;
                    break;
                }
                const long double term = ratio * ratio;
                if(term > std::numeric_limits<long double>::max() - quadratic){
                    quadratic_overflow = true;
                    break;
                }
                quadratic += term;
            }
            if(quadratic_overflow){
                log_probability[component] = -std::numeric_limits<long double>::infinity();
                continue;
            }
            long double exponent = static_cast<long double>(dimension) * log_two_pi;
            exponent = checked_finite_sum(
                exponent, log_determinant, "Gaussian mixture exponent overflowed"
            );
            exponent = checked_finite_sum(
                exponent, quadratic, "Gaussian mixture exponent overflowed"
            );
            log_probability[component] = std::log(weights[component]) - 0.5L * exponent;
            if(!std::isfinite(log_probability[component])){
                throw std::overflow_error("Gaussian mixture log probability overflowed");
            }
            maximum_log_probability = std::max(maximum_log_probability, log_probability[component]);
        }
        if(!std::isfinite(maximum_log_probability)){
            throw std::overflow_error("all Gaussian mixture probabilities underflowed");
        }
        long double exponential_sum = 0.0L;
        for(std::size_t component = 0; component < component_count; ++component){
            const long double value = std::exp(log_probability[component] - maximum_log_probability);
            exponential_sum += value;
            result.responsibility[point][component] = value;
        }
        const long double log_normalizer = maximum_log_probability + std::log(exponential_sum);
        if(!std::isfinite(log_normalizer)){
            throw std::overflow_error("Gaussian mixture log likelihood overflowed");
        }
        result.log_likelihood = checked_finite_sum(
            result.log_likelihood, log_normalizer, "Gaussian mixture log likelihood overflowed"
        );
        for(long double& value : result.responsibility[point]) value /= exponential_sum;
    }
    return result;
}

template<class Real>
[[nodiscard]] DensePoint<long double> weighted_diagonal_variance(
    const DensePoints<Real>& points,
    const std::vector<std::vector<long double>>& responsibility,
    std::size_t component,
    long double effective_count,
    const DensePoint<long double>& mean,
    long double minimum_variance
){
    DensePoint<long double> variance(mean.size(), minimum_variance);
    for(std::size_t coordinate = 0; coordinate < mean.size(); ++coordinate){
        long double scale = 0.0L;
        for(std::size_t point = 0; point < points.size(); ++point){
            if(responsibility[point][component] == 0.0L) continue;
            scale = std::max(scale, stable_absolute_difference(
                static_cast<long double>(points[point][coordinate]), mean[coordinate]
            ));
        }
        if(scale == 0.0L) continue;
        long double normalized_variance = 0.0L;
        for(std::size_t point = 0; point < points.size(); ++point){
            const long double ratio = stable_absolute_difference(
                static_cast<long double>(points[point][coordinate]), mean[coordinate]
            ) / scale;
            normalized_variance += (responsibility[point][component] / effective_count) * ratio * ratio;
        }
        const long double scaled = scale * std::sqrt(normalized_variance);
        const long double value = checked_nonnegative_product(
            scaled, scaled, "Gaussian mixture variance overflowed"
        );
        variance[coordinate] = std::max(value, minimum_variance);
    }
    return variance;
}

}  // namespace detail

template<class Real, class UniformRandomBitGenerator>
[[nodiscard]] GaussianMixtureResult gaussian_mixture_em(
    const DensePoints<Real>& points,
    std::size_t component_count,
    UniformRandomBitGenerator& generator,
    std::size_t maximum_iterations = 100,
    long double tolerance = 1.0e-7L,
    long double minimum_variance = 1.0e-9L
){
    detail::validate_dense_points(points);
    detail::validate_cluster_count(points.size(), component_count);
    if(!std::isfinite(tolerance) || tolerance < 0.0L
       || !std::isfinite(minimum_variance) || minimum_variance <= 0.0L){
        throw std::invalid_argument("invalid Gaussian mixture tolerance or variance floor");
    }
    const std::size_t point_count = points.size();
    const std::size_t dimension = points.front().size();
    DensePoints<long double> converted_points;
    converted_points.reserve(point_count);
    for(const auto& point : points) converted_points.emplace_back(point.begin(), point.end());

    std::vector<std::size_t> order(point_count);
    std::iota(order.begin(), order.end(), 0);
    std::shuffle(order.begin(), order.end(), generator);
    GaussianMixtureResult result;
    result.weights.assign(component_count, 1.0L / static_cast<long double>(component_count));
    result.means.reserve(component_count);
    for(std::size_t component = 0; component < component_count; ++component){
        result.means.push_back(converted_points[order[component]]);
    }
    DensePoint<long double> global_mean(dimension, 0.0L);
    for(std::size_t point = 0; point < point_count; ++point){
        const long double rate = 1.0L / static_cast<long double>(point + 1);
        for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
            global_mean[coordinate] = std::lerp(
                global_mean[coordinate], converted_points[point][coordinate], rate
            );
        }
    }
    const DensePoint<long double> global_variance = detail::global_diagonal_variance(
        converted_points, global_mean, minimum_variance
    );
    result.variances.assign(component_count, global_variance);

    long double previous_log_likelihood = -std::numeric_limits<long double>::infinity();
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        detail::GMMExpectationResult expectation = detail::gmm_expectation(
            converted_points, result.weights, result.means, result.variances
        );
        if(std::isfinite(previous_log_likelihood)){
            const long double scale = std::max(1.0L, std::abs(previous_log_likelihood));
            const long double relative_change = detail::stable_absolute_difference(
                expectation.log_likelihood, previous_log_likelihood
            ) / scale;
            if(relative_change <= tolerance){
                result.iterations = iteration;
                break;
            }
        }
        previous_log_likelihood = expectation.log_likelihood;
        std::vector<long double> effective_count(component_count, 0.0L);
        for(const auto& row : expectation.responsibility){
            for(std::size_t component = 0; component < component_count; ++component){
                effective_count[component] += row[component];
            }
        }
        std::vector<long double> raw_weight(component_count, 0.0L);
        for(std::size_t component = 0; component < component_count; ++component){
            if(effective_count[component] <= std::numeric_limits<long double>::min()){
                result.means[component] = converted_points[component % point_count];
                result.variances[component] = global_variance;
                raw_weight[component] = 1.0L / static_cast<long double>(point_count);
                continue;
            }
            DensePoint<long double> mean(dimension, 0.0L);
            long double accumulated = 0.0L;
            for(std::size_t point = 0; point < point_count; ++point){
                const long double value = expectation.responsibility[point][component];
                if(value == 0.0L) continue;
                accumulated += value;
                const long double rate = value / accumulated;
                for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                    mean[coordinate] = std::lerp(
                        mean[coordinate], converted_points[point][coordinate], rate
                    );
                }
            }
            result.means[component] = std::move(mean);
            result.variances[component] = detail::weighted_diagonal_variance(
                converted_points,
                expectation.responsibility,
                component,
                effective_count[component],
                result.means[component],
                minimum_variance
            );
            raw_weight[component] = effective_count[component] / static_cast<long double>(point_count);
            if(raw_weight[component] == 0.0L){
                raw_weight[component] = std::numeric_limits<long double>::min();
            }
        }
        long double weight_sum = 0.0L;
        for(const long double weight : raw_weight) weight_sum += weight;
        for(std::size_t component = 0; component < component_count; ++component){
            result.weights[component] = raw_weight[component] / weight_sum;
        }
        result.iterations = iteration + 1;
    }

    detail::GMMExpectationResult final_expectation = detail::gmm_expectation(
        converted_points, result.weights, result.means, result.variances
    );
    result.log_likelihood = final_expectation.log_likelihood;
    result.responsibilities = std::move(final_expectation.responsibility);
    result.labels.resize(point_count);
    for(std::size_t point = 0; point < point_count; ++point){
        result.labels[point] = static_cast<std::size_t>(std::max_element(
            result.responsibilities[point].begin(), result.responsibilities[point].end()
        ) - result.responsibilities[point].begin());
    }
    return result;
}

}  // namespace approximate::clustering
