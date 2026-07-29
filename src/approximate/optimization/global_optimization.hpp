#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_GLOBAL_OPTIMIZATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_GLOBAL_OPTIMIZATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/advanced_common.hpp"

namespace approximate::optimization {

enum class BayesianAcquisition {
    expected_improvement,
    probability_of_improvement,
    upper_confidence_bound,
};

template<class Real>
struct GlobalOptimizationResult {
    std::vector<Real> point;
    Real value{};
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
};

[[nodiscard]] inline long double expected_improvement(
    long double mean,
    long double standard_deviation,
    long double best,
    long double exploration = 0.0L
) {
    if(!std::isfinite(mean) || !std::isfinite(standard_deviation)
       || !std::isfinite(best) || !std::isfinite(exploration)
       || standard_deviation < 0.0L || exploration < 0.0L){
        throw std::invalid_argument(
            "expected-improvement inputs must be finite and valid"
        );
    }
    const long double improvement = best - mean - exploration;
    if(standard_deviation == 0.0L){
        return std::max(0.0L, improvement);
    }
    const long double z = improvement / standard_deviation;
    const long double cdf =
        0.5L * std::erfc(-z / std::sqrt(2.0L));
    const long double density =
        std::exp(-0.5L * z * z)
        / std::sqrt(2.0L * std::acos(-1.0L));
    return improvement * cdf + standard_deviation * density;
}

[[nodiscard]] inline long double probability_of_improvement(
    long double mean,
    long double standard_deviation,
    long double best,
    long double exploration = 0.0L
) {
    if(!std::isfinite(mean) || !std::isfinite(standard_deviation)
       || !std::isfinite(best) || !std::isfinite(exploration)
       || standard_deviation < 0.0L || exploration < 0.0L){
        throw std::invalid_argument(
            "probability-of-improvement inputs must be finite and valid"
        );
    }
    const long double improvement = best - mean - exploration;
    if(standard_deviation == 0.0L){
        return improvement > 0.0L ? 1.0L : 0.0L;
    }
    return 0.5L * std::erfc(
        -improvement
            / (standard_deviation * std::sqrt(2.0L))
    );
}

[[nodiscard]] inline long double upper_confidence_bound(
    long double mean,
    long double standard_deviation,
    long double exploration
) {
    if(!std::isfinite(mean) || !std::isfinite(standard_deviation)
       || !std::isfinite(exploration)
       || standard_deviation < 0.0L || exploration < 0.0L){
        throw std::invalid_argument(
            "upper-confidence-bound inputs must be finite and valid"
        );
    }
    return mean + exploration * standard_deviation;
}

namespace global_optimization_internal {

template<class Real>
void validate_bounds(
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
) {
    detail::validate_real_type<Real>();
    if(lower.empty() || lower.size() != upper.size()){
        throw std::invalid_argument(
            "global optimization needs nonempty matching bounds"
        );
    }
    detail::validate_box(lower, upper, lower.size());
    for(std::size_t coordinate = 0;
        coordinate < lower.size(); ++coordinate){
        if(!(lower[coordinate] < upper[coordinate])){
            throw std::invalid_argument(
                "global-optimization bounds must have positive width"
            );
        }
    }
}

template<class Real, class UniformRandomBitGenerator>
std::vector<Real> uniform_point(
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    UniformRandomBitGenerator& generator
) {
    std::vector<Real> point(lower.size());
    for(std::size_t coordinate = 0;
        coordinate < point.size(); ++coordinate){
        std::uniform_real_distribution<Real> choose(
            lower[coordinate], upper[coordinate]
        );
        point[coordinate] = choose(generator);
    }
    return point;
}

inline long double squared_distance(
    const std::vector<long double>& first,
    const std::vector<long double>& second
) {
    long double result = 0.0L;
    for(std::size_t coordinate = 0;
        coordinate < first.size(); ++coordinate){
        const long double difference =
            first[coordinate] - second[coordinate];
        result += difference * difference;
    }
    if(!std::isfinite(result)){
        throw std::overflow_error(
            "Gaussian-process distance overflowed"
        );
    }
    return result;
}

inline long double rbf_kernel(
    const std::vector<long double>& first,
    const std::vector<long double>& second,
    long double length_scale
) {
    return std::exp(
        -0.5L * squared_distance(first, second)
            / (length_scale * length_scale)
    );
}

inline std::vector<std::vector<long double>> cholesky_factor(
    const std::vector<std::vector<long double>>& matrix
) {
    const std::size_t size = matrix.size();
    std::vector<std::vector<long double>> lower(
        size, std::vector<long double>(size, 0.0L)
    );
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column <= row; ++column){
            long double value = matrix[row][column];
            for(std::size_t index = 0; index < column; ++index){
                value -= lower[row][index] * lower[column][index];
            }
            if(row == column){
                if(!(value > 0.0L) || !std::isfinite(value)){
                    throw std::domain_error(
                        "Gaussian-process kernel is not positive definite"
                    );
                }
                lower[row][column] = std::sqrt(value);
            }else{
                lower[row][column] =
                    value / lower[column][column];
            }
        }
    }
    return lower;
}

