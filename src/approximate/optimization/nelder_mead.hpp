#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/common.hpp"

namespace approximate::optimization {

template<class Real, class Objective>
auto nelder_mead(
    std::vector<Real> initial,
    Objective objective,
    std::size_t max_iterations,
    Real initial_step,
    Real tolerance = Real{1e-8},
    Real reflection = Real{1},
    Real expansion = Real{2},
    Real contraction = Real{0.5},
    Real shrink = Real{0.5},
    const std::vector<Real>& lower = {},
    const std::vector<Real>& upper = {}
){
    detail::validate_problem(initial, lower, upper);
    detail::validate_positive(
        initial_step, "initial_step must be finite and positive"
    );
    detail::validate_nonnegative(
        tolerance, "tolerance must be finite and non-negative"
    );
    detail::validate_positive(reflection, "reflection must be finite and positive");
    if(!(Real{1} < expansion) || !std::isfinite(expansion))[[unlikely]]{
        throw std::invalid_argument("expansion must be finite and greater than one");
    }
    if(!(Real{} < contraction) || !(contraction < Real{1})
       || !std::isfinite(contraction))[[unlikely]]{
        throw std::invalid_argument("contraction must be in (0, 1)");
    }
    if(!(Real{} < shrink) || !(shrink < Real{1})
       || !std::isfinite(shrink))[[unlikely]]{
        throw std::invalid_argument("shrink must be in (0, 1)");
    }
    const std::size_t dimension = initial.size();
    if(dimension == std::numeric_limits<std::size_t>::max())[[unlikely]]{
        throw std::overflow_error("simplex size overflows size_t");
    }
    std::vector<std::vector<Real>> simplex(dimension + 1, initial);
    for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
        simplex[coordinate + 1][coordinate] += initial_step;
        detail::project_and_validate(simplex[coordinate + 1], lower, upper);
        if(simplex[coordinate + 1] == initial && !lower.empty()){
            simplex[coordinate + 1][coordinate] = initial[coordinate] - initial_step;
            detail::project_and_validate(simplex[coordinate + 1], lower, upper);
        }
    }
    std::size_t evaluations = 0;
    std::vector<Real> values;
    values.reserve(dimension + 1);
    for(const auto& point: simplex){
        values.push_back(detail::evaluate_objective<Real>(
            objective, point, evaluations
        ));
    }
    const auto reorder = [&]{
        std::vector<std::size_t> order(dimension + 1);
        for(std::size_t index = 0; index <= dimension; ++index){
            order[index] = index;
        }
        std::stable_sort(order.begin(), order.end(), [&](auto left, auto right){
            return values[left] < values[right];
        });
        auto old_simplex = simplex;
        auto old_values = values;
        for(std::size_t index = 0; index <= dimension; ++index){
            simplex[index] = std::move(old_simplex[order[index]]);
            values[index] = old_values[order[index]];
        }
    };
    const auto make_candidate = [&](const std::vector<Real>& centroid,
                                    const std::vector<Real>& other,
                                    Real factor){
        auto candidate = centroid;
        for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
            candidate[coordinate] += factor
                * (centroid[coordinate] - other[coordinate]);
        }
        detail::project_and_validate(candidate, lower, upper);
        return candidate;
    };

    std::size_t iterations = 0;
    bool converged = false;
    while(iterations < max_iterations){
        reorder();
        long double diameter = 0.0L;
        long double value_spread = 0.0L;
        for(std::size_t index = 1; index <= dimension; ++index){
            value_spread = std::max(
                value_spread,
                std::abs(static_cast<long double>(values[index])
                         - static_cast<long double>(values[0]))
            );
            for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                diameter = std::max(
                    diameter,
                    std::abs(static_cast<long double>(simplex[index][coordinate])
                             - static_cast<long double>(simplex[0][coordinate]))
                );
            }
        }
        const long double threshold = static_cast<long double>(tolerance);
        if(diameter <= threshold && value_spread <= threshold){
            converged = true;
            break;
        }
        std::vector<Real> centroid(dimension, Real{});
        for(std::size_t index = 0; index < dimension; ++index){
            const Real divisor = static_cast<Real>(index + 1);
            for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                centroid[coordinate] +=
                    (simplex[index][coordinate] - centroid[coordinate]) / divisor;
            }
        }
        auto reflected = make_candidate(centroid, simplex[dimension], reflection);
        const Real reflected_value = detail::evaluate_objective<Real>(
            objective, reflected, evaluations
        );
        if(reflected_value < values[0]){
            auto expanded = make_candidate(
                centroid, simplex[dimension], reflection * expansion
            );
            const Real expanded_value = detail::evaluate_objective<Real>(
                objective, expanded, evaluations
            );
            if(expanded_value < reflected_value){
                simplex[dimension] = std::move(expanded);
                values[dimension] = expanded_value;
            }else{
                simplex[dimension] = std::move(reflected);
                values[dimension] = reflected_value;
            }
        }else if(reflected_value < values[dimension - 1]){
            simplex[dimension] = std::move(reflected);
            values[dimension] = reflected_value;
        }else{
            const bool outside = reflected_value < values[dimension];
            std::vector<Real> contracted(dimension);
            for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                const Real endpoint = outside
                    ? reflected[coordinate] : simplex[dimension][coordinate];
                contracted[coordinate] = centroid[coordinate]
                    + contraction * (endpoint - centroid[coordinate]);
            }
            detail::project_and_validate(contracted, lower, upper);
            const Real contracted_value = detail::evaluate_objective<Real>(
                objective, contracted, evaluations
            );
            const Real comparison = outside ? reflected_value : values[dimension];
            if(contracted_value < comparison){
                simplex[dimension] = std::move(contracted);
                values[dimension] = contracted_value;
            }else{
                for(std::size_t index = 1; index <= dimension; ++index){
                    for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                        simplex[index][coordinate] = simplex[0][coordinate]
                            + shrink * (simplex[index][coordinate]
                                        - simplex[0][coordinate]);
                    }
                    detail::project_and_validate(simplex[index], lower, upper);
                    values[index] = detail::evaluate_objective<Real>(
                        objective, simplex[index], evaluations
                    );
                }
            }
        }
        ++iterations;
    }
    reorder();
    return OptimizationResult<Real>{
        std::move(simplex[0]), values[0], iterations,
        evaluations, 0, converged
    };
}

}  // namespace approximate::optimization
