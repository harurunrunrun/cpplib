#ifndef CPPLIB_SRC_APPROXIMATE_DP_APPROXIMATE_STATE_DP_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_DP_APPROXIMATE_STATE_DP_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace approximate::dp {

template<class State>
struct StateDpResult {
    std::vector<State> states;
    std::size_t layers_completed = 0;
    bool interrupted = false;
};

namespace state_dp_detail {

inline void require_positive_limit(std::size_t limit, const char* name){
    if(limit == 0)[[unlikely]] throw std::invalid_argument(name);
}

template<class State, class Expand>
[[nodiscard]] std::vector<State> expand_layer(
    const std::vector<State>& states,
    std::size_t layer,
    Expand& expand
){
    std::vector<State> next;
    for(const State& state : states){
        auto generated = std::invoke(expand, state, layer);
        for(auto&& candidate : generated){
            next.emplace_back(
                std::forward<decltype(candidate)>(candidate)
            );
        }
    }
    return next;
}

template<class State, class Score>
void trim_ranked(
    std::vector<State>& states,
    std::size_t limit,
    Score& score,
    bool use_selection
){
    if(states.size() <= limit) return;
    using ScoreValue = std::decay_t<
        std::invoke_result_t<Score&, const State&>
    >;
    struct Entry {
        ScoreValue value;
        std::size_t ordinal = 0;
    };
    std::vector<Entry> ranking;
    ranking.reserve(states.size());
    for(std::size_t index = 0; index < states.size(); ++index){
        ranking.push_back({std::invoke(score, states[index]), index});
    }
    const auto better = [](const Entry& left, const Entry& right){
        if(left.value < right.value) return false;
        if(right.value < left.value) return true;
        return left.ordinal < right.ordinal;
    };
    if(use_selection){
        std::nth_element(
            ranking.begin(), ranking.begin() + static_cast<std::ptrdiff_t>(limit), ranking.end(), better
        );
        ranking.resize(limit);
        std::sort(ranking.begin(), ranking.end(), better);
    }else{
        std::partial_sort(
            ranking.begin(), ranking.begin() + static_cast<std::ptrdiff_t>(limit), ranking.end(), better
        );
        ranking.resize(limit);
    }
    std::vector<State> kept;
    kept.reserve(limit);
    for(const Entry& entry : ranking){
        kept.push_back(std::move(states[entry.ordinal]));
    }
    states = std::move(kept);
}

struct VectorIntegerHash {
    [[nodiscard]] std::size_t operator()(
        const std::vector<std::int64_t>& values
    ) const noexcept {
        std::size_t result = static_cast<std::size_t>(
            0x9e3779b97f4a7c15ULL
        );
        for(const std::int64_t value : values){
            const std::size_t part = std::hash<std::int64_t>{}(value);
            result ^= part + static_cast<std::size_t>(0x9e3779b97f4a7c15ULL)
                + (result << 6U) + (result >> 2U);
        }
        return result;
    }
};

template<class State, class Objectives>
[[nodiscard]] std::pair<std::vector<std::int64_t>, long double> epsilon_box(
    const State& state,
    Objectives& objectives,
    long double logarithm
){
    auto values = std::invoke(objectives, state);
    std::vector<std::int64_t> box;
    long double total = 0.0L;
    for(const auto raw_value : values){
        const long double value = static_cast<long double>(raw_value);
        if(!std::isfinite(value) || value < 0.0L)[[unlikely]]{
            throw std::invalid_argument(
                "epsilon-dominance objectives must be finite and nonnegative"
            );
        }
        total += value;
        if(value == 0.0L){
            box.push_back(std::numeric_limits<std::int64_t>::min());
            continue;
        }
        const long double coordinate = std::floor(std::log(value) / logarithm);
        if(coordinate < static_cast<long double>(
                std::numeric_limits<std::int64_t>::min() + 1)
           || coordinate > static_cast<long double>(
                std::numeric_limits<std::int64_t>::max()))[[unlikely]]{
            throw std::overflow_error(
                "epsilon-dominance box coordinate is not representable"
            );
        }
        box.push_back(static_cast<std::int64_t>(coordinate));
    }
    if(!std::isfinite(total))[[unlikely]]{
        throw std::overflow_error(
            "the objective sum is not finite"
        );
    }
    if(box.empty())[[unlikely]]{
        throw std::invalid_argument("an objective vector must not be empty");
    }
    return {std::move(box), total};
}

inline long double epsilon_logarithm(long double epsilon){
    if(!std::isfinite(epsilon) || !(epsilon > 0.0L))[[unlikely]]{
        throw std::invalid_argument("epsilon must be finite and positive");
    }
    return std::log1p(epsilon);
}

}  // namespace state_dp_detail