inline std::vector<long double> solve_cholesky(
    const std::vector<std::vector<long double>>& factor,
    const std::vector<long double>& right_hand_side
) {
    const std::size_t size = right_hand_side.size();
    std::vector<long double> intermediate(size);
    for(std::size_t row = 0; row < size; ++row){
        long double value = right_hand_side[row];
        for(std::size_t column = 0; column < row; ++column){
            value -= factor[row][column] * intermediate[column];
        }
        intermediate[row] = value / factor[row][row];
    }
    std::vector<long double> result(size);
    for(std::size_t reverse = size; reverse > 0; --reverse){
        const std::size_t row = reverse - 1;
        long double value = intermediate[row];
        for(std::size_t column = row + 1;
            column < size; ++column){
            value -= factor[column][row] * result[column];
        }
        result[row] = value / factor[row][row];
    }
    return result;
}

inline std::pair<long double, long double> predict(
    const std::vector<std::vector<long double>>& samples,
    const std::vector<std::vector<long double>>& factor,
    const std::vector<long double>& alpha,
    const std::vector<long double>& point,
    long double length_scale
) {
    std::vector<long double> covariance(samples.size());
    for(std::size_t sample = 0;
        sample < samples.size(); ++sample){
        covariance[sample] = rbf_kernel(
            samples[sample], point, length_scale
        );
    }
    long double mean = 0.0L;
    for(std::size_t sample = 0;
        sample < samples.size(); ++sample){
        mean += covariance[sample] * alpha[sample];
    }
    std::vector<long double> forward(samples.size());
    for(std::size_t row = 0; row < samples.size(); ++row){
        long double value = covariance[row];
        for(std::size_t column = 0; column < row; ++column){
            value -= factor[row][column] * forward[column];
        }
        forward[row] = value / factor[row][row];
    }
    long double variance = 1.0L;
    for(const long double value : forward){
        variance -= value * value;
    }
    variance = std::max(0.0L, variance);
    return {mean, std::sqrt(variance)};
}

inline long double acquisition_score(
    BayesianAcquisition acquisition,
    long double mean,
    long double deviation,
    long double best,
    long double parameter
) {
    if(acquisition == BayesianAcquisition::expected_improvement){
        return expected_improvement(
            mean, deviation, best, parameter
        );
    }
    if(acquisition == BayesianAcquisition::probability_of_improvement){
        return probability_of_improvement(
            mean, deviation, best, parameter
        );
    }
    return -mean + parameter * deviation;
}

template<class Real, class Objective>
std::pair<std::vector<Real>, Real> local_pattern_refine(
    std::vector<Real> point,
    Real value,
    Objective& objective,
    std::size_t local_iterations,
    std::size_t maximum_evaluations,
    std::size_t& evaluations,
    Real initial_step,
    Real reduction,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
) {
    Real step = initial_step;
    for(std::size_t iteration = 0;
        iteration < local_iterations; ++iteration){
        bool improved = false;
        for(std::size_t coordinate = 0;
            coordinate < point.size(); ++coordinate){
            for(const Real sign : {Real{-1}, Real{1}}){
                if(!detail::has_evaluation_budget(
                       evaluations, maximum_evaluations)){
                    return {std::move(point), value};
                }
                std::vector<Real> candidate = point;
                candidate[coordinate] = static_cast<Real>(
                    static_cast<long double>(candidate[coordinate])
                    + static_cast<long double>(sign)
                        * static_cast<long double>(step)
                );
                detail::project_and_validate(candidate, lower, upper);
                if(candidate == point) continue;
                const Real candidate_value =
                    detail::evaluate_objective<Real>(
                        objective, candidate, evaluations
                    );
                if(candidate_value < value){
                    point = std::move(candidate);
                    value = candidate_value;
                    improved = true;
                }
            }
        }
        if(!improved) step *= reduction;
    }
    return {std::move(point), value};
}

}  // namespace global_optimization_internal

