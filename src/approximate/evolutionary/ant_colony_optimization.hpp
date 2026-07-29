#ifndef CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_ANT_COLONY_OPTIMIZATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_ANT_COLONY_OPTIMIZATION_HPP_INCLUDED
#include <algorithm>
#include <limits>

#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "detail/objective.hpp"
#include "detail/safe_count.hpp"

namespace approximate::evolutionary {

template<class State, class Score, class Key, class Real,
         class Hash = std::hash<Key>>
struct AntColonyResult{
    State solution;
    Score score;
    std::unordered_map<Key, Real, Hash> pheromones;
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
};

namespace ant_colony_detail {

template<class State, class Complete, class Choices, class Apply,
         class Heuristic, class KeyFunction, class Real, class Hash,
         class LocalUpdate, class URBG>
auto construct(
    State state,
    Complete& complete,
    Choices& choices,
    Apply& apply,
    Heuristic& heuristic,
    KeyFunction& key,
    std::unordered_map<
        std::decay_t<std::invoke_result_t<
            KeyFunction&, const State&,
            const std::decay_t<decltype(
                *std::begin(std::declval<std::remove_reference_t<
                    std::invoke_result_t<Choices&, const State&>
                >&>())
            )>&
        >>,
        Real, Hash
    >& pheromones,
    Real initial_pheromone,
    Real pheromone_exponent,
    Real heuristic_exponent,
    std::size_t maximum_steps,
    LocalUpdate& local_update,
    URBG& random
){
    using ChoiceRange = std::invoke_result_t<Choices&, const State&>;
    using Choice = std::decay_t<decltype(
        *std::begin(std::declval<std::remove_reference_t<ChoiceRange>&>())
    )>;
    using Key = std::decay_t<
        std::invoke_result_t<KeyFunction&, const State&, const Choice&>
    >;
    std::vector<Key> path;
    for(std::size_t step = 0;
        step < maximum_steps
        && !static_cast<bool>(std::invoke(complete, state));
        ++step){
        auto range = std::invoke(choices, state);
        std::vector<Choice> available(std::begin(range), std::end(range));
        if(available.empty()){
            throw std::runtime_error("ant construction reached a dead end");
        }
        std::vector<double> weights;
        weights.reserve(available.size());
        for(const Choice& choice : available){
            const Key edge_key = std::invoke(key, state, choice);
            auto [found, inserted] = pheromones.try_emplace(
                edge_key, initial_pheromone
            );
            static_cast<void>(inserted);
            const Real desirability = static_cast<Real>(
                std::invoke(heuristic, state, choice)
            );
            if(desirability < Real{}
               || !std::isfinite(desirability))[[unlikely]]{
                throw std::invalid_argument(
                    "ant heuristic values must be finite and non-negative"
                );
            }
            const Real weight =
                std::pow(found->second, pheromone_exponent)
                * std::pow(desirability, heuristic_exponent);
            if(weight < Real{} || !std::isfinite(weight))[[unlikely]]{
                throw std::invalid_argument(
                    "ant transition weights must be finite"
                );
            }
            weights.push_back(static_cast<double>(weight));
        }
        std::discrete_distribution<std::size_t> select(
            weights.begin(), weights.end()
        );
        const Choice& selected = available[select(random)];
        const Key selected_key = std::invoke(key, state, selected);
        path.push_back(selected_key);
        std::invoke(local_update, selected_key, pheromones);
        state = std::invoke(apply, std::move(state), selected);
    }
    if(!static_cast<bool>(std::invoke(complete, state))){
        throw std::runtime_error(
            "ant construction exceeded maximum_steps"
        );
    }
    return std::pair<State, std::vector<Key>>{
        std::move(state), std::move(path)
    };
}

template<class Real>
void validate_common(
    std::size_t iterations,
    std::size_t ants,
    std::size_t maximum_steps,
    Real initial_pheromone,
    Real pheromone_exponent,
    Real heuristic_exponent
){
    if(iterations == 0 || ants == 0 || maximum_steps == 0
       || !(initial_pheromone > Real{})
       || pheromone_exponent < Real{}
       || heuristic_exponent < Real{}
       || !std::isfinite(initial_pheromone)
       || !std::isfinite(pheromone_exponent)
       || !std::isfinite(heuristic_exponent))[[unlikely]]{
        throw std::invalid_argument(
            "ant colony optimization has invalid parameters"
        );
    }
}

}  // namespace ant_colony_detail

template<class Real = double, class State, class Complete, class Choices,
         class Apply, class Heuristic, class KeyFunction, class Evaluate,
         class Update, class URBG,
         class Key = std::decay_t<std::invoke_result_t<
             KeyFunction&, const State&,
             const std::decay_t<decltype(
                 *std::begin(std::declval<std::remove_reference_t<
                     std::invoke_result_t<Choices&, const State&>
                 >&>())
             )>&
         >>,
         class Hash = std::hash<Key>, class Compare = std::less<>>
auto ant_colony_optimization(
    State initial_state,
    Complete complete,
    Choices choices,
    Apply apply,
    Heuristic heuristic,
    KeyFunction key,
    Evaluate evaluate,
    Update update,
    std::size_t iterations,
    std::size_t ants,
    std::size_t maximum_steps,
    Real initial_pheromone,
    Real pheromone_exponent,
    Real heuristic_exponent,
    URBG& random,
    Hash hash = {},
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    ant_colony_detail::validate_common(
        iterations, ants, maximum_steps, initial_pheromone,
        pheromone_exponent, heuristic_exponent
    );
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const State&>
    >;
    const std::size_t evaluations = detail::checked_multiply(
        iterations, ants, "ACO evaluation count overflows size_t"
    );
    std::unordered_map<Key, Real, Hash> pheromones(0, std::move(hash));
    std::optional<State> best;
    std::optional<Score> best_score;
    const auto no_local_update = [](const Key&, auto&){};
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::vector<State> solutions;
        std::vector<std::vector<Key>> paths;
        std::vector<Score> scores;
        solutions.reserve(ants);
        paths.reserve(ants);
        scores.reserve(ants);
        for(std::size_t ant = 0; ant < ants; ++ant){
            auto built = ant_colony_detail::construct(
                initial_state, complete, choices, apply, heuristic, key,
                pheromones, initial_pheromone, pheromone_exponent,
                heuristic_exponent, maximum_steps, no_local_update, random
            );
            Score score = detail::evaluate_objective(
                evaluate, built.first
            );
            if(!best_score || std::invoke(compare, score, *best_score)){
                best = built.first;
                best_score = score;
            }
            solutions.push_back(std::move(built.first));
            paths.push_back(std::move(built.second));
            scores.push_back(std::move(score));
        }
        std::invoke(
            update, pheromones, std::as_const(solutions),
            std::as_const(scores), std::as_const(paths), iteration
        );
        for(const auto& entry : pheromones){
            if(!(entry.second > Real{})
               || !std::isfinite(entry.second))[[unlikely]]{
                throw std::invalid_argument(
                    "ACO update produced invalid pheromone"
                );
            }
        }
    }
    return AntColonyResult<State, Score, Key, Real, Hash>{
        std::move(*best), std::move(*best_score), std::move(pheromones),
        iterations, evaluations
    };
}

