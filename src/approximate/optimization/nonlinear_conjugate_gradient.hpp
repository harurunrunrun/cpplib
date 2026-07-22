#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_NONLINEAR_CONJUGATE_GRADIENT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_NONLINEAR_CONJUGATE_GRADIENT_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/advanced_common.hpp"

namespace approximate::optimization {

template<class Real, class Objective, class Gradient>
auto nonlinear_conjugate_gradient(
    std::vector<Real> point,
    Objective objective,
    Gradient gradient,
    std::size_t max_iterations,
    std::size_t max_evaluations,
    Real initial_step = Real{1},
    Real tolerance = Real{1e-8},
    Real armijo = Real{1e-4},
    Real line_reduction = Real{0.5},
    Real minimum_step = Real{1e-12},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_problem(point, lower, upper);
    detail::validate_evaluation_limit(max_evaluations);
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    detail::validate_backtracking_parameters(
        initial_step, armijo, line_reduction, minimum_step
    );
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    Real value = detail::evaluate_objective<Real>(objective, point, evaluations);
    if(max_iterations == 0){
        return OptimizationResult<Real>{
            std::move(point), value, 0, evaluations, 0, false
        };
    }
    auto gradient_value = detail::evaluate_gradient<Real>(
        gradient, point, gradient_evaluations
    );
    bool converged = detail::infinity_norm(gradient_value) <= tolerance;
    auto direction = detail::negative_gradient(gradient_value);
    std::size_t iterations = 0;
    while(iterations < max_iterations && !converged){
        if(!detail::has_evaluation_budget(evaluations, max_evaluations)) break;
        if(!(detail::checked_dot(gradient_value, direction) < 0)){
            direction = detail::negative_gradient(gradient_value);
        }
        auto line = detail::armijo_backtracking(
            point, value, gradient_value, direction, objective,
            evaluations, max_evaluations, initial_step, armijo,
            line_reduction, minimum_step, lower, upper
        );
        if(line.status == detail::LineSearchStatus::evaluation_limit) break;
        if(line.status == detail::LineSearchStatus::no_feasible_descent){
            auto fallback = detail::negative_gradient(gradient_value);
            if(fallback == direction){
                converged = true;
                break;
            }
            direction = std::move(fallback);
            line = detail::armijo_backtracking(
                point, value, gradient_value, direction, objective,
                evaluations, max_evaluations, initial_step, armijo,
                line_reduction, minimum_step, lower, upper
            );
            if(line.status == detail::LineSearchStatus::evaluation_limit) break;
            if(line.status == detail::LineSearchStatus::no_feasible_descent){
                converged = true;
                break;
            }
        }
        auto next_gradient = detail::evaluate_gradient<Real>(
            gradient, line.point, gradient_evaluations
        );
        point = std::move(line.point);
        value = line.value;
        ++iterations;
        if(detail::infinity_norm(next_gradient) <= tolerance){
            gradient_value = std::move(next_gradient);
            converged = true;
            break;
        }
        std::vector<Real> gradient_change(next_gradient.size());
        for(std::size_t index = 0; index < next_gradient.size(); ++index){
            gradient_change[index] = detail::checked_real<Real>(
                static_cast<long double>(next_gradient[index])
                    - static_cast<long double>(gradient_value[index]),
                "nonlinear-CG gradient difference overflowed"
            );
        }
        const long double denominator = detail::checked_dot(
            gradient_value, gradient_value
        );
        long double beta = 0;
        if(0 < denominator){
            beta = std::max(
                0.0L,
                detail::checked_dot(next_gradient, gradient_change) / denominator
            );
        }
        if(!std::isfinite(beta))[[unlikely]]{
            throw std::overflow_error("Polak-Ribiere coefficient overflowed");
        }
        std::vector<Real> next_direction(direction.size());
        for(std::size_t index = 0; index < direction.size(); ++index){
            next_direction[index] = detail::checked_real<Real>(
                -static_cast<long double>(next_gradient[index])
                    + beta * static_cast<long double>(direction[index]),
                "nonlinear-CG direction overflowed"
            );
        }
        gradient_value = std::move(next_gradient);
        direction = std::move(next_direction);
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations,
        gradient_evaluations, converged
    };
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_NONLINEAR_CONJUGATE_GRADIENT_HPP_INCLUDED
