#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_POWELL_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_POWELL_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "detail/direct_search_common.hpp"

namespace approximate::optimization {

template<class Real, class Objective>
auto powell(
    std::vector<Real> point,
    Objective objective,
    std::size_t max_iterations,
    std::size_t max_evaluations,
    Real initial_step = Real{1},
    Real line_reduction = Real{0.5},
    Real line_tolerance = Real{1e-6},
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_problem(point, lower, upper);
    detail::validate_evaluation_limit(max_evaluations);
    detail::validate_direct_line_parameters(
        initial_step, line_reduction, line_tolerance
    );
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    std::size_t evaluations = 0;
    Real value = detail::evaluate_objective<Real>(objective, point, evaluations);
    const std::size_t dimension = point.size();
    std::vector<std::vector<Real>> directions(
        dimension, std::vector<Real>(dimension, Real{})
    );
    for(std::size_t index = 0; index < dimension; ++index){
        directions[index][index] = Real{1};
    }
    std::size_t iterations = 0;
    bool converged = false;
    while(iterations < max_iterations){
        if(!detail::has_evaluation_budget(evaluations, max_evaluations)) break;
        const auto cycle_start = point;
        const Real cycle_value = value;
        long double largest_decrease = 0;
        std::size_t largest_index = 0;
        bool exhausted = false;
        for(std::size_t index = 0; index < dimension; ++index){
            const Real previous_value = value;
            auto line = detail::directional_pattern_search(
                std::move(point), value, directions[index], objective,
                evaluations, max_evaluations, initial_step,
                line_reduction, line_tolerance, lower, upper
            );
            point = std::move(line.point);
            value = line.value;
            const long double decrease =
                static_cast<long double>(previous_value)
                - static_cast<long double>(value);
            if(!std::isfinite(decrease))[[unlikely]]{
                throw std::overflow_error("Powell objective decrease overflowed");
            }
            if(largest_decrease < decrease){
                largest_decrease = decrease;
                largest_index = index;
            }
            if(line.evaluation_limit){
                exhausted = true;
                break;
            }
        }
        ++iterations;
        if(exhausted) break;
        auto displacement = detail::point_displacement(point, cycle_start);
        const Real displacement_norm = detail::infinity_norm(displacement);
        const long double value_change = std::abs(
            static_cast<long double>(cycle_value)
                - static_cast<long double>(value)
        );
        if(!std::isfinite(value_change))[[unlikely]]{
            throw std::overflow_error("Powell objective change overflowed");
        }
        if(displacement_norm == Real{}
           || (displacement_norm <= tolerance
               && value_change <= static_cast<long double>(tolerance))){
            converged = true;
            break;
        }
        for(Real& coordinate: displacement){
            coordinate /= displacement_norm;
        }
        directions[largest_index] = directions.back();
        directions.back() = displacement;
        if(!detail::has_evaluation_budget(evaluations, max_evaluations)) break;
        auto line = detail::directional_pattern_search(
            std::move(point), value, directions.back(), objective,
            evaluations, max_evaluations, initial_step,
            line_reduction, line_tolerance, lower, upper
        );
        point = std::move(line.point);
        value = line.value;
        if(line.evaluation_limit) break;
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations, 0, converged
    };
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_POWELL_HPP_INCLUDED