template<class Real, class Objective, class UniformRandomBitGenerator>
[[nodiscard]] GlobalOptimizationResult<Real> bayesian_optimization(
    Objective objective,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    UniformRandomBitGenerator& generator,
    std::size_t initial_samples,
    std::size_t iterations,
    std::size_t candidates_per_iteration,
    BayesianAcquisition acquisition =
        BayesianAcquisition::expected_improvement,
    Real acquisition_parameter = Real{0.01},
    Real length_scale = Real{1},
    Real observation_noise = Real{1e-8}
) {
    static_assert(std::is_floating_point_v<Real>);
    global_optimization_internal::validate_bounds(lower, upper);
    if(initial_samples == 0 || candidates_per_iteration == 0){
        throw std::invalid_argument(
            "Bayesian optimization requires samples and candidates"
        );
    }
    detail::validate_nonnegative(
        acquisition_parameter,
        "Bayesian acquisition parameter must be finite and nonnegative"
    );
    detail::validate_positive(
        length_scale,
        "Gaussian-process length scale must be positive and finite"
    );
    detail::validate_positive(
        observation_noise,
        "Gaussian-process observation noise must be positive and finite"
    );
    std::vector<std::vector<long double>> samples;
    std::vector<long double> values;
    samples.reserve(initial_samples + iterations);
    values.reserve(initial_samples + iterations);
    GlobalOptimizationResult<Real> result;
    result.value = std::numeric_limits<Real>::infinity();
    for(std::size_t sample = 0; sample < initial_samples; ++sample){
        std::vector<Real> point =
            global_optimization_internal::uniform_point(
                lower, upper, generator
            );
        const Real value = detail::evaluate_objective<Real>(
            objective, point, result.evaluations
        );
        samples.emplace_back(point.begin(), point.end());
        values.push_back(static_cast<long double>(value));
        if(value < result.value){
            result.point = std::move(point);
            result.value = value;
        }
    }
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const std::size_t size = samples.size();
        std::vector<std::vector<long double>> kernel(
            size, std::vector<long double>(size)
        );
        for(std::size_t row = 0; row < size; ++row){
            for(std::size_t column = 0; column <= row; ++column){
                const long double value =
                    global_optimization_internal::rbf_kernel(
                        samples[row], samples[column],
                        static_cast<long double>(length_scale)
                    );
                kernel[row][column] = value;
                kernel[column][row] = value;
            }
            kernel[row][row] +=
                static_cast<long double>(observation_noise);
        }
        const auto factor =
            global_optimization_internal::cholesky_factor(kernel);
        const auto alpha =
            global_optimization_internal::solve_cholesky(
                factor, values
            );
        std::vector<Real> selected;
        long double selected_score =
            -std::numeric_limits<long double>::infinity();
        for(std::size_t candidate = 0;
            candidate < candidates_per_iteration; ++candidate){
            std::vector<Real> point =
                global_optimization_internal::uniform_point(
                    lower, upper, generator
                );
            const std::vector<long double> normalized(
                point.begin(), point.end()
            );
            const auto [mean, deviation] =
                global_optimization_internal::predict(
                    samples, factor, alpha, normalized,
                    static_cast<long double>(length_scale)
                );
            const long double score =
                global_optimization_internal::acquisition_score(
                    acquisition, mean, deviation,
                    static_cast<long double>(result.value),
                    static_cast<long double>(acquisition_parameter)
                );
            if(selected.empty() || selected_score < score){
                selected = std::move(point);
                selected_score = score;
            }
        }
        const Real value = detail::evaluate_objective<Real>(
            objective, selected, result.evaluations
        );
        samples.emplace_back(selected.begin(), selected.end());
        values.push_back(static_cast<long double>(value));
        if(value < result.value){
            result.point = selected;
            result.value = value;
        }
        ++result.iterations;
    }
    return result;
}

