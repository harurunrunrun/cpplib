#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

template<class Real, class Objective, class CoordinateGradient>
auto coordinate_descent(
    std::vector<Real> point,
    Objective objective,
    CoordinateGradient coordinate_gradient,
    std::size_t max_sweeps,
    Real learning_rate,
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_problem(point, lower, upper);
    detail::validate_positive(
        learning_rate, "learning_rate must be finite and positive"
    );
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    Real value = detail::evaluate_objective<Real>(objective, point, evaluations);
    std::size_t sweeps = 0;
    bool converged = false;
    while(sweeps < max_sweeps){
        bool moved = false;
        for(std::size_t coordinate = 0; coordinate < point.size(); ++coordinate){
            static_assert(std::is_convertible_v<
                std::invoke_result_t<
                    CoordinateGradient&, const std::vector<Real>&, std::size_t
                >,
                Real
            >);
            const Real derivative = static_cast<Real>(std::invoke(
                coordinate_gradient, point, coordinate
            ));
            detail::increment_count(gradient_evaluations);
            if(!std::isfinite(derivative))[[unlikely]]{
                throw std::domain_error(
                    "the coordinate gradient returned a non-finite value"
                );
            }
            if(std::abs(derivative) <= tolerance) continue;
            Real candidate = point[coordinate] - learning_rate * derivative;
            if(!std::isfinite(candidate))[[unlikely]]{
                throw std::overflow_error(
                    "a coordinate-descent step produced a non-finite value"
                );
            }
            if(!lower.empty()){
                candidate = std::clamp(
                    candidate, lower[coordinate], upper[coordinate]
                );
            }
            if(candidate == point[coordinate]) continue;
            point[coordinate] = candidate;
            value = detail::evaluate_objective<Real>(objective, point, evaluations);
            moved = true;
        }
        if(!moved){
            converged = true;
            break;
        }
        ++sweeps;
    }
    return OptimizationResult<Real>{
        std::move(point), value, sweeps, evaluations,
        gradient_evaluations, converged
    };
}

}  // namespace approximate::optimization
