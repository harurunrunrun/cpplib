#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_ADAMW_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_ADAMW_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

template<class Real, class Objective, class Gradient>
auto adamw(
    std::vector<Real> initial,
    Objective objective,
    Gradient gradient,
    std::size_t max_iterations,
    Real learning_rate,
    Real weight_decay,
    Real beta1 = Real{0.9},
    Real beta2 = Real{0.999},
    Real epsilon = Real{1e-8},
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_positive(
        learning_rate, "learning_rate must be finite and positive"
    );
    detail::validate_nonnegative(
        weight_decay, "weight_decay must be finite and non-negative"
    );
    if(beta1 < Real{} || !(beta1 < Real{1}) || !std::isfinite(beta1))[[unlikely]]{
        throw std::invalid_argument("beta1 must be in [0, 1)");
    }
    if(beta2 < Real{} || !(beta2 < Real{1}) || !std::isfinite(beta2))[[unlikely]]{
        throw std::invalid_argument("beta2 must be in [0, 1)");
    }
    detail::validate_positive(epsilon, "epsilon must be finite and positive");
    std::vector<Real> first(initial.size(), Real{});
    std::vector<Real> second(initial.size(), Real{});
    return detail::run_first_order(
        std::move(initial), std::move(objective), max_iterations,
        tolerance, lower, upper,
        [gradient = std::move(gradient), learning_rate, weight_decay,
         beta1, beta2, epsilon, first = std::move(first),
         second = std::move(second)](
            const std::vector<Real>& point, std::size_t iteration
        ) mutable {
            std::vector<Real> values = std::invoke(gradient, point);
            detail::validate_gradient(values, point.size());
            const Real exponent = static_cast<Real>(iteration) + Real{1};
            const Real first_correction = Real{1} - std::pow(beta1, exponent);
            const Real second_correction = Real{1} - std::pow(beta2, exponent);
            auto next = point;
            for(std::size_t index = 0; index < point.size(); ++index){
                first[index] = beta1 * first[index]
                    + (Real{1} - beta1) * values[index];
                second[index] = beta2 * second[index]
                    + (Real{1} - beta2) * values[index] * values[index];
                if(!std::isfinite(first[index]) || !std::isfinite(second[index])){
                    throw std::overflow_error("AdamW accumulator overflowed");
                }
                const Real corrected_first = first[index] / first_correction;
                const Real corrected_second = second[index] / second_correction;
                next[index] -= learning_rate * (
                    corrected_first / (std::sqrt(corrected_second) + epsilon)
                    + weight_decay * point[index]
                );
            }
            return detail::FirstOrderUpdate<Real>{
                std::move(next), std::move(values)
            };
        }
    );
}

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_ADAMW_HPP_INCLUDED
