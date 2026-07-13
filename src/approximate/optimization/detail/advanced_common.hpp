#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common.hpp"

namespace approximate::optimization::detail {

inline void validate_evaluation_limit(std::size_t max_evaluations){
    if(max_evaluations == 0)[[unlikely]]{
        throw std::invalid_argument("max_evaluations must be positive");
    }
}

inline bool has_evaluation_budget(
    std::size_t evaluations,
    std::size_t max_evaluations,
    std::size_t required = 1
){
    return evaluations <= max_evaluations
        && required <= max_evaluations - evaluations;
}

template<class Real, class Gradient>
std::vector<Real> evaluate_gradient(
    Gradient& gradient,
    const std::vector<Real>& point,
    std::size_t& gradient_evaluations
){
    static_assert(std::is_convertible_v<
        std::invoke_result_t<Gradient&, const std::vector<Real>&>,
        std::vector<Real>
    >);
    std::vector<Real> value = std::invoke(gradient, point);
    increment_count(gradient_evaluations);
    validate_gradient(value, point.size());
    return value;
}

template<class Real>
long double checked_dot(
    const std::vector<Real>& left,
    const std::vector<Real>& right
){
    if(left.size() != right.size())[[unlikely]]{
        throw std::invalid_argument("dot-product dimensions do not match");
    }
    long double result = 0;
    for(std::size_t index = 0; index < left.size(); ++index){
        const long double term = static_cast<long double>(left[index])
            * static_cast<long double>(right[index]);
        if(!std::isfinite(term))[[unlikely]]{
            throw std::overflow_error("optimization dot product overflowed");
        }
        result += term;
        if(!std::isfinite(result))[[unlikely]]{
            throw std::overflow_error("optimization dot product overflowed");
        }
    }
    return result;
}

template<class Real>
Real checked_real(long double value, const char* message){
    const Real converted = static_cast<Real>(value);
    if(!std::isfinite(value) || !std::isfinite(converted))[[unlikely]]{
        throw std::overflow_error(message);
    }
    return converted;
}

template<class Real>
void validate_backtracking_parameters(
    Real initial_step,
    Real armijo,
    Real reduction,
    Real minimum_step
){
    validate_positive(initial_step, "initial_step must be finite and positive");
    if(!(Real{} < armijo) || !(armijo < Real{1}) || !std::isfinite(armijo))[[unlikely]]{
        throw std::invalid_argument("armijo must be in (0, 1)");
    }
    if(!(Real{} < reduction) || !(reduction < Real{1})
       || !std::isfinite(reduction))[[unlikely]]{
        throw std::invalid_argument("line_reduction must be in (0, 1)");
    }
    validate_positive(
        minimum_step, "minimum_step must be finite and positive"
    );
    if(initial_step < minimum_step)[[unlikely]]{
        throw std::invalid_argument("minimum_step must not exceed initial_step");
    }
}

enum class LineSearchStatus{
    accepted,
    evaluation_limit,
    no_feasible_descent,
};

template<class Real>
struct LineSearchResult{
    std::vector<Real> point;
    Real value{};
    LineSearchStatus status = LineSearchStatus::evaluation_limit;
};

template<class Real, class Objective>
LineSearchResult<Real> armijo_backtracking(
    const std::vector<Real>& point,
    Real value,
    const std::vector<Real>& gradient,
    const std::vector<Real>& direction,
    Objective& objective,
    std::size_t& evaluations,
    std::size_t max_evaluations,
    Real initial_step,
    Real armijo,
    Real reduction,
    Real minimum_step,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
){
    Real step = initial_step;
    bool evaluated_candidate = false;
    while(minimum_step <= step){
        std::vector<Real> candidate(point.size());
        for(std::size_t index = 0; index < point.size(); ++index){
            const long double coordinate = static_cast<long double>(point[index])
                + static_cast<long double>(step)
                    * static_cast<long double>(direction[index]);
            candidate[index] = checked_real<Real>(
                coordinate, "line-search point overflowed"
            );
        }
        project_and_validate(candidate, lower, upper);
        if(candidate != point){
            std::vector<Real> displacement(point.size());
            for(std::size_t index = 0; index < point.size(); ++index){
                displacement[index] = checked_real<Real>(
                    static_cast<long double>(candidate[index])
                        - static_cast<long double>(point[index]),
                    "line-search displacement overflowed"
                );
            }
            const long double slope = checked_dot(gradient, displacement);
            if(slope < 0){
                if(!has_evaluation_budget(evaluations, max_evaluations)){
                    return {{}, value, LineSearchStatus::evaluation_limit};
                }
                const Real candidate_value = evaluate_objective<Real>(
                    objective, candidate, evaluations
                );
                evaluated_candidate = true;
                const long double threshold = static_cast<long double>(value)
                    + static_cast<long double>(armijo) * slope;
                if(!std::isfinite(threshold))[[unlikely]]{
                    throw std::overflow_error("Armijo threshold overflowed");
                }
                if(static_cast<long double>(candidate_value) <= threshold){
                    return {
                        std::move(candidate), candidate_value,
                        LineSearchStatus::accepted
                    };
                }
            }
        }
        const Real next_step = step * reduction;
        if(!std::isfinite(next_step))[[unlikely]]{
            throw std::overflow_error("line-search step became non-finite");
        }
        if(next_step == step)[[unlikely]]{
            throw std::runtime_error("line search cannot reduce its step");
        }
        step = next_step;
    }
    if(!evaluated_candidate){
        return {{}, value, LineSearchStatus::no_feasible_descent};
    }
    throw std::runtime_error("Armijo line search failed");
}

template<class Real>
std::vector<Real> negative_gradient(const std::vector<Real>& gradient){
    std::vector<Real> direction(gradient.size());
    for(std::size_t index = 0; index < gradient.size(); ++index){
        direction[index] = -gradient[index];
        if(!std::isfinite(direction[index]))[[unlikely]]{
            throw std::overflow_error("negative gradient overflowed");
        }
    }
    return direction;
}

}  // namespace approximate::optimization::detail