template<class Real, class Objective, class UniformRandomBitGenerator>
[[nodiscard]] GlobalOptimizationResult<Real>
expected_improvement_optimization(
    Objective objective,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    UniformRandomBitGenerator& generator,
    std::size_t initial_samples,
    std::size_t iterations,
    std::size_t candidates_per_iteration,
    Real exploration = Real{0.01},
    Real length_scale = Real{1},
    Real observation_noise = Real{1e-8}
) {
    return bayesian_optimization(
        std::move(objective), lower, upper, generator,
        initial_samples, iterations, candidates_per_iteration,
        BayesianAcquisition::expected_improvement,
        exploration, length_scale, observation_noise
    );
}

template<class Real, class Objective, class UniformRandomBitGenerator>
[[nodiscard]] GlobalOptimizationResult<Real>
probability_of_improvement_optimization(
    Objective objective,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    UniformRandomBitGenerator& generator,
    std::size_t initial_samples,
    std::size_t iterations,
    std::size_t candidates_per_iteration,
    Real exploration = Real{0.01},
    Real length_scale = Real{1},
    Real observation_noise = Real{1e-8}
) {
    return bayesian_optimization(
        std::move(objective), lower, upper, generator,
        initial_samples, iterations, candidates_per_iteration,
        BayesianAcquisition::probability_of_improvement,
        exploration, length_scale, observation_noise
    );
}

template<class Real, class Objective, class UniformRandomBitGenerator>
[[nodiscard]] GlobalOptimizationResult<Real>
upper_confidence_bound_optimization(
    Objective objective,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    UniformRandomBitGenerator& generator,
    std::size_t initial_samples,
    std::size_t iterations,
    std::size_t candidates_per_iteration,
    Real exploration = Real{2},
    Real length_scale = Real{1},
    Real observation_noise = Real{1e-8}
) {
    return bayesian_optimization(
        std::move(objective), lower, upper, generator,
        initial_samples, iterations, candidates_per_iteration,
        BayesianAcquisition::upper_confidence_bound,
        exploration, length_scale, observation_noise
    );
}

template<class Real, class Objective>
[[nodiscard]] GlobalOptimizationResult<Real> direct_optimization(
    Objective objective,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    std::size_t maximum_iterations,
    std::size_t maximum_evaluations,
    Real exploration_weight = Real{1e-4}
) {
    static_assert(std::is_floating_point_v<Real>);
    global_optimization_internal::validate_bounds(lower, upper);
    detail::validate_evaluation_limit(maximum_evaluations);
    detail::validate_nonnegative(
        exploration_weight,
        "DIRECT exploration weight must be finite and nonnegative"
    );
    struct Box {
        std::vector<Real> center;
        std::vector<Real> half_width;
        Real value{};
    };
    Box initial;
    initial.center.resize(lower.size());
    initial.half_width.resize(lower.size());
    for(std::size_t coordinate = 0;
        coordinate < lower.size(); ++coordinate){
        initial.center[coordinate] = static_cast<Real>(
            (static_cast<long double>(lower[coordinate])
             + static_cast<long double>(upper[coordinate])) * 0.5L
        );
        initial.half_width[coordinate] = static_cast<Real>(
            (static_cast<long double>(upper[coordinate])
             - static_cast<long double>(lower[coordinate])) * 0.5L
        );
    }
    GlobalOptimizationResult<Real> result;
    initial.value = detail::evaluate_objective<Real>(
        objective, initial.center, result.evaluations
    );
    result.point = initial.center;
    result.value = initial.value;
    std::vector<Box> boxes;
    boxes.push_back(std::move(initial));
    for(std::size_t iteration = 0;
        iteration < maximum_iterations; ++iteration){
        if(!detail::has_evaluation_budget(
               result.evaluations, maximum_evaluations, 2)){
            break;
        }
        std::size_t selected = 0;
        long double selected_score =
            std::numeric_limits<long double>::infinity();
        for(std::size_t index = 0; index < boxes.size(); ++index){
            const Real radius = *std::max_element(
                boxes[index].half_width.begin(),
                boxes[index].half_width.end()
            );
            const long double score =
                static_cast<long double>(boxes[index].value)
                - static_cast<long double>(exploration_weight)
                    * static_cast<long double>(radius);
            if(score < selected_score){
                selected = index;
                selected_score = score;
            }
        }
        const Real longest = *std::max_element(
            boxes[selected].half_width.begin(),
            boxes[selected].half_width.end()
        );
        std::vector<std::size_t> dimensions;
        for(std::size_t coordinate = 0;
            coordinate < lower.size(); ++coordinate){
            if(boxes[selected].half_width[coordinate] == longest){
                dimensions.push_back(coordinate);
            }
        }
        for(const std::size_t coordinate : dimensions){
            if(!detail::has_evaluation_budget(
                   result.evaluations, maximum_evaluations, 2)){
                break;
            }
            const Real old_half =
                boxes[selected].half_width[coordinate];
            boxes[selected].half_width[coordinate] =
                old_half / Real{3};
            const Real shift = static_cast<Real>(
                static_cast<long double>(old_half) * (2.0L / 3.0L)
            );
            for(const Real sign : {Real{-1}, Real{1}}){
                Box child = boxes[selected];
                child.center[coordinate] = static_cast<Real>(
                    static_cast<long double>(
                        boxes[selected].center[coordinate]
                    ) + static_cast<long double>(sign)
                        * static_cast<long double>(shift)
                );
                child.center[coordinate] = std::clamp(
                    child.center[coordinate],
                    lower[coordinate], upper[coordinate]
                );
                child.value = detail::evaluate_objective<Real>(
                    objective, child.center, result.evaluations
                );
                if(child.value < result.value){
                    result.point = child.center;
                    result.value = child.value;
                }
                boxes.push_back(std::move(child));
            }
        }
        ++result.iterations;
    }
    return result;
}