template<class Real = double, class State, class Complete, class Choices,
         class Apply, class Heuristic, class KeyFunction, class Evaluate,
         class Deposit, class URBG,
         class Key = std::decay_t<std::invoke_result_t<
             KeyFunction&, const State&,
             const std::decay_t<decltype(
                 *std::begin(std::declval<std::remove_reference_t<
                     std::invoke_result_t<Choices&, const State&>
                 >&>())
             )>&
         >>,
         class Hash = std::hash<Key>, class Compare = std::less<>>
auto ant_system(
    State initial_state,
    Complete complete,
    Choices choices,
    Apply apply,
    Heuristic heuristic,
    KeyFunction key,
    Evaluate evaluate,
    Deposit deposit,
    std::size_t iterations,
    std::size_t ants,
    std::size_t maximum_steps,
    Real initial_pheromone,
    Real evaporation_rate,
    Real pheromone_exponent,
    Real heuristic_exponent,
    URBG& random,
    Hash hash = {},
    Compare compare = {}
){
    if(!(evaporation_rate > Real{} && evaporation_rate <= Real{1}))[[unlikely]]{
        throw std::invalid_argument("ant_system has invalid evaporation");
    }
    const auto update = [&](auto& pheromones, const auto&,
                            const auto& scores, const auto& paths,
                            std::size_t){
        for(auto& entry : pheromones){
            entry.second *= Real{1} - evaporation_rate;
        }
        for(std::size_t ant = 0; ant < paths.size(); ++ant){
            const Real amount = static_cast<Real>(
                std::invoke(deposit, scores[ant])
            );
            if(amount < Real{} || !std::isfinite(amount))[[unlikely]]{
                throw std::invalid_argument(
                    "ant_system deposit must be finite and non-negative"
                );
            }
            for(const Key& edge : paths[ant]) pheromones[edge] += amount;
        }
        for(auto& entry : pheromones){
            entry.second = std::max(
                entry.second, std::numeric_limits<Real>::min()
            );
        }
    };
    return ant_colony_optimization<Real>(
        std::move(initial_state), std::move(complete), std::move(choices),
        std::move(apply), std::move(heuristic), std::move(key),
        std::move(evaluate), update, iterations, ants, maximum_steps,
        initial_pheromone, pheromone_exponent, heuristic_exponent,
        random, std::move(hash), std::move(compare)
    );
}