/**
 * Beam DP retaining the best `beam_width` states after each transition layer.
 * Complexity per layer is O(N + beam_width log beam_width), plus transitions.
 */
template<class State, class Expand, class Score>
[[nodiscard]] StateDpResult<State> beam_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    Score score,
    std::size_t beam_width
){
    state_dp_detail::require_positive_limit(
        beam_width, "beam_width must be positive"
    );
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        result.states =
            state_dp_detail::expand_layer(result.states, layer, expand);
        state_dp_detail::trim_ranked(
            result.states, beam_width, score, true
        );
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Top-K DP using partial sorting at each layer.
 * Complexity per layer is O(N log K), plus transitions.
 */
template<class State, class Expand, class Score>
[[nodiscard]] StateDpResult<State> top_k_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    Score score,
    std::size_t k
){
    state_dp_detail::require_positive_limit(k, "k must be positive");
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        result.states =
            state_dp_detail::expand_layer(result.states, layer, expand);
        state_dp_detail::trim_ranked(result.states, k, score, false);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Sparse DP retaining the highest-scoring representative for every exact key.
 * Expected complexity per layer is O(N), plus transitions.
 */
template<
    class State,
    class Expand,
    class KeyFunction,
    class Score,
    class Key = std::decay_t<
        std::invoke_result_t<KeyFunction&, const State&>
    >,
    class Hash = std::hash<Key>
>
[[nodiscard]] StateDpResult<State> sparse_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    KeyFunction key,
    Score score,
    Hash hash = {}
){
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> candidates =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::unordered_map<Key, std::size_t, Hash> positions(
            0, hash
        );
        std::vector<State> kept;
        kept.reserve(candidates.size());
        for(State& candidate : candidates){
            Key candidate_key = std::invoke(key, candidate);
            const auto [iterator, inserted] =
                positions.emplace(std::move(candidate_key), kept.size());
            if(inserted){
                kept.push_back(std::move(candidate));
            }else if(std::invoke(score, kept[iterator->second])
                     < std::invoke(score, candidate)){
                kept[iterator->second] = std::move(candidate);
            }
        }
        result.states = std::move(kept);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Hash-beam DP retaining the best state in each hash bucket, followed by an
 * optional score beam.  Expected complexity per layer is O(N + B log B).
 */
template<class State, class Expand, class Score, class Hash = std::hash<State>>
[[nodiscard]] StateDpResult<State> hash_beam_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    Score score,
    std::size_t bucket_count,
    std::size_t beam_width,
    Hash hash = {}
){
    state_dp_detail::require_positive_limit(
        bucket_count, "bucket_count must be positive"
    );
    state_dp_detail::require_positive_limit(
        beam_width, "beam_width must be positive"
    );
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> candidates =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::vector<std::optional<State>> buckets(bucket_count);
        for(State& candidate : candidates){
            const std::size_t bucket =
                static_cast<std::size_t>(std::invoke(hash, candidate))
                % bucket_count;
            if(!buckets[bucket].has_value()
               || std::invoke(score, *buckets[bucket])
                    < std::invoke(score, candidate)){
                buckets[bucket] = std::move(candidate);
            }
        }
        result.states.clear();
        for(auto& bucket : buckets){
            if(bucket.has_value()) result.states.push_back(std::move(*bucket));
        }
        state_dp_detail::trim_ranked(
            result.states, beam_width, score, true
        );
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Hard state-count-limited DP.  Candidates are admitted in transition order
 * until the per-layer limit is reached.
 *
 * Complexity is O(layer_count * maximum_states * branching_factor) time and
 * O(maximum_states) retained space.
 */
template<class State, class Expand>
[[nodiscard]] StateDpResult<State> state_count_limited_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    std::size_t maximum_states
){
    state_dp_detail::require_positive_limit(
        maximum_states, "maximum_states must be positive"
    );
    if(initial_states.size() > maximum_states){
        initial_states.resize(maximum_states);
    }
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> next;
        next.reserve(maximum_states);
        for(const State& state : result.states){
            auto generated = std::invoke(expand, state, layer);
            for(auto&& candidate : generated){
                next.emplace_back(
                    std::forward<decltype(candidate)>(candidate)
                );
                if(next.size() == maximum_states) break;
            }
            if(next.size() == maximum_states) break;
        }
        result.states = std::move(next);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Value-rounding DP retaining the highest-value state in each additive bucket.
 * Expected complexity per layer is O(N), plus transitions.
 */
template<class State, class Expand, class ValueFunction>
[[nodiscard]] StateDpResult<State> value_rounding_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    ValueFunction value,
    long double quantum
){
    if(!std::isfinite(quantum) || !(quantum > 0.0L))[[unlikely]]{
        throw std::invalid_argument("rounding quantum must be finite and positive");
    }
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> candidates =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::unordered_map<std::int64_t, std::size_t> positions;
        std::vector<State> kept;
        for(State& candidate : candidates){
            const long double candidate_value =
                static_cast<long double>(std::invoke(value, candidate));
            if(!std::isfinite(candidate_value))[[unlikely]]{
                throw std::invalid_argument("rounded DP values must be finite");
            }
            const long double quotient = std::floor(candidate_value / quantum);
            if(quotient < static_cast<long double>(
                    std::numeric_limits<std::int64_t>::min())
               || quotient > static_cast<long double>(
                    std::numeric_limits<std::int64_t>::max()))[[unlikely]]{
                throw std::overflow_error("rounded DP bucket is not representable");
            }
            const std::int64_t bucket = static_cast<std::int64_t>(quotient);
            const auto [iterator, inserted] =
                positions.emplace(bucket, kept.size());
            if(inserted){
                kept.push_back(std::move(candidate));
            }else if(std::invoke(value, kept[iterator->second])
                     < std::invoke(value, candidate)){
                kept[iterator->second] = std::move(candidate);
            }
        }
        result.states = std::move(kept);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Canonical-state compression followed by exact duplicate removal.
 * Expected complexity per layer is O(N), plus compression and transitions.
 */
template<
    class State,
    class Expand,
    class Compress,
    class Hash = std::hash<State>,
    class Equal = std::equal_to<State>
>
[[nodiscard]] StateDpResult<State> state_compression_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    Compress compress,
    Hash hash = {},
    Equal equal = {}
){
    std::unordered_set<State, Hash, Equal> initial_seen(0, hash, equal);
    std::vector<State> canonical_initial;
    canonical_initial.reserve(initial_states.size());
    for(State& state : initial_states){
        State canonical = std::invoke(compress, std::move(state));
        if(initial_seen.insert(canonical).second){
            canonical_initial.push_back(std::move(canonical));
        }
    }
    StateDpResult<State> result{std::move(canonical_initial), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> candidates =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::unordered_set<State, Hash, Equal> seen(0, hash, equal);
        std::vector<State> kept;
        for(State& candidate : candidates){
            State canonical = std::invoke(compress, std::move(candidate));
            if(seen.insert(canonical).second){
                kept.push_back(std::move(canonical));
            }
        }
        result.states = std::move(kept);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Aggregates all states with the same key through a caller-supplied associative
 * merge operation.  Expected complexity per layer is O(N), plus aggregation.
 */
template<
    class State,
    class Expand,
    class KeyFunction,
    class Aggregate,
    class Key = std::decay_t<
        std::invoke_result_t<KeyFunction&, const State&>
    >,
    class Hash = std::hash<Key>
>
[[nodiscard]] StateDpResult<State> state_aggregation_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    KeyFunction key,
    Aggregate aggregate,
    Hash hash = {}
){
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> candidates =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::unordered_map<Key, std::size_t, Hash> positions(0, hash);
        std::vector<State> kept;
        for(State& candidate : candidates){
            Key candidate_key = std::invoke(key, candidate);
            const auto [iterator, inserted] =
                positions.emplace(std::move(candidate_key), kept.size());
            if(inserted){
                kept.push_back(std::move(candidate));
            }else{
                kept[iterator->second] = std::invoke(
                    aggregate,
                    std::move(kept[iterator->second]),
                    std::move(candidate)
                );
            }
        }
        result.states = std::move(kept);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Removes states dominated by another state after every layer.
 * Complexity per layer is O(N^2) dominance tests and O(N) retained space.
 */
template<class State, class Expand, class Dominates>
[[nodiscard]] StateDpResult<State> dominance_pruned_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    Dominates dominates
){
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> candidates =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::vector<State> frontier;
        for(State& candidate : candidates){
            bool rejected = false;
            for(const State& state : frontier){
                if(std::invoke(dominates, state, candidate)){
                    rejected = true;
                    break;
                }
            }
            if(rejected) continue;
            frontier.erase(
                std::remove_if(
                    frontier.begin(),
                    frontier.end(),
                    [&](const State& state){
                        return std::invoke(dominates, candidate, state);
                    }
                ),
                frontier.end()
            );
            frontier.push_back(std::move(candidate));
        }
        result.states = std::move(frontier);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Epsilon-dominance DP retaining one maximum-sum representative per
 * multiplicative objective box.
 *
 * Expected complexity per layer is O(ND), where D is objective dimension.
 */
template<class State, class Expand, class Objectives>
[[nodiscard]] StateDpResult<State> epsilon_dominance_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    Objectives objectives,
    long double epsilon
){
    const long double logarithm =
        state_dp_detail::epsilon_logarithm(epsilon);
    StateDpResult<State> result{std::move(initial_states), 0, false};
    std::optional<std::size_t> objective_dimension;
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        std::vector<State> candidates =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::unordered_map<
            std::vector<std::int64_t>,
            std::size_t,
            state_dp_detail::VectorIntegerHash
        > positions;
        std::vector<State> kept;
        std::vector<long double> totals;
        for(State& candidate : candidates){
            auto [box, total] = state_dp_detail::epsilon_box(
                candidate, objectives, logarithm
            );
            if(!objective_dimension.has_value()){
                objective_dimension = box.size();
            }else if(*objective_dimension != box.size())[[unlikely]]{
                throw std::invalid_argument(
                    "objective vector dimensions differ"
                );
            }
            const auto [iterator, inserted] =
                positions.emplace(std::move(box), kept.size());
            if(inserted){
                kept.push_back(std::move(candidate));
                totals.push_back(total);
            }else if(totals[iterator->second] < total){
                kept[iterator->second] = std::move(candidate);
                totals[iterator->second] = total;
            }
        }
        result.states = std::move(kept);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Builds an approximate Pareto frontier for nonnegative maximization
 * objectives: one representative per epsilon box, then exact dominance pruning.
 *
 * Complexity is O(ND + F^2D) time and O(FD) auxiliary space.
 */
template<class State, class Objectives>
[[nodiscard]] std::vector<State> approximate_pareto_frontier(
    const std::vector<State>& candidates,
    Objectives objectives,
    long double epsilon
){
    const long double logarithm =
        state_dp_detail::epsilon_logarithm(epsilon);
    std::unordered_map<
        std::vector<std::int64_t>,
        std::size_t,
        state_dp_detail::VectorIntegerHash
    > positions;
    std::vector<State> representatives;
    std::vector<std::vector<long double>> values;
    std::vector<long double> totals;
    for(const State& candidate : candidates){
        auto [box, total] =
            state_dp_detail::epsilon_box(candidate, objectives, logarithm);
        std::vector<long double> objective_values;
        for(const auto raw_value : std::invoke(objectives, candidate)){
            objective_values.push_back(static_cast<long double>(raw_value));
        }
        const auto [iterator, inserted] =
            positions.emplace(std::move(box), representatives.size());
        if(inserted){
            representatives.push_back(candidate);
            values.push_back(std::move(objective_values));
            totals.push_back(total);
        }else if(totals[iterator->second] < total){
            representatives[iterator->second] = candidate;
            values[iterator->second] = std::move(objective_values);
            totals[iterator->second] = total;
        }
    }
    std::vector<unsigned char> dominated(representatives.size(), 0);
    for(std::size_t left = 0; left < representatives.size(); ++left){
        if(dominated[left]) continue;
        for(std::size_t right = 0; right < representatives.size(); ++right){
            if(left == right || values[left].size() != values[right].size()){
                if(values[left].size() != values[right].size())[[unlikely]]{
                    throw std::invalid_argument(
                        "objective vector dimensions differ"
                    );
                }
                continue;
            }
            bool no_worse = true;
            bool strictly_better = false;
            for(std::size_t coordinate = 0;
                coordinate < values[left].size();
                ++coordinate){
                no_worse = no_worse
                    && values[right][coordinate] >= values[left][coordinate];
                strictly_better = strictly_better
                    || values[right][coordinate] > values[left][coordinate];
            }
            if(no_worse && strictly_better){
                dominated[left] = 1;
                break;
            }
        }
    }
    std::vector<State> result;
    for(std::size_t index = 0; index < representatives.size(); ++index){
        if(!dominated[index]) result.push_back(std::move(representatives[index]));
    }
    return result;
}

/**
 * Exact-layer DP that consults `stop(layer, states)` before each layer.
 * Complexity is the cost of the completed transition layers.
 */
template<class State, class Expand, class Stop>
[[nodiscard]] StateDpResult<State> interrupted_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    Stop stop
){
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        if(std::invoke(stop, layer, result.states)){
            result.interrupted = true;
            break;
        }
        result.states =
            state_dp_detail::expand_layer(result.states, layer, expand);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

/**
 * Stochastic beam DP uniformly shuffling all candidates before truncation.
 * Complexity per layer is O(N), plus transitions.
 */
template<class State, class Expand, class URBG>
[[nodiscard]] StateDpResult<State> stochastic_beam_dynamic_programming(
    std::vector<State> initial_states,
    std::size_t layer_count,
    Expand expand,
    std::size_t beam_width,
    URBG& random
){
    state_dp_detail::require_positive_limit(
        beam_width, "beam_width must be positive"
    );
    StateDpResult<State> result{std::move(initial_states), 0, false};
    for(std::size_t layer = 0; layer < layer_count; ++layer){
        result.states =
            state_dp_detail::expand_layer(result.states, layer, expand);
        std::shuffle(result.states.begin(), result.states.end(), random);
        if(result.states.size() > beam_width) result.states.resize(beam_width);
        ++result.layers_completed;
        if(result.states.empty()) break;
    }
    return result;
}

}  // namespace approximate::dp

#endif  // CPPLIB_SRC_APPROXIMATE_DP_APPROXIMATE_STATE_DP_HPP_INCLUDED
