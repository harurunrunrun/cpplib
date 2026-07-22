#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_QUASI_NEWTON_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_QUASI_NEWTON_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/advanced_common.hpp"

namespace approximate::optimization {
namespace detail {

template<class Real>
std::vector<std::vector<Real>> identity_matrix(std::size_t dimension){
    std::vector<std::vector<Real>> result(
        dimension, std::vector<Real>(dimension, Real{})
    );
    for(std::size_t index = 0; index < dimension; ++index){
        result[index][index] = Real{1};
    }
    return result;
}

template<class Real>
std::vector<Real> negative_matrix_vector(
    const std::vector<std::vector<Real>>& matrix,
    const std::vector<Real>& vector
){
    std::vector<Real> result(vector.size());
    for(std::size_t row = 0; row < vector.size(); ++row){
        long double value = 0;
        for(std::size_t column = 0; column < vector.size(); ++column){
            value += static_cast<long double>(matrix[row][column])
                * static_cast<long double>(vector[column]);
            if(!std::isfinite(value))[[unlikely]]{
                throw std::overflow_error("quasi-Newton direction overflowed");
            }
        }
        result[row] = checked_real<Real>(
            -value, "quasi-Newton direction overflowed"
        );
    }
    return result;
}

template<class Real>
bool sufficient_curvature(
    long double ys,
    const std::vector<Real>& step,
    const std::vector<Real>& gradient_difference
){
    if(!(0 < ys)) return false;
    const long double step_norm = std::sqrt(checked_dot(step, step));
    const long double difference_norm = std::sqrt(
        checked_dot(gradient_difference, gradient_difference)
    );
    const long double threshold = std::numeric_limits<Real>::epsilon()
        * std::max(1.0L, step_norm * difference_norm);
    if(!std::isfinite(threshold))[[unlikely]]{
        throw std::overflow_error("quasi-Newton curvature test overflowed");
    }
    return threshold < ys;
}

template<class Real>
void bfgs_inverse_update(
    std::vector<std::vector<Real>>& inverse_hessian,
    const std::vector<Real>& step,
    const std::vector<Real>& gradient_difference,
    long double ys
){
    const std::size_t dimension = step.size();
    std::vector<Real> h_y(dimension);
    for(std::size_t row = 0; row < dimension; ++row){
        long double value = 0;
        for(std::size_t column = 0; column < dimension; ++column){
            value += static_cast<long double>(inverse_hessian[row][column])
                * static_cast<long double>(gradient_difference[column]);
            if(!std::isfinite(value))[[unlikely]]{
                throw std::overflow_error("BFGS inverse-Hessian update overflowed");
            }
        }
        h_y[row] = checked_real<Real>(
            value, "BFGS inverse-Hessian update overflowed"
        );
    }
    const long double y_h_y = checked_dot(gradient_difference, h_y);
    const long double coefficient = (1.0L + y_h_y / ys) / ys;
    if(!std::isfinite(coefficient))[[unlikely]]{
        throw std::overflow_error("BFGS inverse-Hessian update overflowed");
    }
    for(std::size_t row = 0; row < dimension; ++row){
        for(std::size_t column = 0; column < dimension; ++column){
            const long double value =
                static_cast<long double>(inverse_hessian[row][column])
                + coefficient * static_cast<long double>(step[row])
                    * static_cast<long double>(step[column])
                - (static_cast<long double>(h_y[row])
                        * static_cast<long double>(step[column])
                    + static_cast<long double>(step[row])
                        * static_cast<long double>(h_y[column])) / ys;
            inverse_hessian[row][column] = checked_real<Real>(
                value, "BFGS inverse-Hessian update overflowed"
            );
        }
    }
}

template<class Real>
struct LbfgsPair{
    std::vector<Real> step;
    std::vector<Real> gradient_difference;
    long double inverse_curvature{};
};

template<class Real>
std::vector<Real> lbfgs_direction(
    const std::vector<Real>& gradient,
    const std::vector<LbfgsPair<Real>>& history
){
    std::vector<Real> work = gradient;
    std::vector<long double> alpha(history.size());
    for(std::size_t reverse = history.size(); reverse > 0; --reverse){
        const std::size_t index = reverse - 1;
        alpha[index] = history[index].inverse_curvature
            * checked_dot(history[index].step, work);
        if(!std::isfinite(alpha[index]))[[unlikely]]{
            throw std::overflow_error("L-BFGS two-loop recursion overflowed");
        }
        for(std::size_t coordinate = 0; coordinate < work.size(); ++coordinate){
            const long double value = static_cast<long double>(work[coordinate])
                - alpha[index] * static_cast<long double>(
                    history[index].gradient_difference[coordinate]
                );
            work[coordinate] = checked_real<Real>(
                value, "L-BFGS two-loop recursion overflowed"
            );
        }
    }
    long double scale = 1;
    if(!history.empty()){
        const auto& last = history.back();
        const long double ys = 1.0L / last.inverse_curvature;
        const long double yy = checked_dot(
            last.gradient_difference, last.gradient_difference
        );
        if(0 < yy) scale = ys / yy;
    }
    for(Real& value: work){
        value = checked_real<Real>(
            scale * static_cast<long double>(value),
            "L-BFGS two-loop recursion overflowed"
        );
    }
    for(std::size_t index = 0; index < history.size(); ++index){
        const long double beta = history[index].inverse_curvature
            * checked_dot(history[index].gradient_difference, work);
        if(!std::isfinite(beta))[[unlikely]]{
            throw std::overflow_error("L-BFGS two-loop recursion overflowed");
        }
        for(std::size_t coordinate = 0; coordinate < work.size(); ++coordinate){
            const long double value = static_cast<long double>(work[coordinate])
                + (alpha[index] - beta)
                    * static_cast<long double>(history[index].step[coordinate]);
            work[coordinate] = checked_real<Real>(
                value, "L-BFGS two-loop recursion overflowed"
            );
        }
    }
    for(Real& value: work) value = -value;
    project_and_validate(work, {}, {});
    return work;
}

template<class Real>
std::vector<Real> difference(
    const std::vector<Real>& left,
    const std::vector<Real>& right,
    const char* message
){
    std::vector<Real> result(left.size());
    for(std::size_t index = 0; index < left.size(); ++index){
        result[index] = checked_real<Real>(
            static_cast<long double>(left[index])
                - static_cast<long double>(right[index]),
            message
        );
    }
    return result;
}

}  // namespace detail

template<class Real, class Objective, class Gradient>
auto bfgs(
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
    std::size_t iterations = 0;
    bool converged = false;
    if(max_iterations == 0){
        return OptimizationResult<Real>{
            std::move(point), value, 0, evaluations, 0, false
        };
    }
    auto gradient_value = detail::evaluate_gradient<Real>(
        gradient, point, gradient_evaluations
    );
    if(detail::infinity_norm(gradient_value) <= tolerance) converged = true;
    auto inverse_hessian = detail::identity_matrix<Real>(point.size());
    while(iterations < max_iterations && !converged){
        if(!detail::has_evaluation_budget(evaluations, max_evaluations)) break;
        auto direction = detail::negative_matrix_vector(
            inverse_hessian, gradient_value
        );
        if(!(detail::checked_dot(gradient_value, direction) < 0)){
            inverse_hessian = detail::identity_matrix<Real>(point.size());
            direction = detail::negative_gradient(gradient_value);
        }
        auto line = detail::armijo_backtracking(
            point, value, gradient_value, direction, objective,
            evaluations, max_evaluations, initial_step, armijo,
            line_reduction, minimum_step, lower, upper
        );
        if(line.status == detail::LineSearchStatus::evaluation_limit) break;
        if(line.status == detail::LineSearchStatus::no_feasible_descent){
            inverse_hessian = detail::identity_matrix<Real>(point.size());
            auto fallback = detail::negative_gradient(gradient_value);
            if(fallback == direction){
                converged = true;
                break;
            }
            line = detail::armijo_backtracking(
                point, value, gradient_value, fallback, objective,
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
        auto step = detail::difference(
            line.point, point, "BFGS step overflowed"
        );
        auto gradient_difference = detail::difference(
            next_gradient, gradient_value, "BFGS gradient difference overflowed"
        );
        point = std::move(line.point);
        value = line.value;
        ++iterations;
        if(detail::infinity_norm(next_gradient) <= tolerance){
            gradient_value = std::move(next_gradient);
            converged = true;
            break;
        }
        const long double ys = detail::checked_dot(
            gradient_difference, step
        );
        if(detail::sufficient_curvature(ys, step, gradient_difference)){
            detail::bfgs_inverse_update(
                inverse_hessian, step, gradient_difference, ys
            );
        }
        gradient_value = std::move(next_gradient);
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations,
        gradient_evaluations, converged
    };
}

template<class Real, class Objective, class Gradient>
auto lbfgs(
    std::vector<Real> point,
    Objective objective,
    Gradient gradient,
    std::size_t max_iterations,
    std::size_t max_evaluations,
    std::size_t history_size = 10,
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
    if(history_size == 0)[[unlikely]]{
        throw std::invalid_argument("history_size must be positive");
    }
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    detail::validate_backtracking_parameters(
        initial_step, armijo, line_reduction, minimum_step
    );
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    Real value = detail::evaluate_objective<Real>(objective, point, evaluations);
    std::size_t iterations = 0;
    bool converged = false;
    if(max_iterations == 0){
        return OptimizationResult<Real>{
            std::move(point), value, 0, evaluations, 0, false
        };
    }
    auto gradient_value = detail::evaluate_gradient<Real>(
        gradient, point, gradient_evaluations
    );
    if(detail::infinity_norm(gradient_value) <= tolerance) converged = true;
    std::vector<detail::LbfgsPair<Real>> history;
    history.reserve(history_size);
    while(iterations < max_iterations && !converged){
        if(!detail::has_evaluation_budget(evaluations, max_evaluations)) break;
        auto direction = detail::lbfgs_direction(gradient_value, history);
        if(!(detail::checked_dot(gradient_value, direction) < 0)){
            history.clear();
            direction = detail::negative_gradient(gradient_value);
        }
        auto line = detail::armijo_backtracking(
            point, value, gradient_value, direction, objective,
            evaluations, max_evaluations, initial_step, armijo,
            line_reduction, minimum_step, lower, upper
        );
        if(line.status == detail::LineSearchStatus::evaluation_limit) break;
        if(line.status == detail::LineSearchStatus::no_feasible_descent){
            history.clear();
            auto fallback = detail::negative_gradient(gradient_value);
            if(fallback == direction){
                converged = true;
                break;
            }
            line = detail::armijo_backtracking(
                point, value, gradient_value, fallback, objective,
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
        auto step = detail::difference(
            line.point, point, "L-BFGS step overflowed"
        );
        auto gradient_difference = detail::difference(
            next_gradient, gradient_value, "L-BFGS gradient difference overflowed"
        );
        point = std::move(line.point);
        value = line.value;
        ++iterations;
        if(detail::infinity_norm(next_gradient) <= tolerance){
            gradient_value = std::move(next_gradient);
            converged = true;
            break;
        }
        const long double ys = detail::checked_dot(
            gradient_difference, step
        );
        if(detail::sufficient_curvature(ys, step, gradient_difference)){
            if(history.size() == history_size) history.erase(history.begin());
            history.push_back(detail::LbfgsPair<Real>{
                std::move(step), std::move(gradient_difference), 1.0L / ys
            });
        }
        gradient_value = std::move(next_gradient);
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations,
        gradient_evaluations, converged
    };
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_QUASI_NEWTON_HPP_INCLUDED
