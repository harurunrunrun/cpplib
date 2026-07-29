#ifndef CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADVANCED_NEIGHBORHOOD_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADVANCED_NEIGHBORHOOD_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "search_result.hpp"

namespace approximate::metaheuristic {

template<class State, class Shakings, class LocalImprove, class Evaluate,
         class Distance, class URBG, class Compare = std::less<>>
auto skewed_variable_neighborhood_search(
    State initial_state,
    const Shakings& shakings,
    LocalImprove local_improve,
    Evaluate evaluate,
    Distance distance,
    std::size_t iterations,
    long double distance_weight,
    URBG& random,
    Compare compare = {}
){
    if(distance_weight < 0.0L || !std::isfinite(distance_weight))[[unlikely]]{
        throw std::invalid_argument(
            "skewed_variable_neighborhood_search requires a finite "
            "non-negative distance weight"
        );
    }
    State current = std::invoke(local_improve, std::move(initial_state));
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    const auto begin = std::begin(shakings);
    const std::size_t count = static_cast<std::size_t>(
        std::distance(begin, std::end(shakings))
    );
    if(count == 0) return result;

    std::size_t neighborhood = 0;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto selected = begin;
        std::advance(
            selected,
            static_cast<typename std::iterator_traits<
                decltype(selected)
            >::difference_type>(neighborhood)
        );
        State candidate = std::invoke(*selected, current, random);
        candidate = std::invoke(local_improve, std::move(candidate));
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        result.iterations = iteration + 1;
        const long double separation = static_cast<long double>(
            std::invoke(distance, current, candidate)
        );
        if(separation < 0.0L || !std::isfinite(separation))[[unlikely]]{
            throw std::invalid_argument(
                "skewed VNS requires finite non-negative distances"
            );
        }
        const long double skewed_score =
            static_cast<long double>(candidate_score)
            - distance_weight * separation;
        if(skewed_score < static_cast<long double>(current_score)){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            neighborhood = 0;
            ++result.accepted_moves;
            if(std::invoke(compare, current_score, result.score)){
                result.state = current;
                result.score = current_score;
            }
        }else{
            neighborhood = (neighborhood + 1) % count;
        }
    }
    return result;
}

template<class State, class Neighborhood, class Evaluate, class Features,
         class Hash = std::hash<std::decay_t<decltype(
             std::declval<decltype(*std::begin(std::declval<
                 std::invoke_result_t<Features&, const State&>&
             >()))>().first
         )>>>