template<class Real, class Objective, class UniformRandomBitGenerator>
[[nodiscard]] GlobalOptimizationResult<Real> basin_hopping(
    std::vector<Real> point,
    Objective objective,
    UniformRandomBitGenerator& generator,
    std::size_t basin_iterations,
    std::size_t local_iterations,
    std::size_t maximum_evaluations,
    Real perturbation,
    Real temperature,
    Real local_step = Real{1},
    Real local_reduction = Real{0.5},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
) {
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_problem(point, lower, upper);
    detail::validate_evaluation_limit(maximum_evaluations);
    detail::validate_positive(
        perturbation,
        "basin-hopping perturbation must be positive and finite"
    );
    detail::validate_nonnegative(
        temperature,
        "basin-hopping temperature must be finite and nonnegative"
    );
    detail::validate_positive(
        local_step,
        "basin-hopping local step must be positive and finite"
    );
    if(!(local_reduction > Real{})
       || !(local_reduction < Real{1})
       || !std::isfinite(local_reduction)){
        throw std::invalid_argument(
            "basin-hopping local reduction must be in (0, 1)"
        );
    }
    GlobalOptimizationResult<Real> result;
    Real value = detail::evaluate_objective<Real>(
        objective, point, result.evaluations
    );
    std::tie(point, value) =
        global_optimization_internal::local_pattern_refine(
            std::move(point), value, objective, local_iterations,
            maximum_evaluations, result.evaluations,
            local_step, local_reduction, lower, upper
        );
    result.point = point;
    result.value = value;
    std::normal_distribution<Real> displacement(Real{}, perturbation);
    std::uniform_real_distribution<Real> uniform(Real{}, Real{1});
    for(std::size_t iteration = 0;
        iteration < basin_iterations; ++iteration){
        if(!detail::has_evaluation_budget(
               result.evaluations, maximum_evaluations)){
            break;
        }
        std::vector<Real> candidate = point;
        for(Real& coordinate : candidate){
            coordinate += displacement(generator);
        }
        detail::project_and_validate(candidate, lower, upper);
        Real candidate_value = detail::evaluate_objective<Real>(
            objective, candidate, result.evaluations
        );
        std::tie(candidate, candidate_value) =
            global_optimization_internal::local_pattern_refine(
                std::move(candidate), candidate_value, objective,
                local_iterations, maximum_evaluations,
                result.evaluations, local_step, local_reduction,
                lower, upper
            );
        const long double difference =
            static_cast<long double>(candidate_value)
            - static_cast<long double>(value);
        bool accept = difference <= 0.0L;
        if(!accept && temperature > Real{}){
            const long double probability = std::exp(
                -difference / static_cast<long double>(temperature)
            );
            accept =
                static_cast<long double>(uniform(generator)) < probability;
        }
        if(accept){
            point = candidate;
            value = candidate_value;
        }
        if(candidate_value < result.value){
            result.point = std::move(candidate);
            result.value = candidate_value;
        }
        ++result.iterations;
    }
    return result;
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_GLOBAL_OPTIMIZATION_HPP_INCLUDED
