#pragma once

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

struct MinConflictsResult {
    std::vector<std::size_t> assignment;
    std::size_t conflict_incidence = 0;
    std::size_t steps = 0;
    std::size_t restarts = 0;
    bool found = false;
};

namespace detail {

inline void validate_finite_domains(std::span<const std::size_t> domain_sizes){
    for(const std::size_t size : domain_sizes){
        if(size == 0){
            throw std::invalid_argument("every CSP variable must have a nonempty domain");
        }
    }
}

template<class ConflictCounter>
[[nodiscard]] std::size_t checked_variable_conflicts(
    ConflictCounter& conflict_count,
    std::size_t variable,
    std::size_t candidate_value,
    std::span<const std::size_t> assignment
){
    using Raw = std::remove_cvref_t<std::invoke_result_t<
        ConflictCounter&,
        std::size_t,
        std::size_t,
        std::span<const std::size_t>
    >>;
    static_assert(std::integral<Raw> && !std::same_as<Raw, bool>,
                  "the CSP conflict counter must return an integer count");
    const Raw raw = std::invoke(
        conflict_count, variable, candidate_value, assignment
    );
    if constexpr(std::signed_integral<Raw>){
        if(raw < 0){
            throw std::invalid_argument("CSP conflict counts must be nonnegative");
        }
    }
    using UnsignedRaw = std::make_unsigned_t<Raw>;
    const UnsignedRaw value = static_cast<UnsignedRaw>(raw);
    if constexpr(std::numeric_limits<UnsignedRaw>::digits
                 > std::numeric_limits<std::size_t>::digits){
        if(value > static_cast<UnsignedRaw>(std::numeric_limits<std::size_t>::max())){
            throw std::overflow_error("CSP conflict count does not fit in size_t");
        }
    }
    return static_cast<std::size_t>(value);
}

template<class ConflictCounter>
[[nodiscard]] std::size_t total_conflict_incidence(
    ConflictCounter& conflict_count,
    std::span<const std::size_t> assignment
){
    std::size_t total = 0;
    for(std::size_t variable = 0; variable < assignment.size(); ++variable){
        const std::size_t value = checked_variable_conflicts(
            conflict_count, variable, assignment[variable], assignment
        );
        if(value > std::numeric_limits<std::size_t>::max() - total){
            throw std::overflow_error("total CSP conflict incidence overflows size_t");
        }
        total += value;
    }
    return total;
}

inline void csp_checked_increment(std::size_t& value, const char* message){
    if(value == std::numeric_limits<std::size_t>::max()){
        throw std::overflow_error(message);
    }
    ++value;
}

}  // namespace detail

template<class ConflictCounter>
[[nodiscard]] bool is_valid_min_conflicts_result(
    std::span<const std::size_t> domain_sizes,
    ConflictCounter&& conflict_count,
    const MinConflictsResult& result
){
    detail::validate_finite_domains(domain_sizes);
    if(result.assignment.size() != domain_sizes.size()) return false;
    for(std::size_t variable = 0; variable < domain_sizes.size(); ++variable){
        if(result.assignment[variable] >= domain_sizes[variable]) return false;
    }
    auto& counter = conflict_count;
    const std::size_t actual = detail::total_conflict_incidence(
        counter, result.assignment
    );
    return result.conflict_incidence == actual && result.found == (actual == 0);
}

template<class ConflictCounter>
void validate_min_conflicts_result(
    std::span<const std::size_t> domain_sizes,
    ConflictCounter&& conflict_count,
    const MinConflictsResult& result
){
    if(!is_valid_min_conflicts_result(
           domain_sizes,
           std::forward<ConflictCounter>(conflict_count),
           result
       )){
        throw std::invalid_argument("min-conflicts result is inconsistent");
    }
}

template<class ConflictCounter, class UniformRandomBitGenerator>
[[nodiscard]] MinConflictsResult min_conflicts(
    std::span<const std::size_t> domain_sizes,
    ConflictCounter&& conflict_count,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double random_move_probability = 0.0
){
    detail::validate_finite_domains(domain_sizes);
    if(!std::isfinite(random_move_probability)
       || random_move_probability < 0.0
       || random_move_probability > 1.0){
        throw std::invalid_argument("min-conflicts random-move probability must be in [0, 1]");
    }
    auto& counter = conflict_count;
    MinConflictsResult result;
    result.assignment.assign(domain_sizes.size(), 0);
    result.conflict_incidence = detail::total_conflict_incidence(
        counter, result.assignment
    );
    result.found = result.conflict_incidence == 0;
    if(result.found) return result;

    std::bernoulli_distribution use_random_move(random_move_probability);
    for(std::size_t restart = 0; restart < restart_limit; ++restart){
        detail::csp_checked_increment(result.restarts, "CSP restart counter overflow");
        std::vector<std::size_t> assignment(domain_sizes.size());
        for(std::size_t variable = 0; variable < domain_sizes.size(); ++variable){
            std::uniform_int_distribution<std::size_t> choose_value(
                0, domain_sizes[variable] - 1
            );
            assignment[variable] = choose_value(generator);
        }
        std::size_t total = detail::total_conflict_incidence(counter, assignment);
        if(total < result.conflict_incidence){
            result.assignment = assignment;
            result.conflict_incidence = total;
            result.found = total == 0;
        }
        if(result.found) return result;

        for(std::size_t step = 0; step < steps_per_restart; ++step){
            std::vector<std::size_t> conflicted_variables;
            for(std::size_t variable = 0; variable < assignment.size(); ++variable){
                if(detail::checked_variable_conflicts(
                       counter, variable, assignment[variable], assignment
                   ) > 0){
                    conflicted_variables.push_back(variable);
                }
            }
            if(conflicted_variables.empty()) break;
            std::uniform_int_distribution<std::size_t> choose_variable(
                0, conflicted_variables.size() - 1
            );
            const std::size_t variable = conflicted_variables[choose_variable(generator)];

            std::size_t chosen_value = 0;
            if(random_move_probability > 0.0 && use_random_move(generator)){
                std::uniform_int_distribution<std::size_t> choose_value(
                    0, domain_sizes[variable] - 1
                );
                chosen_value = choose_value(generator);
            }else{
                std::size_t best_conflicts = detail::checked_variable_conflicts(
                    counter, variable, 0, assignment
                );
                for(std::size_t value = 1; value < domain_sizes[variable]; ++value){
                    const std::size_t candidate = detail::checked_variable_conflicts(
                        counter, variable, value, assignment
                    );
                    if(candidate < best_conflicts){
                        best_conflicts = candidate;
                        chosen_value = value;
                    }
                }
            }
            assignment[variable] = chosen_value;
            detail::csp_checked_increment(result.steps, "CSP step counter overflow");
            total = detail::total_conflict_incidence(counter, assignment);
            if(total < result.conflict_incidence){
                result.assignment = assignment;
                result.conflict_incidence = total;
                result.found = total == 0;
            }
            if(result.found) return result;
        }
    }
    return result;
}

template<class ConflictCounter, class UniformRandomBitGenerator>
[[nodiscard]] MinConflictsResult randomized_min_conflicts(
    std::span<const std::size_t> domain_sizes,
    ConflictCounter&& conflict_count,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double random_move_probability
){
    return min_conflicts(
        domain_sizes,
        std::forward<ConflictCounter>(conflict_count),
        generator,
        steps_per_restart,
        restart_limit,
        random_move_probability
    );
}

}  // namespace approximate::constraint
