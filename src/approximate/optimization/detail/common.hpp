#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_DETAIL_COMMON_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_DETAIL_COMMON_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "../optimization_result.hpp"

namespace approximate::optimization::detail {

template<class Real>
void validate_real_type(){
    static_assert(std::is_floating_point_v<Real>);
}

template<class Real>
void validate_point(const std::vector<Real>& point){
    validate_real_type<Real>();
    if(point.empty())[[unlikely]]{
        throw std::invalid_argument("the initial point must be non-empty");
    }
    for(const Real value: point){
        if(!std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument("the initial point must be finite");
        }
    }
}

template<class Real>
void validate_box(
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    std::size_t dimension
){
    if(lower.empty() && upper.empty()) return;
    if(lower.size() != dimension || upper.size() != dimension)[[unlikely]]{
        throw std::invalid_argument("bound dimensions do not match the point");
    }
    for(std::size_t index = 0; index < dimension; ++index){
        if(!std::isfinite(lower[index]) || !std::isfinite(upper[index])
           || upper[index] < lower[index])[[unlikely]]{
            throw std::invalid_argument("box bounds must be finite and ordered");
        }
    }
}

template<class Real>
void validate_problem(
    const std::vector<Real>& point,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
){
    validate_point(point);
    validate_box(lower, upper, point.size());
    if(lower.empty()) return;
    for(std::size_t index = 0; index < point.size(); ++index){
        if(point[index] < lower[index] || upper[index] < point[index])[[unlikely]]{
            throw std::invalid_argument("the initial point is outside the bounds");
        }
    }
}

template<class Real>
void validate_nonnegative(Real value, const char* message){
    if(value < Real{} || !std::isfinite(value))[[unlikely]]{
        throw std::invalid_argument(message);
    }
}

template<class Real>
void validate_positive(Real value, const char* message){
    if(!(Real{} < value) || !std::isfinite(value))[[unlikely]]{
        throw std::invalid_argument(message);
    }
}

inline void increment_count(std::size_t& count){
    if(count == std::numeric_limits<std::size_t>::max())[[unlikely]]{
        throw std::overflow_error("optimization evaluation count overflows size_t");
    }
    ++count;
}

template<class Real>
void project_and_validate(
    std::vector<Real>& point,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
){
    for(const Real value: point){
        if(!std::isfinite(value))[[unlikely]]{
            throw std::overflow_error("an optimization step produced a non-finite point");
        }
    }
    if(lower.empty()) return;
    for(std::size_t index = 0; index < point.size(); ++index){
        point[index] = std::clamp(point[index], lower[index], upper[index]);
    }
}

template<class Real, class Objective>
Real evaluate_objective(
    Objective& objective,
    const std::vector<Real>& point,
    std::size_t& evaluations
){
    static_assert(std::is_convertible_v<
        std::invoke_result_t<Objective&, const std::vector<Real>&>, Real
    >);
    const Real value = static_cast<Real>(std::invoke(objective, point));
    increment_count(evaluations);
    if(!std::isfinite(value))[[unlikely]]{
        throw std::domain_error("the objective returned a non-finite value");
    }
    return value;
}

template<class Real>
Real infinity_norm(const std::vector<Real>& values){
    Real result{};
    for(const Real value: values) result = std::max(result, std::abs(value));
    return result;
}

template<class Real>
void validate_gradient(
    const std::vector<Real>& gradient,
    std::size_t dimension
){
    if(gradient.size() != dimension)[[unlikely]]{
        throw std::invalid_argument("the gradient dimension does not match the point");
    }
    for(const Real value: gradient){
        if(!std::isfinite(value))[[unlikely]]{
            throw std::domain_error("the gradient returned a non-finite value");
        }
    }
}

template<class Real>
struct FirstOrderUpdate{
    std::vector<Real> point;
    std::vector<Real> gradient;
};

template<class Real, class Objective, class Step>
auto run_first_order(
    std::vector<Real> point,
    Objective objective,
    std::size_t max_iterations,
    Real tolerance,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Step step
){
    validate_problem(point, lower, upper);
    validate_nonnegative(tolerance, "tolerance must be finite and non-negative");
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    Real value = evaluate_objective<Real>(objective, point, evaluations);
    std::size_t iterations = 0;
    bool converged = false;
    while(iterations < max_iterations){
        auto update = std::invoke(step, point, iterations);
        increment_count(gradient_evaluations);
        validate_gradient(update.gradient, point.size());
        if(infinity_norm(update.gradient) <= tolerance){
            converged = true;
            break;
        }
        project_and_validate(update.point, lower, upper);
        point = std::move(update.point);
        value = evaluate_objective<Real>(objective, point, evaluations);
        ++iterations;
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations,
        gradient_evaluations, converged
    };
}

}  // namespace approximate::optimization::detail

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_DETAIL_COMMON_HPP_INCLUDED
