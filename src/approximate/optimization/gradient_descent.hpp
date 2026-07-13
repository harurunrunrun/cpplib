#pragma once

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

template<class Real, class Objective, class Gradient>
auto gradient_descent(
    std::vector<Real> initial,
    Objective objective,
    Gradient gradient,
    std::size_t max_iterations,
    Real learning_rate,
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_positive(
        learning_rate, "learning_rate must be finite and positive"
    );
    return detail::run_first_order(
        std::move(initial), std::move(objective), max_iterations,
        tolerance, lower, upper,
        [gradient = std::move(gradient), learning_rate](
            const std::vector<Real>& point, std::size_t
        ) mutable {
            std::vector<Real> values = std::invoke(gradient, point);
            detail::validate_gradient(values, point.size());
            auto next = point;
            for(std::size_t index = 0; index < point.size(); ++index){
                next[index] -= learning_rate * values[index];
            }
            return detail::FirstOrderUpdate<Real>{
                std::move(next), std::move(values)
            };
        }
    );
}

}  // namespace approximate::optimization
