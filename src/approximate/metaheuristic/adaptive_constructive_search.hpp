#ifndef CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADAPTIVE_CONSTRUCTIVE_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADAPTIVE_CONSTRUCTIVE_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "search_result.hpp"

namespace approximate::metaheuristic {

template<class State, class Neighborhood, class Evaluate, class KeyFunction,
         class Hash = std::hash<std::decay_t<
             std::invoke_result_t<KeyFunction&, const State&>
         >>,
         class Compare = std::less<>>
auto reactive_tabu_search(
    State initial_state,
    Neighborhood neighborhood,
    Evaluate evaluate,
    KeyFunction key,
    std::size_t iterations,
    std::size_t initial_tenure,
    std::size_t minimum_tenure,
    std::size_t maximum_tenure,
    std::size_t decrease_interval,
    Hash hash = {},
    Compare compare = {}
){
    if(minimum_tenure == 0 || initial_tenure < minimum_tenure
       || maximum_tenure < initial_tenure
       || decrease_interval == 0)[[unlikely]]{
        throw std::invalid_argument(
            "reactive_tabu_search has invalid tenure parameters"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    using Key = std::decay_t<
        std::invoke_result_t<KeyFunction&, const State&>
    >;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    std::unordered_map<Key, std::size_t, Hash> forbidden(0, hash);
    std::unordered_map<Key, std::size_t, Hash> last_visit(0, hash);
    last_visit.emplace(std::invoke(key, current), 0);
    std::size_t tenure = initial_tenure;
    std::size_t iterations_without_cycle = 0;

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto candidates = std::invoke(neighborhood, current);
        std::optional<std::pair<State, Score>> selected;
        for(auto&& item : candidates){
            State candidate = std::forward<decltype(item)>(item);
            Score candidate_score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            const Key candidate_key = std::invoke(key, candidate);
            const auto blocked = forbidden.find(candidate_key);
            const bool tabu = blocked != forbidden.end()
                && iteration < blocked->second;
            const bool aspiration =
                std::invoke(compare, candidate_score, result.score);
            if(tabu && !aspiration) continue;
            if(!selected
               || std::invoke(
                   compare, candidate_score, selected->second
               )){
                selected.emplace(
                    std::move(candidate), std::move(candidate_score)
                );
            }
        }
        result.iterations = iteration + 1;
        if(!selected) break;
        const Key previous = std::invoke(key, current);
        current = std::move(selected->first);
        current_score = std::move(selected->second);
        forbidden[previous] = iteration + tenure + 1;
        ++result.accepted_moves;

        const Key current_key = std::invoke(key, current);
        const auto seen = last_visit.find(current_key);
        if(seen != last_visit.end()){
            tenure = std::min(maximum_tenure, tenure + std::size_t{1});
            iterations_without_cycle = 0;
        }else if(++iterations_without_cycle == decrease_interval){
            if(minimum_tenure < tenure) --tenure;
            iterations_without_cycle = 0;
        }
        last_visit[current_key] = iteration + 1;
        if(std::invoke(compare, current_score, result.score)){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

template<class State, class Neighborhood, class Evaluate, class KeyFunction,
         class GranularFilter,
         class Hash = std::hash<std::decay_t<
             std::invoke_result_t<KeyFunction&, const State&>
         >>,
         class Compare = std::less<>>
auto granular_tabu_search(
    State initial_state,
    Neighborhood neighborhood,
    Evaluate evaluate,
    KeyFunction key,
    GranularFilter granular_filter,
    std::size_t iterations,
    std::size_t tabu_tenure,
    Hash hash = {},
    Compare compare = {}
){
    if(tabu_tenure == 0)[[unlikely]]{
        throw std::invalid_argument(
            "granular_tabu_search requires positive tenure"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    using Key = std::decay_t<
        std::invoke_result_t<KeyFunction&, const State&>
    >;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    std::unordered_map<Key, std::size_t, Hash> forbidden(
        0, std::move(hash)
    );
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto candidates = std::invoke(neighborhood, current);
        std::optional<std::pair<State, Score>> selected;
        for(auto&& item : candidates){
            State candidate = std::forward<decltype(item)>(item);
            if(!static_cast<bool>(std::invoke(
                   granular_filter, current, candidate
               ))){
                continue;
            }
            Score candidate_score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            const Key candidate_key = std::invoke(key, candidate);
            const auto blocked = forbidden.find(candidate_key);
            const bool tabu = blocked != forbidden.end()
                && iteration < blocked->second;
            const bool aspiration =
                std::invoke(compare, candidate_score, result.score);
            if(tabu && !aspiration) continue;
            if(!selected
               || std::invoke(
                   compare, candidate_score, selected->second
               )){
                selected.emplace(
                    std::move(candidate), std::move(candidate_score)
                );
            }
        }
        result.iterations = iteration + 1;
        if(!selected) break;
        const Key previous = std::invoke(key, current);
        current = std::move(selected->first);
        current_score = std::move(selected->second);
        forbidden[previous] = iteration + tabu_tenure + 1;
        ++result.accepted_moves;
        if(std::invoke(compare, current_score, result.score)){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

template<class Constructor, class LocalImprove, class Evaluate, class URBG,
         class Compare = std::less<>>
auto reactive_grasp(
    Constructor construct,
    LocalImprove local_improve,
    Evaluate evaluate,
    const std::vector<long double>& alpha_values,
    std::size_t iterations,
    std::size_t adaptation_interval,
    URBG& random,
    Compare compare = {}
){
    if(alpha_values.empty() || iterations == 0
       || adaptation_interval == 0)[[unlikely]]{
        throw std::invalid_argument(
            "reactive_grasp requires non-empty alpha values and positive "
            "iteration counts"
        );
    }
    for(const long double alpha : alpha_values){
        if(alpha < 0.0L || alpha > 1.0L
           || !std::isfinite(alpha))[[unlikely]]{
            throw std::invalid_argument(
                "reactive_grasp alpha values must be in [0, 1]"
            );
        }
    }
    using State = std::decay_t<
        std::invoke_result_t<Constructor&, long double, URBG&>
    >;
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    std::vector<long double> weights(alpha_values.size(), 1.0L);
    std::vector<long double> score_sum(alpha_values.size());
    std::vector<std::size_t> uses(alpha_values.size());
    std::optional<SearchResult<State, Score>> result;

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::discrete_distribution<std::size_t> choose(
            weights.begin(), weights.end()
        );
        const std::size_t selected = choose(random);
        State candidate = std::invoke(
            construct, alpha_values[selected], random
        );
        candidate = std::invoke(local_improve, std::move(candidate));
        Score candidate_score = std::invoke(evaluate, candidate);
        score_sum[selected] += static_cast<long double>(candidate_score);
        ++uses[selected];
        if(!result){
            result.emplace(SearchResult<State, Score>{
                candidate, candidate_score, iteration + 1, 1, 0
            });
        }else{
            ++result->evaluations;
            result->iterations = iteration + 1;
            if(std::invoke(compare, candidate_score, result->score)){
                result->state = candidate;
                result->score = candidate_score;
                ++result->accepted_moves;
            }
        }
        if((iteration + 1) % adaptation_interval != 0) continue;
        const long double best =
            static_cast<long double>(result->score);
        for(std::size_t i = 0; i < weights.size(); ++i){
            if(uses[i] == 0) continue;
            const long double average =
                score_sum[i] / static_cast<long double>(uses[i]);
            weights[i] = 1.0L
                / (std::fabs(average - best) + 1.0e-12L);
            score_sum[i] = 0.0L;
            uses[i] = 0;
        }
    }
    return *result;
}

template<class State, class Destroy, class Repair, class Evaluate,
         class Accept, class URBG, class Compare = std::less<>>
auto large_neighborhood_search(
    State initial_state,
    Destroy destroy,
    Repair repair,
    Evaluate evaluate,
    Accept accept,
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
        State candidate = std::invoke(repair, std::move(partial), random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(!static_cast<bool>(std::invoke(
               accept, candidate_score, current_score, iteration, random
           ))){
            continue;
        }
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

template<class State, class Destroys, class Repairs, class Evaluate,
         class URBG, class Compare = std::less<>>
auto adaptive_large_neighborhood_search(
    State initial_state,
    const Destroys& destroys,
    const Repairs& repairs,
    Evaluate evaluate,
    std::size_t iterations,
    long double reaction_factor,
    URBG& random,
    Compare compare = {}
){
    const auto destroy_begin = std::begin(destroys);
    const auto repair_begin = std::begin(repairs);
    const std::size_t destroy_count = static_cast<std::size_t>(
        std::distance(destroy_begin, std::end(destroys))
    );
    const std::size_t repair_count = static_cast<std::size_t>(
        std::distance(repair_begin, std::end(repairs))
    );
    if(destroy_count == 0 || repair_count == 0
       || !(reaction_factor > 0.0L && reaction_factor <= 1.0L)
       || !std::isfinite(reaction_factor))[[unlikely]]{
        throw std::invalid_argument(
            "adaptive_large_neighborhood_search has invalid operators "
            "or reaction factor"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::vector<long double> destroy_weights(destroy_count, 1.0L);
    std::vector<long double> repair_weights(repair_count, 1.0L);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::discrete_distribution<std::size_t> choose_destroy(
            destroy_weights.begin(), destroy_weights.end()
        );
        std::discrete_distribution<std::size_t> choose_repair(
            repair_weights.begin(), repair_weights.end()
        );
        const std::size_t destroy_index = choose_destroy(random);
        const std::size_t repair_index = choose_repair(random);
        auto destroy = destroy_begin;
        auto repair = repair_begin;
        std::advance(
            destroy,
            static_cast<typename std::iterator_traits<
                decltype(destroy)
            >::difference_type>(destroy_index)
        );
        std::advance(
            repair,
            static_cast<typename std::iterator_traits<
                decltype(repair)
            >::difference_type>(repair_index)
        );
        auto partial = std::invoke(*destroy, current, random);
        State candidate = std::invoke(*repair, std::move(partial), random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        long double reward = 0.1L;
        if(std::invoke(compare, candidate_score, current_score)){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            reward = 2.0L;
            if(std::invoke(compare, current_score, result.score)){
                result.state = current;
                result.score = current_score;
                reward = 5.0L;
            }
        }
        destroy_weights[destroy_index] =
            (1.0L - reaction_factor) * destroy_weights[destroy_index]
            + reaction_factor * reward;
        repair_weights[repair_index] =
            (1.0L - reaction_factor) * repair_weights[repair_index]
            + reaction_factor * reward;
    }
    return result;
}

template<class State, class Ruin, class Recreate, class Evaluate,
         class URBG, class Compare = std::less<>>
auto ruin_and_recreate(
    State initial_state,
    Ruin ruin,
    Recreate recreate,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t initial_ruin_size,
    std::size_t maximum_ruin_size,
    URBG& random,
    Compare compare = {}
){
    if(initial_ruin_size == 0
       || maximum_ruin_size < initial_ruin_size)[[unlikely]]{
        throw std::invalid_argument(
            "ruin_and_recreate has invalid ruin sizes"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::size_t ruin_size = initial_ruin_size;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto partial = std::invoke(ruin, current, ruin_size, random);
        State candidate = std::invoke(
            recreate, std::move(partial), random
        );
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(std::invoke(compare, candidate_score, current_score)){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ruin_size = initial_ruin_size;
            ++result.accepted_moves;
            if(std::invoke(compare, current_score, result.score)){
                result.state = current;
                result.score = current_score;
            }
        }else if(ruin_size < maximum_ruin_size){
            ++ruin_size;
        }
    }
    return result;
}

template<class State, class Combine, class LocalImprove, class Evaluate,
         class URBG, class Compare = std::less<>>
auto scatter_search(
    std::vector<State> reference_set,
    Combine combine,
    LocalImprove local_improve,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t reference_set_size,
    URBG& random,
    Compare compare = {}
){
    if(reference_set.empty() || reference_set_size == 0)[[unlikely]]{
        throw std::invalid_argument(
            "scatter_search requires a non-empty reference set"
        );
    }
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const State&>
    >;
    std::vector<Score> scores;
    scores.reserve(reference_set.size());
    for(const State& state : reference_set){
        scores.push_back(std::invoke(evaluate, state));
    }
    std::size_t evaluations = reference_set.size();
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::vector<State> pool = reference_set;
        std::vector<Score> pool_scores = scores;
        for(std::size_t first = 0; first < reference_set.size(); ++first){
            for(std::size_t second = first + 1;
                second < reference_set.size();
                ++second){
                State candidate = std::invoke(
                    combine, reference_set[first],
                    reference_set[second], random
                );
                candidate = std::invoke(
                    local_improve, std::move(candidate)
                );
                pool_scores.push_back(std::invoke(evaluate, candidate));
                pool.push_back(std::move(candidate));
                ++evaluations;
            }
        }
        std::vector<std::size_t> order(pool.size());
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::stable_sort(
            order.begin(), order.end(),
            [&](std::size_t lhs, std::size_t rhs){
                return std::invoke(
                    compare, pool_scores[lhs], pool_scores[rhs]
                );
            }
        );
        const std::size_t keep =
            std::min(reference_set_size, pool.size());
        reference_set.clear();
        scores.clear();
        reference_set.reserve(keep);
        scores.reserve(keep);
        for(std::size_t i = 0; i < keep; ++i){
            reference_set.push_back(std::move(pool[order[i]]));
            scores.push_back(std::move(pool_scores[order[i]]));
        }
    }
    std::size_t best = 0;
    for(std::size_t i = 1; i < scores.size(); ++i){
        if(std::invoke(compare, scores[i], scores[best])) best = i;
    }
    return SearchResult<State, Score>{
        reference_set[best], scores[best], iterations,
        evaluations, 0
    };
}

template<class State, class Complete, class Extensions, class Rollout,
         class Evaluate, class Compare = std::less<>>
auto pilot_method(
    State initial_partial,
    Complete complete,
    Extensions extensions,
    Rollout rollout,
    Evaluate evaluate,
    std::size_t maximum_steps,
    Compare compare = {}
){
    State initial_solution = std::invoke(rollout, initial_partial);
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const State&>
    >;
    Score initial_score = std::invoke(evaluate, initial_solution);
    SearchResult<State, Score> result{
        initial_solution, initial_score, 0, 1, 0
    };
    State partial = std::move(initial_partial);
    for(std::size_t step = 0;
        step < maximum_steps
        && !static_cast<bool>(std::invoke(complete, partial));
        ++step){
        auto candidates = std::invoke(extensions, partial);
        std::optional<State> selected_partial;
        std::optional<State> selected_solution;
        std::optional<Score> selected_score;
        for(auto&& item : candidates){
            State candidate_partial =
                std::forward<decltype(item)>(item);
            State candidate_solution = std::invoke(
                rollout, candidate_partial
            );
            Score candidate_score = std::invoke(
                evaluate, candidate_solution
            );
            ++result.evaluations;
            if(!selected_score
               || std::invoke(
                   compare, candidate_score, *selected_score
               )){
                selected_partial = std::move(candidate_partial);
                selected_solution = std::move(candidate_solution);
                selected_score = std::move(candidate_score);
            }
        }
        result.iterations = step + 1;
        if(!selected_partial) break;
        partial = std::move(*selected_partial);
        ++result.accepted_moves;
        if(std::invoke(compare, *selected_score, result.score)){
            result.state = std::move(*selected_solution);
            result.score = std::move(*selected_score);
        }
    }
    return result;
}

template<class Parameters, class Sample, class Evaluate, class Update,
         class URBG, class Compare = std::less<>>
auto cross_entropy_method(
    Parameters parameters,
    Sample sample,
    Evaluate evaluate,
    Update update,
    std::size_t iterations,
    std::size_t population_size,
    std::size_t elite_count,
    URBG& random,
    Compare compare = {}
){
    if(population_size == 0 || elite_count == 0
       || population_size < elite_count)[[unlikely]]{
        throw std::invalid_argument(
            "cross_entropy_method has invalid population or elite size"
        );
    }
    using State = std::decay_t<
        std::invoke_result_t<Sample&, const Parameters&, URBG&>
    >;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const State&>
    >;
    std::optional<SearchResult<State, Score>> result;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::vector<State> population;
        std::vector<Score> scores;
        population.reserve(population_size);
        scores.reserve(population_size);
        for(std::size_t i = 0; i < population_size; ++i){
            State state = std::invoke(sample, parameters, random);
            Score score = std::invoke(evaluate, state);
            population.push_back(std::move(state));
            scores.push_back(std::move(score));
        }
        std::vector<std::size_t> order(population_size);
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::stable_sort(
            order.begin(), order.end(),
            [&](std::size_t lhs, std::size_t rhs){
                return std::invoke(compare, scores[lhs], scores[rhs]);
            }
        );
        if(!result){
            result.emplace(SearchResult<State, Score>{
                population[order.front()], scores[order.front()],
                iteration + 1, population_size, 0
            });
        }else{
            result->iterations = iteration + 1;
            result->evaluations += population_size;
            if(std::invoke(
                   compare, scores[order.front()], result->score
               )){
                result->state = population[order.front()];
                result->score = scores[order.front()];
                ++result->accepted_moves;
            }
        }
        std::vector<State> elites;
        elites.reserve(elite_count);
        for(std::size_t i = 0; i < elite_count; ++i){
            elites.push_back(std::move(population[order[i]]));
        }
        parameters = std::invoke(
            update, std::as_const(elites), std::move(parameters)
        );
    }
    if(!result)[[unlikely]]{
        throw std::invalid_argument(
            "cross_entropy_method requires a positive iteration count"
        );
    }
    return *result;
}

}  // namespace approximate::metaheuristic

#endif  // CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADAPTIVE_CONSTRUCTIVE_SEARCH_HPP_INCLUDED
