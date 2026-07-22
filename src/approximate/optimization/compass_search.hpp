#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_COMPASS_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_COMPASS_SEARCH_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

template<class Real, class Objective>
auto compass_search(
    std::vector<Real> point,
    Objective objective,
    std::size_t max_iterations,
    Real initial_step,
    Real reduction_factor = Real{0.5},
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_problem(point, lower, upper);
    detail::validate_positive(
        initial_step, "initial_step must be finite and positive"
    );
    if(!(Real{} < reduction_factor) || !(reduction_factor < Real{1})
       || !std::isfinite(reduction_factor))[[unlikely]]{
        throw std::invalid_argument("reduction_factor must be in (0, 1)");
    }
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    std::size_t evaluations = 0;
    Real value = detail::evaluate_objective<Real>(objective, point, evaluations);
    Real step = initial_step;
    std::size_t iterations = 0;
    bool converged = step <= tolerance;
    while(iterations < max_iterations && !converged){
        auto best_point = point;
        Real best_value = value;
        for(std::size_t coordinate = 0; coordinate < point.size(); ++coordinate){
            for(const Real direction: {Real{-1}, Real{1}}){
                auto candidate = point;
                candidate[coordinate] += direction * step;
                detail::project_and_validate(candidate, lower, upper);
                if(candidate == point) continue;
                const Real candidate_value = detail::evaluate_objective<Real>(
                    objective, candidate, evaluations
                );
                if(candidate_value < best_value){
                    best_value = candidate_value;
                    best_point = std::move(candidate);
                }
            }
        }
        if(best_value < value){
            point = std::move(best_point);
            value = best_value;
        }else{
            step *= reduction_factor;
            if(!std::isfinite(step))[[unlikely]]{
                throw std::overflow_error("compass-search step became non-finite");
            }
            converged = step <= tolerance;
        }
        ++iterations;
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations, 0, converged
    };
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_COMPASS_SEARCH_HPP_INCLUDED
