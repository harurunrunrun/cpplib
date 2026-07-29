#ifndef CPPLIB_SRC_APPROXIMATE_CONSTRAINT_WEIGHTED_CSP_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CONSTRAINT_WEIGHTED_CSP_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::constraint {

struct ConstraintSearchResult {
    std::vector<std::size_t> assignment;
    std::vector<std::size_t> violated_constraints;
    std::size_t steps = 0;
    std::size_t restarts = 0;
    bool found = false;
};

namespace weighted_csp_internal {

inline void validate_model(
    std::span<const std::size_t> domain_sizes,
    const std::vector<std::vector<std::size_t>>& scopes
) {
    for(const std::size_t size : domain_sizes){
        if(size == 0){
            throw std::invalid_argument(
                "every weighted-CSP variable needs a nonempty domain"
            );
        }
    }
    for(const auto& scope : scopes){
        for(const std::size_t variable : scope){
            if(variable >= domain_sizes.size()){
                throw std::out_of_range(
                    "weighted-CSP constraint scope is out of range"
                );
            }
        }
    }
}

inline void checked_increment(std::size_t& value, const char* message) {
    if(value == std::numeric_limits<std::size_t>::max()){
        throw std::overflow_error(message);
    }
    ++value;
}

template<class ConstraintViolated>
bool is_violated(
    ConstraintViolated& violated,
    std::size_t constraint,
    std::span<const std::size_t> assignment
) {
    using Raw = std::remove_cvref_t<std::invoke_result_t<
        ConstraintViolated&, std::size_t,
        std::span<const std::size_t>
    >>;
    static_assert(
        std::convertible_to<Raw, bool>,
        "the constraint predicate must return a boolean-like value"
    );
    return static_cast<bool>(
        std::invoke(violated, constraint, assignment)
    );
}

template<class ConstraintViolated>
std::vector<std::size_t> collect_violations(
    std::size_t constraint_count,
    ConstraintViolated& violated,
    std::span<const std::size_t> assignment
) {
    std::vector<std::size_t> result;
    for(std::size_t constraint = 0;
        constraint < constraint_count; ++constraint){
        if(is_violated(violated, constraint, assignment)){
            result.push_back(constraint);
        }
    }
    return result;
}

template<class ConstraintViolated>
long double weighted_cost(
    const std::vector<long double>& weight,
    ConstraintViolated& violated,
    std::span<const std::size_t> assignment
) {
    long double result = 0.0L;
    for(std::size_t constraint = 0;
        constraint < weight.size(); ++constraint){
        if(!is_violated(violated, constraint, assignment)) continue;
        result += weight[constraint];
        if(!std::isfinite(result)){
            throw std::overflow_error(
                "weighted-CSP penalty overflowed"
            );
        }
    }
    return result;
}

template<class UniformRandomBitGenerator>
std::vector<std::size_t> random_assignment(
    std::span<const std::size_t> domain_sizes,
    UniformRandomBitGenerator& generator
) {
    std::vector<std::size_t> result(domain_sizes.size());
    for(std::size_t variable = 0;
        variable < domain_sizes.size(); ++variable){
        std::uniform_int_distribution<std::size_t> choose(
            0, domain_sizes[variable] - 1
        );
        result[variable] = choose(generator);
    }
    return result;
}

inline void consider(
    const std::vector<std::size_t>& assignment,
    const std::vector<std::size_t>& violations,
    ConstraintSearchResult& result
) {
    if(violations.size() < result.violated_constraints.size()){
        result.assignment = assignment;
        result.violated_constraints = violations;
    }
    result.found = result.violated_constraints.empty();
}

template<class ConstraintViolated>
std::pair<std::pair<std::size_t, std::size_t>, long double>
best_scope_move(
    const std::vector<std::size_t>& scope,
    std::span<const std::size_t> domain_sizes,
    ConstraintViolated& violated,
    const std::vector<long double>& weight,
    std::vector<std::size_t>& assignment
) {
    std::pair<std::size_t, std::size_t> best{
        assignment.size(), 0
    };
    long double best_cost =
        std::numeric_limits<long double>::infinity();
    std::vector<std::size_t> variables = scope;
    std::sort(variables.begin(), variables.end());
    variables.erase(
        std::unique(variables.begin(), variables.end()),
        variables.end()
    );
    for(const std::size_t variable : variables){
        const std::size_t original = assignment[variable];
        for(std::size_t value = 0;
            value < domain_sizes[variable]; ++value){
            if(value == original) continue;
            assignment[variable] = value;
            const long double candidate = weighted_cost(
                weight, violated, assignment
            );
            if(candidate < best_cost){
                best = {variable, value};
                best_cost = candidate;
            }
        }
        assignment[variable] = original;
    }
    return {best, best_cost};
}

template<class ConstraintViolated, class UniformRandomBitGenerator>
ConstraintSearchResult run_search(
    std::span<const std::size_t> domain_sizes,
    const std::vector<std::vector<std::size_t>>& scopes,
    ConstraintViolated& violated,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    long double weight_increment,
    bool random_walk
) {
    ConstraintSearchResult result;
    result.assignment.assign(domain_sizes.size(), 0);
    result.violated_constraints = collect_violations(
        scopes.size(), violated, result.assignment
    );
    result.found = result.violated_constraints.empty();
    if(result.found) return result;

    for(std::size_t restart = 0; restart < restart_limit; ++restart){
        checked_increment(
            result.restarts, "weighted-CSP restart counter overflow"
        );
        std::vector<std::size_t> assignment =
            random_assignment(domain_sizes, generator);
        std::vector<long double> weight(scopes.size(), 1.0L);
        auto violations = collect_violations(
            scopes.size(), violated, assignment
        );
        consider(assignment, violations, result);
        if(result.found) return result;

        for(std::size_t step = 0; step < steps_per_restart; ++step){
            std::vector<std::size_t> movable;
            for(const std::size_t constraint : violations){
                if(!scopes[constraint].empty()){
                    movable.push_back(constraint);
                }
            }
            if(movable.empty()) break;
            std::uniform_int_distribution<std::size_t> choose_constraint(
                0, movable.size() - 1
            );
            const std::size_t selected =
                movable[choose_constraint(generator)];
            if(random_walk){
                std::uniform_int_distribution<std::size_t> choose_variable(
                    0, scopes[selected].size() - 1
                );
                const std::size_t variable =
                    scopes[selected][choose_variable(generator)];
                std::uniform_int_distribution<std::size_t> choose_value(
                    0, domain_sizes[variable] - 1
                );
                assignment[variable] = choose_value(generator);
            }else{
                const long double current = weighted_cost(
                    weight, violated, assignment
                );
                auto [move, candidate] = best_scope_move(
                    scopes[selected], domain_sizes, violated,
                    weight, assignment
                );
                if(move.first == assignment.size()) break;
                if(!(candidate < current)){
                    for(const std::size_t constraint : violations){
                        weight[constraint] += weight_increment;
                        if(!std::isfinite(weight[constraint])){
                            throw std::overflow_error(
                                "weighted-CSP constraint weight overflowed"
                            );
                        }
                    }
                    std::tie(move, candidate) = best_scope_move(
                        scopes[selected], domain_sizes, violated,
                        weight, assignment
                    );
                }
                assignment[move.first] = move.second;
            }
            violations = collect_violations(
                scopes.size(), violated, assignment
            );
            checked_increment(
                result.steps, "weighted-CSP step counter overflow"
            );
            consider(assignment, violations, result);
            if(result.found) return result;
        }
    }
    return result;
}

}  // namespace weighted_csp_internal

