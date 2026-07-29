#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_SECOND_ORDER_AND_PATTERN_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_SECOND_ORDER_AND_PATTERN_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/advanced_common.hpp"

namespace approximate::optimization {

template<class Real>
struct NewtonOptimizationResult {
    OptimizationResult<Real> optimization;
    std::size_t hessian_evaluations = 0;
};

template<class Real>
struct ConjugateGradientResult {
    std::vector<Real> point;
    Real residual_norm{};
    std::size_t iterations = 0;
    bool converged = false;
};

namespace second_order_internal {

template<class Real>
std::vector<Real> solve_regularized_system(
    const std::vector<std::vector<Real>>& input,
    const std::vector<Real>& right_hand_side,
    Real regularization
) {
    const std::size_t size = right_hand_side.size();
    if(input.size() != size){
        throw std::invalid_argument(
            "Newton Hessian row count does not match the point"
        );
    }
    std::vector<std::vector<long double>> matrix(
        size, std::vector<long double>(size + 1, 0.0L)
    );
    for(std::size_t row = 0; row < size; ++row){
        if(input[row].size() != size){
            throw std::invalid_argument(
                "Newton Hessian must be square"
            );
        }
        for(std::size_t column = 0; column < size; ++column){
            matrix[row][column] =
                static_cast<long double>(input[row][column]);
            if(!std::isfinite(matrix[row][column])){
                throw std::domain_error(
                    "Newton Hessian contains a non-finite value"
                );
            }
        }
        matrix[row][row] += static_cast<long double>(regularization);
        matrix[row][size] =
            static_cast<long double>(right_hand_side[row]);
    }
    for(std::size_t pivot = 0; pivot < size; ++pivot){
        std::size_t selected = pivot;
        for(std::size_t row = pivot + 1; row < size; ++row){
            if(std::fabs(matrix[selected][pivot])
               < std::fabs(matrix[row][pivot])){
                selected = row;
            }
        }
        const long double scale = std::max(
            1.0L, std::fabs(matrix[selected][pivot])
        );
        if(std::fabs(matrix[selected][pivot])
           <= 64.0L * std::numeric_limits<long double>::epsilon()
                * scale){
            throw std::domain_error(
                "regularized Newton Hessian is singular"
            );
        }
        if(selected != pivot){
            std::swap(matrix[selected], matrix[pivot]);
        }
        for(std::size_t row = pivot + 1; row < size; ++row){
            const long double factor =
                matrix[row][pivot] / matrix[pivot][pivot];
            for(std::size_t column = pivot;
                column <= size; ++column){
                matrix[row][column] -=
                    factor * matrix[pivot][column];
            }
        }
    }
    std::vector<Real> result(size);
    for(std::size_t reverse = size; reverse > 0; --reverse){
        const std::size_t row = reverse - 1;
        long double value = matrix[row][size];
        for(std::size_t column = row + 1;
            column < size; ++column){
            value -= matrix[row][column]
                * static_cast<long double>(result[column]);
        }
        value /= matrix[row][row];
        result[row] = detail::checked_real<Real>(
            value, "Newton linear solve overflowed"
        );
    }
    return result;
}

template<class Real>
long double squared_norm(const std::vector<Real>& values) {
    long double result = 0.0L;
    for(const Real value : values){
        result += static_cast<long double>(value)
            * static_cast<long double>(value);
        if(!std::isfinite(result)){
            throw std::overflow_error("vector norm overflowed");
        }
    }
    return result;
}

}  // namespace second_order_internal

template<class Real, class Objective, class Gradient, class Hessian>
[[nodiscard]] NewtonOptimizationResult<Real> newton_method(
    std::vector<Real> point,
    Objective objective,
    Gradient gradient,
    Hessian hessian,
    std::size_t maximum_iterations,
    std::size_t maximum_evaluations,
    Real regularization = Real{1e-8},
    Real initial_step = Real{1},
    Real tolerance = Real{1e-8},
    Real armijo = Real{1e-4},
    Real line_reduction = Real{0.5},
    Real minimum_step = Real{1e-12},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
) {
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_problem(point, lower, upper);
    detail::validate_evaluation_limit(maximum_evaluations);
    detail::validate_nonnegative(
        regularization,
        "Newton regularization must be finite and nonnegative"
    );
    detail::validate_nonnegative(
        tolerance, "Newton tolerance must be finite and nonnegative"
    );
    detail::validate_backtracking_parameters(
        initial_step, armijo, line_reduction, minimum_step
    );
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    std::size_t hessian_evaluations = 0;
    Real value = detail::evaluate_objective<Real>(
        objective, point, evaluations
    );
    if(maximum_iterations == 0){
        return {
            {std::move(point), value, 0, evaluations, 0, false},
            0
        };
    }
    std::vector<Real> gradient_value = detail::evaluate_gradient<Real>(
        gradient, point, gradient_evaluations
    );
    bool converged =
        detail::infinity_norm(gradient_value) <= tolerance;
    std::size_t iterations = 0;
    while(iterations < maximum_iterations && !converged){
        if(!detail::has_evaluation_budget(
               evaluations, maximum_evaluations)){
            break;
        }
        const auto hessian_value = std::invoke(hessian, point);
        detail::increment_count(hessian_evaluations);
        std::vector<Real> right_hand_side(gradient_value.size());
        for(std::size_t index = 0;
            index < gradient_value.size(); ++index){
            right_hand_side[index] = -gradient_value[index];
        }
        std::vector<Real> direction =
            second_order_internal::solve_regularized_system(
                hessian_value, right_hand_side, regularization
            );
        if(!(detail::checked_dot(gradient_value, direction) < 0.0L)){
            direction = detail::negative_gradient(gradient_value);
        }
        auto line = detail::armijo_backtracking(
            point, value, gradient_value, direction, objective,
            evaluations, maximum_evaluations, initial_step, armijo,
            line_reduction, minimum_step, lower, upper
        );
        if(line.status == detail::LineSearchStatus::evaluation_limit){
            break;
        }
        if(line.status == detail::LineSearchStatus::no_feasible_descent){
            converged = true;
            break;
        }
        point = std::move(line.point);
        value = line.value;
        ++iterations;
        gradient_value = detail::evaluate_gradient<Real>(
            gradient, point, gradient_evaluations
        );
        converged =
            detail::infinity_norm(gradient_value) <= tolerance;
    }
    return {
        {
            std::move(point), value, iterations, evaluations,
            gradient_evaluations, converged
        },
        hessian_evaluations
    };
}

template<class Real, class MatrixVector>
[[nodiscard]] ConjugateGradientResult<Real> conjugate_gradient(
    MatrixVector matrix_vector,
    const std::vector<Real>& right_hand_side,
    std::vector<Real> point,
    std::size_t maximum_iterations,
    Real tolerance = Real{1e-10}
) {
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_point(right_hand_side);
    if(point.size() != right_hand_side.size()){
        throw std::invalid_argument(
            "conjugate-gradient point dimension does not match the system"
        );
    }
    detail::validate_point(point);
    detail::validate_nonnegative(
        tolerance,
        "conjugate-gradient tolerance must be finite and nonnegative"
    );
    auto apply = [&](const std::vector<Real>& vector){
        std::vector<Real> result = std::invoke(matrix_vector, vector);
        detail::validate_gradient(result, vector.size());
        return result;
    };
    const std::vector<Real> product = apply(point);
    std::vector<Real> residual(point.size());
    for(std::size_t index = 0; index < point.size(); ++index){
        residual[index] = detail::checked_real<Real>(
            static_cast<long double>(right_hand_side[index])
                - static_cast<long double>(product[index]),
            "conjugate-gradient residual overflowed"
        );
    }
    std::vector<Real> direction = residual;
    long double squared_residual =
        second_order_internal::squared_norm(residual);
    ConjugateGradientResult<Real> result;
    result.point = std::move(point);
    result.residual_norm = static_cast<Real>(
        std::sqrt(squared_residual)
    );
    result.converged = result.residual_norm <= tolerance;
    while(result.iterations < maximum_iterations
          && !result.converged){
        const std::vector<Real> matrix_direction = apply(direction);
        const long double curvature =
            detail::checked_dot(direction, matrix_direction);
        if(!(curvature > 0.0L)){
            throw std::domain_error(
                "conjugate gradient requires positive curvature"
            );
        }
        const long double alpha = squared_residual / curvature;
        for(std::size_t index = 0;
            index < result.point.size(); ++index){
            result.point[index] = detail::checked_real<Real>(
                static_cast<long double>(result.point[index])
                    + alpha
                        * static_cast<long double>(direction[index]),
                "conjugate-gradient point overflowed"
            );
            residual[index] = detail::checked_real<Real>(
                static_cast<long double>(residual[index])
                    - alpha * static_cast<long double>(
                        matrix_direction[index]
                    ),
                "conjugate-gradient residual overflowed"
            );
        }
        const long double next_squared =
            second_order_internal::squared_norm(residual);
        ++result.iterations;
        result.residual_norm = static_cast<Real>(
            std::sqrt(next_squared)
        );
        if(result.residual_norm <= tolerance){
            result.converged = true;
            break;
        }
        const long double beta = next_squared / squared_residual;
        for(std::size_t index = 0;
            index < direction.size(); ++index){
            direction[index] = detail::checked_real<Real>(
                static_cast<long double>(residual[index])
                    + beta * static_cast<long double>(direction[index]),
                "conjugate-gradient direction overflowed"
            );
        }
        squared_residual = next_squared;
    }
    return result;
}

template<class Real, class Objective>
[[nodiscard]] OptimizationResult<Real> pattern_search(
    std::vector<Real> point,
    Objective objective,
    const std::vector<std::vector<Real>>& directions,
    std::size_t maximum_iterations,
    std::size_t maximum_evaluations,
    Real initial_step = Real{1},
    Real reduction = Real{0.5},
    Real step_tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
) {
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_problem(point, lower, upper);
    detail::validate_evaluation_limit(maximum_evaluations);
    detail::validate_positive(
        initial_step,
        "pattern-search initial step must be positive and finite"
    );
    detail::validate_positive(
        step_tolerance,
        "pattern-search tolerance must be positive and finite"
    );
    if(!(reduction > Real{}) || !(reduction < Real{1})
       || !std::isfinite(reduction)){
        throw std::invalid_argument(
            "pattern-search reduction must be finite and in (0, 1)"
        );
    }
    if(directions.empty()){
        throw std::invalid_argument(
            "pattern search requires a direction"
        );
    }
    for(const auto& direction : directions){
        if(direction.size() != point.size()){
            throw std::invalid_argument(
                "pattern-search direction dimension mismatch"
            );
        }
        bool nonzero = false;
        for(const Real value : direction){
            if(!std::isfinite(value)){
                throw std::invalid_argument(
                    "pattern-search directions must be finite"
                );
            }
            nonzero = nonzero || value != Real{};
        }
        if(!nonzero){
            throw std::invalid_argument(
                "pattern-search directions must be nonzero"
            );
        }
    }
    std::size_t evaluations = 0;
    Real value = detail::evaluate_objective<Real>(
        objective, point, evaluations
    );
    Real step = initial_step;
    std::size_t iterations = 0;
    bool converged = false;
    while(iterations < maximum_iterations){
        if(step < step_tolerance){
            converged = true;
            break;
        }
        bool improved = false;
        std::vector<Real> best_point = point;
        Real best_value = value;
        for(const auto& direction : directions){
            if(!detail::has_evaluation_budget(
                   evaluations, maximum_evaluations)){
                break;
            }
            std::vector<Real> candidate = point;
            for(std::size_t coordinate = 0;
                coordinate < point.size(); ++coordinate){
                candidate[coordinate] = static_cast<Real>(
                    static_cast<long double>(candidate[coordinate])
                    + static_cast<long double>(step)
                        * static_cast<long double>(
                            direction[coordinate]
                        )
                );
            }
            detail::project_and_validate(candidate, lower, upper);
            if(candidate == point) continue;
            const Real candidate_value =
                detail::evaluate_objective<Real>(
                    objective, candidate, evaluations
                );
            if(candidate_value < best_value){
                improved = true;
                best_value = candidate_value;
                best_point = std::move(candidate);
            }
        }
        if(improved){
            point = std::move(best_point);
            value = best_value;
        }else{
            step *= reduction;
        }
        ++iterations;
        if(!detail::has_evaluation_budget(
               evaluations, maximum_evaluations)){
            break;
        }
    }
    return {
        std::move(point), value, iterations, evaluations, 0, converged
    };
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_SECOND_ORDER_AND_PATTERN_HPP_INCLUDED
