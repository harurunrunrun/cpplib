#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_STOCHASTIC_DESCENT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_STOCHASTIC_DESCENT_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <functional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

namespace stochastic_descent_internal {

template<class Real, class Objective, class GradientStep>
OptimizationResult<Real> run(
    std::vector<Real> point,
    Objective objective,
    std::size_t maximum_iterations,
    Real learning_rate,
    Real tolerance,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    GradientStep gradient_step
) {
    detail::validate_problem(point, lower, upper);
    detail::validate_positive(
        learning_rate, "learning rate must be positive and finite"
    );
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and nonnegative"
    );
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    Real value = detail::evaluate_objective<Real>(
        objective, point, evaluations
    );
    std::size_t iterations = 0;
    bool converged = false;
    while(iterations < maximum_iterations){
        std::vector<Real> gradient = std::invoke(
            gradient_step, point, gradient_evaluations
        );
        detail::validate_gradient(gradient, point.size());
        if(detail::infinity_norm(gradient) <= tolerance){
            converged = true;
            break;
        }
        for(std::size_t coordinate = 0;
            coordinate < point.size(); ++coordinate){
            const long double next =
                static_cast<long double>(point[coordinate])
                - static_cast<long double>(learning_rate)
                    * static_cast<long double>(gradient[coordinate]);
            point[coordinate] = static_cast<Real>(next);
        }
        detail::project_and_validate(point, lower, upper);
        value = detail::evaluate_objective<Real>(
            objective, point, evaluations
        );
        ++iterations;
    }
    return {
        std::move(point), value, iterations, evaluations,
        gradient_evaluations, converged
    };
}

}  // namespace stochastic_descent_internal

template<
    class Real,
    class Objective,
    class SampleGradient,
    class UniformRandomBitGenerator
>
[[nodiscard]] OptimizationResult<Real> stochastic_gradient_descent(
    std::vector<Real> point,
    Objective objective,
    SampleGradient sample_gradient,
    std::size_t sample_count,
    UniformRandomBitGenerator& generator,
    std::size_t maximum_iterations,
    Real learning_rate,
    Real tolerance = Real{},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
) {
    static_assert(std::is_floating_point_v<Real>);
    if(sample_count == 0){
        throw std::invalid_argument(
            "stochastic gradient descent requires a sample"
        );
    }
    std::uniform_int_distribution<std::size_t> choose_sample(
        0, sample_count - 1
    );
    return stochastic_descent_internal::run(
        std::move(point), std::move(objective), maximum_iterations,
        learning_rate, tolerance, lower, upper,
        [gradient = std::move(sample_gradient), &generator,
         choose_sample = std::move(choose_sample)](
            const std::vector<Real>& current,
            std::size_t& gradient_evaluations
        ) mutable {
            std::vector<Real> result = std::invoke(
                gradient, current, choose_sample(generator)
            );
            detail::increment_count(gradient_evaluations);
            return result;
        }
    );
}

template<
    class Real,
    class Objective,
    class SampleGradient,
    class UniformRandomBitGenerator
>
[[nodiscard]] OptimizationResult<Real> mini_batch_gradient_descent(
    std::vector<Real> point,
    Objective objective,
    SampleGradient sample_gradient,
    std::size_t sample_count,
    std::size_t batch_size,
    UniformRandomBitGenerator& generator,
    std::size_t maximum_iterations,
    Real learning_rate,
    Real tolerance = Real{},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
) {
    static_assert(std::is_floating_point_v<Real>);
    if(sample_count == 0 || batch_size == 0){
        throw std::invalid_argument(
            "mini-batch gradient descent requires samples and a batch"
        );
    }
    std::uniform_int_distribution<std::size_t> choose_sample(
        0, sample_count - 1
    );
    return stochastic_descent_internal::run(
        std::move(point), std::move(objective), maximum_iterations,
        learning_rate, tolerance, lower, upper,
        [gradient = std::move(sample_gradient), batch_size, &generator,
         choose_sample = std::move(choose_sample)](
            const std::vector<Real>& current,
            std::size_t& gradient_evaluations
        ) mutable {
            std::vector<Real> result(current.size(), Real{});
            for(std::size_t sample = 0; sample < batch_size; ++sample){
                std::vector<Real> contribution = std::invoke(
                    gradient, current, choose_sample(generator)
                );
                detail::increment_count(gradient_evaluations);
                detail::validate_gradient(
                    contribution, current.size()
                );
                for(std::size_t coordinate = 0;
                    coordinate < current.size(); ++coordinate){
                    const long double sum =
                        static_cast<long double>(result[coordinate])
                        + static_cast<long double>(
                            contribution[coordinate]
                        );
                    result[coordinate] = static_cast<Real>(sum);
                }
            }
            for(Real& value : result){
                value = static_cast<Real>(
                    static_cast<long double>(value)
                    / static_cast<long double>(batch_size)
                );
            }
            return result;
        }
    );
}

template<
    class Real,
    class Objective,
    class CoordinateGradient,
    class UniformRandomBitGenerator
>
[[nodiscard]] OptimizationResult<Real> stochastic_coordinate_descent(
    std::vector<Real> point,
    Objective objective,
    CoordinateGradient coordinate_gradient,
    UniformRandomBitGenerator& generator,
    std::size_t maximum_iterations,
    Real learning_rate,
    Real tolerance = Real{},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
) {
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_problem(point, lower, upper);
    detail::validate_positive(
        learning_rate, "learning rate must be positive and finite"
    );
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and nonnegative"
    );
    std::uniform_int_distribution<std::size_t> choose_coordinate(
        0, point.size() - 1
    );
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    Real value = detail::evaluate_objective<Real>(
        objective, point, evaluations
    );
    std::size_t iterations = 0;
    bool converged = false;
    while(iterations < maximum_iterations){
        const std::size_t coordinate = choose_coordinate(generator);
        const Real gradient = static_cast<Real>(std::invoke(
            coordinate_gradient, point, coordinate
        ));
        detail::increment_count(gradient_evaluations);
        if(!std::isfinite(gradient)){
            throw std::domain_error(
                "the coordinate gradient returned a non-finite value"
            );
        }
        if(std::abs(gradient) <= tolerance){
            converged = true;
            break;
        }
        const long double next =
            static_cast<long double>(point[coordinate])
            - static_cast<long double>(learning_rate)
                * static_cast<long double>(gradient);
        point[coordinate] = static_cast<Real>(next);
        detail::project_and_validate(point, lower, upper);
        value = detail::evaluate_objective<Real>(
            objective, point, evaluations
        );
        ++iterations;
    }
    return {
        std::move(point), value, iterations, evaluations,
        gradient_evaluations, converged
    };
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_STOCHASTIC_DESCENT_HPP_INCLUDED