template<class Real = double, class State, class Complete, class Choices,
         class Apply, class Heuristic, class KeyFunction, class Evaluate,
         class Deposit, class URBG,
         class Key = std::decay_t<std::invoke_result_t<
             KeyFunction&, const State&,
             const std::decay_t<decltype(
                 *std::begin(std::declval<std::remove_reference_t<
                     std::invoke_result_t<Choices&, const State&>
                 >&>())
             )>&
         >>,
         class Hash = std::hash<Key>, class Compare = std::less<>>
auto ant_colony_system(
    State initial_state,
    Complete complete,
    Choices choices,
    Apply apply,
    Heuristic heuristic,
    KeyFunction key,
    Evaluate evaluate,
    Deposit deposit,
    std::size_t iterations,
    std::size_t ants,
    std::size_t maximum_steps,
    Real initial_pheromone,
    Real global_evaporation_rate,
    Real local_update_rate,
    Real pheromone_exponent,
    Real heuristic_exponent,
    URBG& random,
    Hash hash = {},
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    ant_colony_detail::validate_common(
        iterations, ants, maximum_steps, initial_pheromone,
        pheromone_exponent, heuristic_exponent
    );
    if(!(global_evaporation_rate > Real{}
         && global_evaporation_rate <= Real{1})
       || !(local_update_rate > Real{}
            && local_update_rate <= Real{1}))[[unlikely]]{
        throw std::invalid_argument("ACS has invalid update rates");
    }
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const State&>
    >;
    const std::size_t evaluations = detail::checked_multiply(
        iterations, ants, "ACS evaluation count overflows size_t"
    );
    std::unordered_map<Key, Real, Hash> pheromones(0, std::move(hash));
    std::optional<State> best;
    std::optional<Score> best_score;
    std::vector<Key> best_path;
    const auto local_update = [&](const Key& edge, auto& values){
        values[edge] =
            (Real{1} - local_update_rate) * values[edge]
            + local_update_rate * initial_pheromone;
    };
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        for(std::size_t ant = 0; ant < ants; ++ant){
            auto built = ant_colony_detail::construct(
                initial_state, complete, choices, apply, heuristic, key,
                pheromones, initial_pheromone, pheromone_exponent,
                heuristic_exponent, maximum_steps, local_update, random
            );
            Score score = detail::evaluate_objective(
                evaluate, built.first
            );
            if(!best_score || std::invoke(compare, score, *best_score)){
                best = built.first;
                best_score = score;
                best_path = std::move(built.second);
            }
        }
        for(auto& entry : pheromones){
            entry.second *= Real{1} - global_evaporation_rate;
            entry.second = std::max(
                entry.second, std::numeric_limits<Real>::min()
            );
        }
        const Real amount = static_cast<Real>(
            std::invoke(deposit, *best_score)
        );
        if(amount < Real{} || !std::isfinite(amount))[[unlikely]]{
            throw std::invalid_argument(
                "ACS deposit must be finite and non-negative"
            );
        }
        for(const Key& edge : best_path) pheromones[edge] += amount;
    }
    return AntColonyResult<State, Score, Key, Real, Hash>{
        std::move(*best), std::move(*best_score), std::move(pheromones),
        iterations, evaluations
    };
}