auto guided_local_search(
    State initial_state,
    Neighborhood neighborhood,
    Evaluate evaluate,
    Features features,
    std::size_t iterations,
    long double penalty_weight,
    Hash hash = {}
){
    if(penalty_weight < 0.0L || !std::isfinite(penalty_weight))[[unlikely]]{
        throw std::invalid_argument(
            "guided_local_search requires a finite non-negative penalty weight"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    using FeatureRange = std::invoke_result_t<Features&, const State&>;
    using Feature = std::decay_t<decltype(
        *std::begin(std::declval<std::remove_reference_t<FeatureRange>&>())
    )>;
    using Key = std::decay_t<decltype(std::declval<Feature>().first)>;

    const auto augmented = [&](const State& state, const Score& score,
                               const std::unordered_map<Key, std::size_t, Hash>& penalties){
        long double value = static_cast<long double>(score);
        auto state_features = std::invoke(features, state);
        for(const auto& feature : state_features){
            const long double cost = static_cast<long double>(feature.second);
            if(cost < 0.0L || !std::isfinite(cost))[[unlikely]]{
                throw std::invalid_argument(
                    "guided_local_search requires finite non-negative feature costs"
                );
            }
            const auto found = penalties.find(feature.first);
            if(found != penalties.end()){
                value += penalty_weight
                    * static_cast<long double>(found->second) * cost;
            }
        }
        return value;
    };

    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    std::unordered_map<Key, std::size_t, Hash> penalties(
        0, std::move(hash)
    );
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto candidates = std::invoke(neighborhood, current);
        std::optional<std::pair<State, Score>> selected;
        long double selected_augmented =
            std::numeric_limits<long double>::infinity();
        for(auto&& item : candidates){
            State candidate = std::forward<decltype(item)>(item);
            Score score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            const long double value = augmented(candidate, score, penalties);
            if(value < selected_augmented){
                selected.emplace(std::move(candidate), std::move(score));
                selected_augmented = value;
            }
        }
        result.iterations = iteration + 1;
        if(!selected) break;
        current = std::move(selected->first);
        current_score = std::move(selected->second);
        ++result.accepted_moves;
        if(current_score < result.score){
            result.state = current;
            result.score = current_score;
        }

        auto current_features = std::invoke(features, current);
        std::optional<long double> maximum_utility;
        std::vector<Key> penalized;
        for(const auto& feature : current_features){
            const long double cost = static_cast<long double>(feature.second);
            const auto found = penalties.find(feature.first);
            const std::size_t penalty =
                found == penalties.end() ? 0 : found->second;
            const long double utility =
                cost / static_cast<long double>(penalty + 1);
            if(!maximum_utility || *maximum_utility < utility){
                maximum_utility = utility;
                penalized.assign(1, feature.first);
            }else if(utility == *maximum_utility){
                penalized.push_back(feature.first);
            }
        }
        for(const Key& key : penalized) ++penalties[key];
    }
    return result;
}

template<class State, class Destroy, class GreedyReconstruct, class Evaluate,
         class URBG, class Compare = std::less<>>
auto iterated_greedy(
    State initial_state,
    Destroy destroy,
    GreedyReconstruct reconstruct,
    Evaluate evaluate,
    std::size_t iterations,
    URBG& random,
    Compare compare = {}
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto partial = std::invoke(destroy, current, random);
        State candidate = std::invoke(
            reconstruct, std::move(partial), random
        );
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(!std::invoke(compare, candidate_score, current_score)) continue;
        current = std::move(candidate);
        current_score = std::move(candidate_score);
        ++result.accepted_moves;
        if(std::invoke(compare, current_score, result.score)){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

template<class State, class RandomNeighbor, class Evaluate, class URBG,
         class Compare = std::less<>>
auto step_counting_hill_climbing(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t steps_per_level,
    URBG& random,
    Compare compare = {}
){
    if(steps_per_level == 0)[[unlikely]]{
        throw std::invalid_argument(
            "step_counting_hill_climbing requires a positive step count"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    Score threshold = current_score;
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::size_t accepted_at_level = 0;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        State candidate = std::invoke(random_neighbor, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(!std::invoke(compare, threshold, candidate_score)){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            if(++accepted_at_level == steps_per_level){
                threshold = current_score;
                accepted_at_level = 0;
            }
            if(std::invoke(compare, current_score, result.score)){
                result.state = current;
                result.score = current_score;
            }
        }
    }
    return result;
}

template<class State, class Neighborhood, class Evaluate, class Violation>
auto strategic_oscillation(
    State initial_state,
    Neighborhood neighborhood,
    Evaluate evaluate,
    Violation violation,
    std::size_t iterations,
    long double initial_penalty,
    long double penalty_step
){
    if(initial_penalty < 0.0L || penalty_step < 0.0L
       || !std::isfinite(initial_penalty)
       || !std::isfinite(penalty_step))[[unlikely]]{
        throw std::invalid_argument(
            "strategic_oscillation requires finite non-negative penalties"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    long double penalty = initial_penalty;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto candidates = std::invoke(neighborhood, current);
        std::optional<std::pair<State, Score>> selected;
        long double selected_augmented =
            std::numeric_limits<long double>::infinity();
        long double selected_violation = 0.0L;
        for(auto&& item : candidates){
            State candidate = std::forward<decltype(item)>(item);
            Score candidate_score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            const long double infeasibility = static_cast<long double>(
                std::invoke(violation, candidate)
            );
            if(infeasibility < 0.0L
               || !std::isfinite(infeasibility))[[unlikely]]{
                throw std::invalid_argument(
                    "strategic_oscillation requires finite "
                    "non-negative violations"
                );
            }
            const long double augmented =
                static_cast<long double>(candidate_score)
                + penalty * infeasibility;
            if(augmented < selected_augmented){
                selected.emplace(
                    std::move(candidate), std::move(candidate_score)
                );
                selected_augmented = augmented;
                selected_violation = infeasibility;
            }
        }
        result.iterations = iteration + 1;
        if(!selected) break;
        current = std::move(selected->first);
        current_score = std::move(selected->second);
        ++result.accepted_moves;
        if(selected_violation > 0.0L){
            penalty += penalty_step;
        }else{
            penalty = std::max(0.0L, penalty - penalty_step);
        }
        if(current_score < result.score){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

template<class State, class Components, class ComponentFitness,
         class MutateComponent, class Evaluate, class URBG,
         class Compare = std::less<>>
auto extremal_optimization(
    State initial_state,
    Components components,
    ComponentFitness component_fitness,
    MutateComponent mutate_component,
    Evaluate evaluate,
    std::size_t iterations,
    long double tau,
    URBG& random,
    Compare compare = {}
){
    if(!(tau > 0.0L) || !std::isfinite(tau))[[unlikely]]{
        throw std::invalid_argument(
            "extremal_optimization requires finite positive tau"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    using ComponentRange = std::invoke_result_t<Components&, const State&>;
    using Component = std::decay_t<decltype(
        *std::begin(std::declval<std::remove_reference_t<ComponentRange>&>())
    )>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto range = std::invoke(components, current);
        std::vector<Component> ranked(std::begin(range), std::end(range));
        if(ranked.empty()) break;
        std::stable_sort(
            ranked.begin(), ranked.end(),
            [&](const Component& lhs, const Component& rhs){
                return static_cast<long double>(
                    std::invoke(component_fitness, current, rhs)
                ) < static_cast<long double>(
                    std::invoke(component_fitness, current, lhs)
                );
            }
        );
        std::vector<long double> weights(ranked.size());
        for(std::size_t rank = 0; rank < ranked.size(); ++rank){
            weights[rank] = std::pow(
                static_cast<long double>(rank + 1), -tau
            );
        }
        std::discrete_distribution<std::size_t> select(
            weights.begin(), weights.end()
        );
        State candidate = std::invoke(
            mutate_component, current, ranked[select(random)], random
        );
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        result.iterations = iteration + 1;
        current = std::move(candidate);
        current_score = std::move(candidate_score);
        ++result.accepted_moves;
        if(std::invoke(compare, current_score, result.score)){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

}  // namespace approximate::metaheuristic

#endif  // CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADVANCED_NEIGHBORHOOD_SEARCH_HPP_INCLUDED
