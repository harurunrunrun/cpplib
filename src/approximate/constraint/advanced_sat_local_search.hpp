#ifndef CPPLIB_SRC_APPROXIMATE_CONSTRAINT_ADVANCED_SAT_LOCAL_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CONSTRAINT_ADVANCED_SAT_LOCAL_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "sat_local_search.hpp"

namespace approximate::constraint {

namespace advanced_sat_internal {

inline void validate_probability(double value, const char* message) {
    if(!std::isfinite(value) || value < 0.0 || value > 1.0){
        throw std::invalid_argument(message);
    }
}

template<class UniformRandomBitGenerator>
std::pair<std::size_t, std::vector<std::size_t>>
random_unsatisfied_clause_variables(
    const CnfFormula& formula,
    const BooleanAssignment& assignment,
    UniformRandomBitGenerator& generator
) {
    const std::vector<std::size_t> unsatisfied =
        formula.unsatisfied_clause_indices(assignment);
    std::uniform_int_distribution<std::size_t> choose_clause(
        0, unsatisfied.size() - 1
    );
    const std::size_t clause = unsatisfied[choose_clause(generator)];
    return {
        clause,
        detail::clause_variables(formula.clauses()[clause])
    };
}

inline long double weighted_score(
    const CnfFormula& formula,
    const BooleanAssignment& assignment,
    const std::vector<long double>& weight
) {
    long double result = 0.0L;
    for(std::size_t clause = 0; clause < formula.clause_count(); ++clause){
        if(formula.clause_satisfied(clause, assignment)){
            result += weight[clause];
            if(!std::isfinite(result)){
                throw std::overflow_error("weighted SAT score overflowed");
            }
        }
    }
    return result;
}

inline std::pair<std::size_t, long double> best_weighted_flip(
    const CnfFormula& formula,
    BooleanAssignment& assignment,
    const std::vector<long double>& weight
) {
    std::size_t best = 0;
    assignment[0] ^= static_cast<unsigned char>(1);
    long double best_score = weighted_score(formula, assignment, weight);
    assignment[0] ^= static_cast<unsigned char>(1);
    for(std::size_t variable = 1;
        variable < formula.variable_count(); ++variable){
        assignment[variable] ^= static_cast<unsigned char>(1);
        const long double candidate =
            weighted_score(formula, assignment, weight);
        assignment[variable] ^= static_cast<unsigned char>(1);
        if(best_score < candidate){
            best = variable;
            best_score = candidate;
        }
    }
    return {best, best_score};
}

enum class WeightUpdate {
    additive,
    paws,
    multiplicative,
};

template<class UniformRandomBitGenerator>
SatSearchResult weighted_sat_search(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    WeightUpdate update,
    long double adjustment,
    double smoothing_probability,
    long double smoothing
) {
    SatSearchResult result = detail::initial_sat_result(formula);
    if(result.found || detail::has_empty_clause(formula)) return result;
    std::bernoulli_distribution smooth(smoothing_probability);
    constexpr long double epsilon =
        64.0L * std::numeric_limits<long double>::epsilon();

    for(std::size_t restart = 0; restart < restart_limit; ++restart){
        detail::checked_increment(
            result.restarts, "weighted SAT restart counter overflow"
        );
        BooleanAssignment assignment = detail::random_boolean_assignment(
            formula.variable_count(), generator
        );
        std::vector<long double> weight(formula.clause_count(), 1.0L);
        std::size_t score =
            formula.satisfied_clause_count(assignment);
        detail::consider_assignment(formula, assignment, score, result);
        if(result.found) return result;

        for(std::size_t step = 0; step < steps_per_restart; ++step){
            long double current = weighted_score(
                formula, assignment, weight
            );
            auto [variable, candidate] = best_weighted_flip(
                formula, assignment, weight
            );
            const long double scale = std::max(
                {1.0L, std::fabs(current), std::fabs(candidate)}
            );
            if(candidate <= current + epsilon * scale){
                for(std::size_t clause = 0;
                    clause < formula.clause_count(); ++clause){
                    if(formula.clause_satisfied(clause, assignment)) continue;
                    if(update == WeightUpdate::multiplicative){
                        weight[clause] *= adjustment;
                    }else{
                        weight[clause] += adjustment;
                    }
                    if(!std::isfinite(weight[clause])){
                        throw std::overflow_error(
                            "SAT clause weight overflowed"
                        );
                    }
                }
                if(smoothing_probability > 0.0
                   && smooth(generator)){
                    for(std::size_t clause = 0;
                        clause < formula.clause_count(); ++clause){
                        if(update == WeightUpdate::paws){
                            if(formula.clause_satisfied(clause, assignment)
                               && 1.0L < weight[clause]){
                                weight[clause] = std::max(
                                    1.0L, weight[clause] - 1.0L
                                );
                            }
                        }else if(update == WeightUpdate::multiplicative){
                            weight[clause] =
                                1.0L + smoothing * (weight[clause] - 1.0L);
                        }else{
                            weight[clause] = std::max(
                                1.0L, weight[clause] - adjustment
                            );
                        }
                    }
                }
                std::tie(variable, candidate) = best_weighted_flip(
                    formula, assignment, weight
                );
            }
            assignment[variable] ^= static_cast<unsigned char>(1);
            score = formula.satisfied_clause_count(assignment);
            detail::checked_increment(
                result.steps, "weighted SAT step counter overflow"
            );
            detail::consider_assignment(
                formula, assignment, score, result
            );
            if(result.found) return result;
        }
    }
    return result;
}

template<class UniformRandomBitGenerator>
SatSearchResult novelty_search(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double novelty_noise,
    double random_walk_probability
) {
    SatSearchResult result = detail::initial_sat_result(formula);
    if(result.found || detail::has_empty_clause(formula)) return result;
    std::bernoulli_distribution use_second(novelty_noise);
    std::bernoulli_distribution use_random_walk(random_walk_probability);

    for(std::size_t restart = 0; restart < restart_limit; ++restart){
        detail::checked_increment(
            result.restarts, "Novelty restart counter overflow"
        );
        BooleanAssignment assignment = detail::random_boolean_assignment(
            formula.variable_count(), generator
        );
        std::vector<std::size_t> last_flip(
            formula.variable_count(), 0
        );
        std::size_t score = formula.satisfied_clause_count(assignment);
        detail::consider_assignment(formula, assignment, score, result);
        if(result.found) return result;

        for(std::size_t step = 0; step < steps_per_restart; ++step){
            auto [ignored_clause, variables] =
                random_unsatisfied_clause_variables(
                    formula, assignment, generator
                );
            static_cast<void>(ignored_clause);
            std::size_t chosen = variables.front();
            if(random_walk_probability > 0.0
               && use_random_walk(generator)){
                std::uniform_int_distribution<std::size_t> choose(
                    0, variables.size() - 1
                );
                chosen = variables[choose(generator)];
            }else{
                std::vector<std::pair<std::size_t, std::size_t>> ranked;
                ranked.reserve(variables.size());
                for(const std::size_t variable : variables){
                    ranked.emplace_back(
                        detail::score_after_flip(
                            formula, assignment, variable
                        ),
                        variable
                    );
                }
                std::stable_sort(
                    ranked.begin(), ranked.end(),
                    [](const auto& left, const auto& right){
                        if(left.first != right.first){
                            return right.first < left.first;
                        }
                        return left.second < right.second;
                    }
                );
                chosen = ranked.front().second;
                std::size_t youngest = variables.front();
                for(const std::size_t variable : variables){
                    if(last_flip[youngest] < last_flip[variable]){
                        youngest = variable;
                    }
                }
                if(ranked.size() > 1 && chosen == youngest
                   && use_second(generator)){
                    chosen = ranked[1].second;
                }
            }
            assignment[chosen] ^= static_cast<unsigned char>(1);
            last_flip[chosen] = step + 1;
            score = formula.satisfied_clause_count(assignment);
            detail::checked_increment(
                result.steps, "Novelty step counter overflow"
            );
            detail::consider_assignment(
                formula, assignment, score, result
            );
            if(result.found) return result;
        }
    }
    return result;
}

}  // namespace advanced_sat_internal

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult novelty(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double novelty_noise = 0.3
) {
    advanced_sat_internal::validate_probability(
        novelty_noise, "Novelty noise must be in [0, 1]"
    );
    return advanced_sat_internal::novelty_search(
        formula, generator, steps_per_restart, restart_limit,
        novelty_noise, 0.0
    );
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult novelty_plus(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double novelty_noise = 0.3,
    double random_walk_probability = 0.01
) {
    advanced_sat_internal::validate_probability(
        novelty_noise, "Novelty+ noise must be in [0, 1]"
    );
    advanced_sat_internal::validate_probability(
        random_walk_probability,
        "Novelty+ random-walk probability must be in [0, 1]"
    );
    return advanced_sat_internal::novelty_search(
        formula, generator, steps_per_restart, restart_limit,
        novelty_noise, random_walk_probability
    );
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult random_walk_sat(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit
) {
    return advanced_sat_internal::novelty_search(
        formula, generator, steps_per_restart, restart_limit,
        0.0, 1.0
    );
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult breakout_method(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    long double weight_increment = 1.0L
) {
    if(!(weight_increment > 0.0L) || !std::isfinite(weight_increment)){
        throw std::invalid_argument(
            "Breakout weight increment must be positive and finite"
        );
    }
    return advanced_sat_internal::weighted_sat_search(
        formula, generator, steps_per_restart, restart_limit,
        advanced_sat_internal::WeightUpdate::additive,
        weight_increment, 0.0, 1.0L
    );
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult paws(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    double smoothing_probability = 0.15
) {
    advanced_sat_internal::validate_probability(
        smoothing_probability,
        "PAWS smoothing probability must be in [0, 1]"
    );
    return advanced_sat_internal::weighted_sat_search(
        formula, generator, steps_per_restart, restart_limit,
        advanced_sat_internal::WeightUpdate::paws,
        1.0L, smoothing_probability, 1.0L
    );
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult saps(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    long double scaling = 1.3L,
    double smoothing_probability = 0.05,
    long double smoothing = 0.8L
) {
    if(!(scaling > 1.0L) || !std::isfinite(scaling)){
        throw std::invalid_argument(
            "SAPS scaling must be finite and greater than one"
        );
    }
    advanced_sat_internal::validate_probability(
        smoothing_probability,
        "SAPS smoothing probability must be in [0, 1]"
    );
    if(smoothing < 0.0L || smoothing > 1.0L
       || !std::isfinite(smoothing)){
        throw std::invalid_argument(
            "SAPS smoothing factor must be in [0, 1]"
        );
    }
    return advanced_sat_internal::weighted_sat_search(
        formula, generator, steps_per_restart, restart_limit,
        advanced_sat_internal::WeightUpdate::multiplicative,
        scaling, smoothing_probability, smoothing
    );
}

template<class UniformRandomBitGenerator>
[[nodiscard]] SatSearchResult clause_weighting_sat(
    const CnfFormula& formula,
    UniformRandomBitGenerator& generator,
    std::size_t steps_per_restart,
    std::size_t restart_limit,
    long double weight_increment = 1.0L,
    double smoothing_probability = 0.0
) {
    if(!(weight_increment > 0.0L) || !std::isfinite(weight_increment)){
        throw std::invalid_argument(
            "clause-weight increment must be positive and finite"
        );
    }
    advanced_sat_internal::validate_probability(
        smoothing_probability,
        "clause-weight smoothing probability must be in [0, 1]"
    );
    return advanced_sat_internal::weighted_sat_search(
        formula, generator, steps_per_restart, restart_limit,
        advanced_sat_internal::WeightUpdate::additive,
        weight_increment, smoothing_probability, 1.0L
    );
}

}  // namespace approximate::constraint

#endif  // CPPLIB_SRC_APPROXIMATE_CONSTRAINT_ADVANCED_SAT_LOCAL_SEARCH_HPP_INCLUDED
