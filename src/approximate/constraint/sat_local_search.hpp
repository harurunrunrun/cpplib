#ifndef CPPLIB_SRC_APPROXIMATE_CONSTRAINT_SAT_LOCAL_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CONSTRAINT_SAT_LOCAL_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <span>
#include <stdexcept>
#include <vector>

#include "cnf.hpp"

namespace approximate::constraint {

struct SatSearchResult {
    BooleanAssignment assignment;
    std::size_t satisfied_clauses = 0;
    std::size_t steps = 0;
    std::size_t restarts = 0;
    bool found = false;
};

[[nodiscard]] inline bool is_valid_sat_search_result(
    const CnfFormula& formula,
    const SatSearchResult& result
){
    if(result.assignment.size() != formula.variable_count()) return false;
    for(const unsigned char value : result.assignment){
        if(value > 1) return false;
    }
    const std::size_t actual = formula.satisfied_clause_count(result.assignment);
    return actual == result.satisfied_clauses
        && result.found == (actual == formula.clause_count());
}

inline void validate_sat_search_result(
    const CnfFormula& formula,
    const SatSearchResult& result
){
    if(!is_valid_sat_search_result(formula, result)){
        throw std::invalid_argument("SAT local-search result is inconsistent");
    }
}

namespace detail {

[[nodiscard]] inline std::size_t sat_variable_index(CnfLiteral literal){
    const std::uint64_t magnitude = literal < 0
        ? static_cast<std::uint64_t>(-(literal + 1)) + std::uint64_t{1}
        : static_cast<std::uint64_t>(literal);
    return static_cast<std::size_t>(magnitude - 1);
}

[[nodiscard]] inline bool raw_clause_satisfied(
    const CnfFormula::Clause& clause,
    std::span<const unsigned char> assignment
){
    for(const CnfLiteral literal : clause){
        const bool value = assignment[sat_variable_index(literal)] != 0;
        if((literal > 0 && value) || (literal < 0 && !value)) return true;
    }
    return false;
}

[[nodiscard]] inline bool has_empty_clause(const CnfFormula& formula){
    for(const CnfFormula::Clause& clause : formula.clauses()){
        if(clause.empty()) return true;
    }
    return false;
}

inline void checked_increment(std::size_t& value, const char* message){
    if(value == std::numeric_limits<std::size_t>::max()){
        throw std::overflow_error(message);
    }
    ++value;
}

template<class UniformRandomBitGenerator>
[[nodiscard]] BooleanAssignment random_boolean_assignment(
    std::size_t variable_count,
    UniformRandomBitGenerator& generator
){
    std::bernoulli_distribution bit(0.5);
    BooleanAssignment assignment(variable_count);
    for(unsigned char& value : assignment){
        value = static_cast<unsigned char>(bit(generator));
    }
    return assignment;
}

inline void consider_assignment(
    const CnfFormula& formula,
    const BooleanAssignment& assignment,
    std::size_t score,
    SatSearchResult& result
){
    if(score > result.satisfied_clauses){
        result.assignment = assignment;
        result.satisfied_clauses = score;
    }
    result.found = result.satisfied_clauses == formula.clause_count();
}

[[nodiscard]] inline std::size_t score_after_flip(
    const CnfFormula& formula,
    BooleanAssignment& assignment,
    std::size_t variable
){
    assignment[variable] ^= static_cast<unsigned char>(1);
    const std::size_t score = formula.satisfied_clause_count(assignment);
    assignment[variable] ^= static_cast<unsigned char>(1);
    return score;
}

[[nodiscard]] inline std::size_t break_count_after_flip(
    const CnfFormula& formula,
    BooleanAssignment& assignment,
    std::size_t variable
){
    std::size_t result = 0;
    for(const CnfFormula::Clause& clause : formula.clauses()){
        const bool before = raw_clause_satisfied(clause, assignment);
        assignment[variable] ^= static_cast<unsigned char>(1);
        const bool after = raw_clause_satisfied(clause, assignment);
        assignment[variable] ^= static_cast<unsigned char>(1);
        if(before && !after) ++result;
    }
    return result;
}

[[nodiscard]] inline std::vector<std::size_t> clause_variables(
    const CnfFormula::Clause& clause
){
    std::vector<std::size_t> variables;
    variables.reserve(clause.size());
    for(const CnfLiteral literal : clause){
        variables.push_back(sat_variable_index(literal));
    }
    std::sort(variables.begin(), variables.end());
    variables.erase(std::unique(variables.begin(), variables.end()), variables.end());
    return variables;
}

[[nodiscard]] inline SatSearchResult initial_sat_result(const CnfFormula& formula){
    SatSearchResult result;
    result.assignment.assign(formula.variable_count(), 0);
    result.satisfied_clauses = formula.satisfied_clause_count(result.assignment);
    result.found = result.satisfied_clauses == formula.clause_count();
    return result;
}

}  // namespace detail

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult gsat(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit
){
    SatSearchResult result = detail::initial_sat_result(formula);
    if(result.found || detail::has_empty_clause(formula)) return result;

    for(std::size_t restart = 0; restart < restart_limit; ++restart){
        detail::checked_increment(result.restarts, "SAT restart counter overflow");
        BooleanAssignment assignment = detail::random_boolean_assignment(
            formula.variable_count(), generator
        );
        std::size_t score = formula.satisfied_clause_count(assignment);
        detail::consider_assignment(formula, assignment, score, result);
        if(result.found) return result;

        for(std::size_t step = 0; step < steps_per_restart; ++step){
            std::size_t best_variable = 0;
            std::size_t best_score = detail::score_after_flip(formula, assignment, 0);
            for(std::size_t variable = 1; variable < formula.variable_count(); ++variable){
                const std::size_t candidate = detail::score_after_flip(
                    formula, assignment, variable
                );
                if(candidate > best_score){
                    best_score = candidate;
                    best_variable = variable;
                }
            }
            assignment[best_variable] ^= static_cast<unsigned char>(1);
            score = best_score;
            detail::checked_increment(result.steps, "SAT step counter overflow");
            detail::consider_assignment(formula, assignment, score, result);
            if(result.found) return result;
        }
    }
    return result;
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult walksat(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double random_walk_probability = 0.5
){
    if(!std::isfinite(random_walk_probability)
       || random_walk_probability < 0.0
       || random_walk_probability > 1.0){
        throw std::invalid_argument("WalkSAT random-walk probability must be in [0, 1]");
    }
    SatSearchResult result = detail::initial_sat_result(formula);
    if(result.found || detail::has_empty_clause(formula)) return result;
    std::bernoulli_distribution use_random_walk(random_walk_probability);

    for(std::size_t restart = 0; restart < restart_limit; ++restart){
        detail::checked_increment(result.restarts, "SAT restart counter overflow");
        BooleanAssignment assignment = detail::random_boolean_assignment(
            formula.variable_count(), generator
        );
        std::size_t score = formula.satisfied_clause_count(assignment);
        detail::consider_assignment(formula, assignment, score, result);
        if(result.found) return result;

        for(std::size_t step = 0; step < steps_per_restart; ++step){
            const std::vector<std::size_t> unsatisfied =
                formula.unsatisfied_clause_indices(assignment);
            std::uniform_int_distribution<std::size_t> choose_clause(0, unsatisfied.size() - 1);
            const CnfFormula::Clause& clause = formula.clauses()[
                unsatisfied[choose_clause(generator)]
            ];
            const std::vector<std::size_t> variables = detail::clause_variables(clause);

            std::size_t chosen = variables.front();
            if(use_random_walk(generator)){
                std::uniform_int_distribution<std::size_t> choose_variable(0, variables.size() - 1);
                chosen = variables[choose_variable(generator)];
            }else{
                std::size_t best_break = detail::break_count_after_flip(
                    formula, assignment, chosen
                );
                for(std::size_t index = 1; index < variables.size(); ++index){
                    const std::size_t candidate = detail::break_count_after_flip(
                        formula, assignment, variables[index]
                    );
                    if(candidate < best_break){
                        best_break = candidate;
                        chosen = variables[index];
                    }
                }
            }
            assignment[chosen] ^= static_cast<unsigned char>(1);
            score = formula.satisfied_clause_count(assignment);
            detail::checked_increment(result.steps, "SAT step counter overflow");
            detail::consider_assignment(formula, assignment, score, result);
            if(result.found) return result;
        }
    }
    return result;
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult probsat(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double break_penalty_base = 2.3
){
    if(!std::isfinite(break_penalty_base) || break_penalty_base <= 1.0){
        throw std::invalid_argument("ProbSAT break-penalty base must be finite and greater than one");
    }
    SatSearchResult result = detail::initial_sat_result(formula);
    if(result.found || detail::has_empty_clause(formula)) return result;

    for(std::size_t restart = 0; restart < restart_limit; ++restart){
        detail::checked_increment(result.restarts, "SAT restart counter overflow");
        BooleanAssignment assignment = detail::random_boolean_assignment(
            formula.variable_count(), generator
        );
        std::size_t score = formula.satisfied_clause_count(assignment);
        detail::consider_assignment(formula, assignment, score, result);
        if(result.found) return result;

        for(std::size_t step = 0; step < steps_per_restart; ++step){
            const std::vector<std::size_t> unsatisfied =
                formula.unsatisfied_clause_indices(assignment);
            std::uniform_int_distribution<std::size_t> choose_clause(0, unsatisfied.size() - 1);
            const std::vector<std::size_t> variables = detail::clause_variables(
                formula.clauses()[unsatisfied[choose_clause(generator)]]
            );
            std::vector<std::size_t> breaks;
            breaks.reserve(variables.size());
            for(const std::size_t variable : variables){
                breaks.push_back(detail::break_count_after_flip(formula, assignment, variable));
            }
            const std::size_t minimum = *std::min_element(breaks.begin(), breaks.end());
            std::vector<double> weights;
            weights.reserve(breaks.size());
            for(const std::size_t value : breaks){
                weights.push_back(std::pow(
                    break_penalty_base,
                    -static_cast<double>(value - minimum)
                ));
            }
            std::discrete_distribution<std::size_t> choose_variable(
                weights.begin(), weights.end()
            );
            assignment[variables[choose_variable(generator)]] ^= static_cast<unsigned char>(1);
            score = formula.satisfied_clause_count(assignment);
            detail::checked_increment(result.steps, "SAT step counter overflow");
            detail::consider_assignment(formula, assignment, score, result);
            if(result.found) return result;
        }
    }
    return result;
}

}  // namespace approximate::constraint

#endif  // CPPLIB_SRC_APPROXIMATE_CONSTRAINT_SAT_LOCAL_SEARCH_HPP_INCLUDED
