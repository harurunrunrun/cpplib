#pragma once

#include <cmath>
#include <cstddef>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/advanced_common.hpp"

namespace approximate::optimization {

template<class Real, class Objective, class Urbg>
auto spsa(
    std::vector<Real> point,
    Objective objective,
    Urbg& random,
    std::size_t max_iterations,
    std::size_t max_evaluations,
    Real learning_rate,
    Real perturbation,
    Real learning_exponent = Real{0.602},
    Real perturbation_exponent = Real{0.101},
    Real stability = Real{10},
    Real tolerance = Real{1e-8},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_problem(point, lower, upper);
    detail::validate_evaluation_limit(max_evaluations);
    detail::validate_positive(
        learning_rate, "learning_rate must be finite and positive"
    );
    detail::validate_positive(
        perturbation, "perturbation must be finite and positive"
    );
    detail::validate_positive(
        learning_exponent, "learning_exponent must be finite and positive"
    );
    detail::validate_positive(
        perturbation_exponent,
        "perturbation_exponent must be finite and positive"
    );
    detail::validate_nonnegative(
        stability, "stability must be finite and non-negative"
    );
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    Real value = detail::evaluate_objective<Real>(objective, point, evaluations);
    std::size_t iterations = 0;
    bool converged = false;
    std::uniform_int_distribution<int> bit(0, 1);
    while(iterations < max_iterations){
        if(!detail::has_evaluation_budget(
            evaluations, max_evaluations, 3
        )) break;
        const Real iteration = static_cast<Real>(iterations + 1);
        if(!std::isfinite(iteration))[[unlikely]]{
            throw std::overflow_error("SPSA iteration does not fit Real");
        }
        const Real learning_denominator = std::pow(
            stability + iteration, learning_exponent
        );
        const Real perturbation_denominator = std::pow(
            iteration, perturbation_exponent
        );
        const Real step = learning_rate / learning_denominator;
        const Real radius = perturbation / perturbation_denominator;
        if(!(Real{} < step) || !std::isfinite(step)
           || !(Real{} < radius) || !std::isfinite(radius))[[unlikely]]{
            throw std::overflow_error("SPSA gain sequence became non-finite");
        }
        std::vector<Real> delta(point.size());
        for(Real& value_delta: delta){
            value_delta = bit(random) == 0 ? Real{-1} : Real{1};
        }
        std::vector<Real> plus(point.size());
        std::vector<Real> minus(point.size());
        for(std::size_t index = 0; index < point.size(); ++index){
            plus[index] = detail::checked_real<Real>(
                static_cast<long double>(point[index])
                    + static_cast<long double>(radius)
                        * static_cast<long double>(delta[index]),
                "SPSA perturbation overflowed"
            );
            minus[index] = detail::checked_real<Real>(
                static_cast<long double>(point[index])
                    - static_cast<long double>(radius)
                        * static_cast<long double>(delta[index]),
                "SPSA perturbation overflowed"
            );
        }
        detail::project_and_validate(plus, lower, upper);
        detail::project_and_validate(minus, lower, upper);
        std::size_t required = 1;
        if(plus != point) ++required;
        if(minus != point && minus != plus) ++required;
        if(!detail::has_evaluation_budget(
            evaluations, max_evaluations, required
        )) break;
        const Real plus_value = plus == point
            ? value
            : detail::evaluate_objective<Real>(objective, plus, evaluations);
        const Real minus_value = minus == point
            ? value
            : (minus == plus
                ? plus_value
                : detail::evaluate_objective<Real>(objective, minus, evaluations));
        const long double objective_difference =
            static_cast<long double>(plus_value)
            - static_cast<long double>(minus_value);
        if(!std::isfinite(objective_difference))[[unlikely]]{
            throw std::overflow_error("SPSA objective difference overflowed");
        }
        std::vector<Real> estimate(point.size());
        for(std::size_t index = 0; index < point.size(); ++index){
            const long double coordinate_difference =
                static_cast<long double>(plus[index])
                - static_cast<long double>(minus[index]);
            estimate[index] = coordinate_difference == 0
                ? Real{}
                : detail::checked_real<Real>(
                    objective_difference / coordinate_difference,
                    "SPSA gradient estimate overflowed"
                );
        }
        detail::increment_count(gradient_evaluations);
        std::vector<Real> candidate(point.size());
        for(std::size_t index = 0; index < point.size(); ++index){
            candidate[index] = detail::checked_real<Real>(
                static_cast<long double>(point[index])
                    - static_cast<long double>(step)
                        * static_cast<long double>(estimate[index]),
                "SPSA update overflowed"
            );
        }
        detail::project_and_validate(candidate, lower, upper);
        Real candidate_value = value;
        if(candidate == plus){
            candidate_value = plus_value;
        }else if(candidate == minus){
            candidate_value = minus_value;
        }else if(candidate != point){
            candidate_value = detail::evaluate_objective<Real>(
                objective, candidate, evaluations
            );
        }
        long double displacement = 0;
        for(std::size_t index = 0; index < point.size(); ++index){
            const long double difference =
                static_cast<long double>(candidate[index])
                - static_cast<long double>(point[index]);
            if(!std::isfinite(difference))[[unlikely]]{
                throw std::overflow_error("SPSA displacement overflowed");
            }
            displacement = std::max(
                displacement, std::abs(difference)
            );
        }
        point = std::move(candidate);
        value = candidate_value;
        ++iterations;
        if(displacement <= static_cast<long double>(tolerance)){
            converged = true;
            break;
        }
    }
    return OptimizationResult<Real>{
        std::move(point), value, iterations, evaluations,
        gradient_evaluations, converged
    };
}

}  // namespace approximate::optimization
