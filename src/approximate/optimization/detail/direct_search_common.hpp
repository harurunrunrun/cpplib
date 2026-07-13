#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "advanced_common.hpp"

namespace approximate::optimization::detail {

template<class Real>
void validate_direct_line_parameters(
    Real initial_step,
    Real reduction,
    Real line_tolerance
){
    validate_positive(initial_step, "initial_step must be finite and positive");
    if(!(Real{} < reduction) || !(reduction < Real{1})
       || !std::isfinite(reduction))[[unlikely]]{
        throw std::invalid_argument("line_reduction must be in (0, 1)");
    }
    validate_positive(
        line_tolerance, "line_tolerance must be finite and positive"
    );
    if(initial_step < line_tolerance)[[unlikely]]{
        throw std::invalid_argument("line_tolerance must not exceed initial_step");
    }
}

template<class Real>
std::vector<Real> displaced_point(
    const std::vector<Real>& point,
    const std::vector<Real>& direction,
    Real distance,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
){
    std::vector<Real> result(point.size());
    for(std::size_t index = 0; index < point.size(); ++index){
        result[index] = checked_real<Real>(
            static_cast<long double>(point[index])
                + static_cast<long double>(distance)
                    * static_cast<long double>(direction[index]),
            "direct-search point overflowed"
        );
    }
    project_and_validate(result, lower, upper);
    return result;
}

template<class Real>
struct DirectionalSearchResult{
    std::vector<Real> point;
    Real value{};
    bool moved = false;
    bool evaluation_limit = false;
};

template<class Real, class Objective>
DirectionalSearchResult<Real> directional_pattern_search(
    std::vector<Real> point,
    Real value,
    const std::vector<Real>& direction,
    Objective& objective,
    std::size_t& evaluations,
    std::size_t max_evaluations,
    Real initial_step,
    Real reduction,
    Real line_tolerance,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
){
    Real step = initial_step;
    bool moved = false;
    while(line_tolerance < step){
        auto best_point = point;
        Real best_value = value;
        for(const Real sign: {Real{1}, Real{-1}}){
            auto candidate = displaced_point(
                point, direction, sign * step, lower, upper
            );
            if(candidate == point) continue;
            if(!has_evaluation_budget(evaluations, max_evaluations)){
                return {
                    std::move(best_point), best_value,
                    moved || best_value < value, true
                };
            }
            const Real candidate_value = evaluate_objective<Real>(
                objective, candidate, evaluations
            );
            if(candidate_value < best_value){
                best_value = candidate_value;
                best_point = std::move(candidate);
            }
        }
        if(best_value < value){
            point = std::move(best_point);
            value = best_value;
            moved = true;
            continue;
        }
        const Real next_step = step * reduction;
        if(!std::isfinite(next_step))[[unlikely]]{
            throw std::overflow_error("direct-search step became non-finite");
        }
        if(next_step == step)[[unlikely]]{
            throw std::runtime_error("directional line search cannot reduce its step");
        }
        step = next_step;
    }
    return {std::move(point), value, moved, false};
}

template<class Real>
std::vector<Real> point_displacement(
    const std::vector<Real>& point,
    const std::vector<Real>& origin
){
    std::vector<Real> result(point.size());
    for(std::size_t index = 0; index < point.size(); ++index){
        result[index] = checked_real<Real>(
            static_cast<long double>(point[index])
                - static_cast<long double>(origin[index]),
            "direct-search displacement overflowed"
        );
    }
    return result;
}

}  // namespace approximate::optimization::detail
