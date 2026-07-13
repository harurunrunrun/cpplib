#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/advanced_common.hpp"

namespace approximate::optimization {

template<class Real, class Objective>
auto hooke_jeeves(
    std::vector<Real> point,
    Objective objective,
    std::size_t max_iterations,
    std::size_t max_evaluations,
    Real initial_step = Real{1},
    Real reduction_factor = Real{0.5},
    Real pattern_factor = Real{1},
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_problem(point, lower, upper);
    detail::validate_evaluation_limit(max_evaluations);
    detail::validate_positive(
        initial_step, "initial_step must be finite and positive"
    );
    if(!(Real{} < reduction_factor) || !(reduction_factor < Real{1})
       || !std::isfinite(reduction_factor))[[unlikely]]{
        throw std::invalid_argument("reduction_factor must be in (0, 1)");
    }
    detail::validate_positive(
        pattern_factor, "pattern_factor must be finite and positive"
    );
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    std::size_t evaluations = 0;
    Real value = detail::evaluate_objective<Real>(objective, point, evaluations);
    Real step = initial_step;
    std::size_t iterations = 0;
    bool converged = step <= tolerance;
    while(iterations < max_iterations && !converged){
        if(!detail::has_evaluation_budget(evaluations, max_evaluations)) break;
        auto explored = point;
        Real explored_value = value;
        bool exhausted = false;
        for(std::size_t coordinate = 0; coordinate < point.size(); ++coordinate){
            bool improved = false;
            for(const Real sign: {Real{1}, Real{-1}}){
                auto candidate = explored;
                candidate[coordinate] = detail::checked_real<Real>(
                    static_cast<long double>(candidate[coordinate])
                        + static_cast<long double>(sign)
                            * static_cast<long double>(step),
                    "Hooke-Jeeves exploratory point overflowed"
                );
                detail::project_and_validate(candidate, lower, upper);
                if(candidate == explored) continue;
                if(!detail::has_evaluation_budget(
                    evaluations, max_evaluations
                )){
                    exhausted = true;
                    break;
                }
                const Real candidate_value = detail::evaluate_objective<Real>(
                    objective, candidate, evaluations
                );
                if(candidate_value < explored_value){
                    explored = std::move(candidate);
                    explored_value = candidate_value;
                    improved = true;
                    break;
                }
            }
            if(exhausted) break;
            static_cast<void>(improved);
        }
        if(explored_value < value){
            auto pattern = explored;
            for(std::size_t index = 0; index < point.size(); ++index){
                const long double displacement =
                    static_cast<long double>(explored[index])
                    - static_cast<long double>(point[index]);
                pattern[index] = detail::checked_real<Real>(
                    static_cast<long double>(explored[index])
                        + static_cast<long double>(pattern_factor) * displacement,
                    "Hooke-Jeeves pattern point overflowed"
                );
            }
            detail::project_and_validate(pattern, lower, upper);
            if(pattern != explored && detail::has_evaluation_budget(
                evaluations, max_evaluations
            )){
                const Real pattern_value = detail::evaluate_objective<Real>(
                    objective, pattern, evaluations
                );
                if(pattern_value < explored_value){
                    point = std::move(pattern);
                    value = pattern_value;
                }else{
                    point = std::move(explored);
                    value = explored_value;
                }
            }else{
                point = std::move(explored);
                value = explored_value;
                if(pattern != point) exhausted = true;
            }
        }else if(!exhausted){
            const Real next_step = step * reduction_factor;
            if(!std::isfinite(next_step))[[unlikely]]{
                throw std::overflow_error("Hooke-Jeeves step became non-finite");
            }
            if(next_step == step)[[unlikely]]{
                throw std::runtime_error("Hooke-Jeeves cannot reduce its step");
            }
            step = next_step;
            converged = step <= tolerance;
        }
        ++iterations;
        if(exhausted) break;
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations, 0, converged
    };
}

}  // namespace approximate::optimization
