#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_ADAGRAD_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_ADAGRAD_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

template<class Real, class Objective, class Gradient>
auto adagrad(
    std::vector<Real> initial,
    Objective objective,
    Gradient gradient,
    std::size_t max_iterations,
    Real learning_rate,
    Real epsilon = Real{1e-8},
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_positive(
        learning_rate, "learning_rate must be finite and positive"
    );
    detail::validate_positive(epsilon, "epsilon must be finite and positive");
    std::vector<Real> accumulated(initial.size(), Real{});
    return detail::run_first_order(
        std::move(initial), std::move(objective), max_iterations,
        tolerance, lower, upper,
        [gradient = std::move(gradient), learning_rate, epsilon,
         accumulated = std::move(accumulated)](
            const std::vector<Real>& point, std::size_t
        ) mutable {
            std::vector<Real> values = std::invoke(gradient, point);
            detail::validate_gradient(values, point.size());
            auto next = point;
            for(std::size_t index = 0; index < point.size(); ++index){
                accumulated[index] += values[index] * values[index];
                if(!std::isfinite(accumulated[index]))[[unlikely]]{
                    throw std::overflow_error("AdaGrad accumulator overflowed");
                }
                next[index] -= learning_rate * values[index]
                    / (std::sqrt(accumulated[index]) + epsilon);
            }
            return detail::FirstOrderUpdate<Real>{
                std::move(next), std::move(values)
            };
        }
    );
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_ADAGRAD_HPP_INCLUDED
