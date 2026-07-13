#pragma once

#include <cmath>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

template<class Real, class Objective, class Gradient>
auto rmsprop(
    std::vector<Real> initial,
    Objective objective,
    Gradient gradient,
    std::size_t max_iterations,
    Real learning_rate,
    Real decay = Real{0.9},
    Real epsilon = Real{1e-8},
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_positive(
        learning_rate, "learning_rate must be finite and positive"
    );
    if(decay < Real{} || !(decay < Real{1}) || !std::isfinite(decay))[[unlikely]]{
        throw std::invalid_argument("decay must be in [0, 1)");
    }
    detail::validate_positive(epsilon, "epsilon must be finite and positive");
    std::vector<Real> average(initial.size(), Real{});
    return detail::run_first_order(
        std::move(initial), std::move(objective), max_iterations,
        tolerance, lower, upper,
        [gradient = std::move(gradient), learning_rate, decay, epsilon,
         average = std::move(average)](
            const std::vector<Real>& point, std::size_t
        ) mutable {
            std::vector<Real> values = std::invoke(gradient, point);
            detail::validate_gradient(values, point.size());
            auto next = point;
            for(std::size_t index = 0; index < point.size(); ++index){
                average[index] = decay * average[index]
                    + (Real{1} - decay) * values[index] * values[index];
                if(!std::isfinite(average[index]))[[unlikely]]{
                    throw std::overflow_error("RMSProp accumulator overflowed");
                }
                next[index] -= learning_rate * values[index]
                    / (std::sqrt(average[index]) + epsilon);
            }
            return detail::FirstOrderUpdate<Real>{
                std::move(next), std::move(values)
            };
        }
    );
}

}  // namespace approximate::optimization