template<class ConstraintViolated, class UniformRandomBitGenerator>
[[nodiscard]] ConstraintSearchResult constraint_weighting(
    std::span<const std::size_t> domain_sizes,
    const std::vector<std::vector<std::size_t>>& constraint_scopes,
    ConstraintViolated&& constraint_violated,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    long double weight_increment = 1.0L
) {
    weighted_csp_internal::validate_model(
        domain_sizes, constraint_scopes
    );
    if(!(weight_increment > 0.0L) || !std::isfinite(weight_increment)){
        throw std::invalid_argument(
            "constraint weight increment must be positive and finite"
        );
    }
    auto& violated = constraint_violated;
    return weighted_csp_internal::run_search(
        domain_sizes, constraint_scopes, violated, generator,
        steps_per_restart, restart_limit, weight_increment, false
    );
}

template<class ConstraintViolated, class UniformRandomBitGenerator>
[[nodiscard]] ConstraintSearchResult random_walk_csp(
    std::span<const std::size_t> domain_sizes,
    const std::vector<std::vector<std::size_t>>& constraint_scopes,
    ConstraintViolated&& constraint_violated,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit
) {
    weighted_csp_internal::validate_model(
        domain_sizes, constraint_scopes
    );
    auto& violated = constraint_violated;
    return weighted_csp_internal::run_search(
        domain_sizes, constraint_scopes, violated, generator,
        steps_per_restart, restart_limit, 1.0L, true
    );
}

}  // namespace approximate::constraint

#endif  // CPPLIB_SRC_APPROXIMATE_CONSTRAINT_WEIGHTED_CSP_SEARCH_HPP_INCLUDED