template<class Real = double, class State, class Complete, class Choices,
         class Apply, class Heuristic, class KeyFunction, class Evaluate,
         class Deposit, class URBG,
         class Key = std::decay_t<std::invoke_result_t<
             KeyFunction&, const State&,
             const std::decay_t<decltype(
                 *std::begin(std::declval<std::remove_reference_t<
                     std::invoke_result_t<Choices&, const State&>
                 >&>())
             )>&
         >>,
         class Hash = std::hash<Key>, class Compare = std::less<>>
auto max_min_ant_system(
    State initial_state,
    Complete complete,
    Choices choices,
    Apply apply,
    Heuristic heuristic,
    KeyFunction key,
    Evaluate evaluate,
    Deposit deposit,
    std::size_t iterations,
    std::size_t ants,
    std::size_t maximum_steps,
    Real initial_pheromone,
    Real minimum_pheromone,
    Real maximum_pheromone,
    Real evaporation_rate,
    Real pheromone_exponent,
    Real heuristic_exponent,
    URBG& random,
    Hash hash = {},
    Compare compare = {}
){
    if(!(minimum_pheromone > Real{})
       || maximum_pheromone < minimum_pheromone
       || !(evaporation_rate > Real{}
            && evaporation_rate <= Real{1}))[[unlikely]]{
        throw std::invalid_argument("MAX-MIN ant system has invalid bounds");
    }
    const auto update = [&](auto& pheromones, const auto&,
                            const auto& scores, const auto& paths,
                            std::size_t){
        std::size_t best = 0;
        for(std::size_t ant = 1; ant < scores.size(); ++ant){
            if(std::invoke(compare, scores[ant], scores[best])) best = ant;
        }
        for(auto& entry : pheromones){
            entry.second *= Real{1} - evaporation_rate;
        }
        const Real amount = static_cast<Real>(
            std::invoke(deposit, scores[best])
        );
        if(amount < Real{} || !std::isfinite(amount))[[unlikely]]{
            throw std::invalid_argument(
                "MAX-MIN deposit must be finite and non-negative"
            );
        }
        for(const Key& edge : paths[best]) pheromones[edge] += amount;
        for(auto& entry : pheromones){
            entry.second = std::clamp(
                entry.second, minimum_pheromone, maximum_pheromone
            );
        }
    };
    return ant_colony_optimization<Real>(
        std::move(initial_state), std::move(complete), std::move(choices),
        std::move(apply), std::move(heuristic), std::move(key),
        std::move(evaluate), update, iterations, ants, maximum_steps,
        initial_pheromone, pheromone_exponent, heuristic_exponent,
        random, std::move(hash), std::move(compare)
    );
}

}  // namespace approximate::evolutionary

#endif  // CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_ANT_COLONY_OPTIMIZATION_HPP_INCLUDED
