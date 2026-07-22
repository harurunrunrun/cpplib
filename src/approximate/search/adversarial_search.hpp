#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ADVERSARIAL_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ADVERSARIAL_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

template<class State, class Score>
struct GameTreeSearchResult{
    Score score{};
    std::vector<State> principal_variation;
    std::size_t visited_nodes = 0;
};

namespace adversarial_search_internal{

template<class State, class Score>
struct ValueAndLine{
    Score score;
    std::vector<State> line;
};

template<class State, class Evaluate, class Children>
auto minimax_impl(
    const State& state,
    std::size_t depth,
    bool maximizing,
    Evaluate& evaluate,
    Children& children,
    std::size_t& visited
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    ++visited;
    if(depth == 0){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }

    auto next_states = std::invoke(children, state);
    std::optional<ValueAndLine<State, Score>> best;
    for(const auto& child : next_states){
        auto candidate = minimax_impl(
            child, depth - 1, !maximizing, evaluate, children, visited
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)){
            best = std::move(candidate);
        }
    }
    if(!best){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }
    best->line.insert(best->line.begin(), state);
    return *std::move(best);
}

template<class State, class Score, class Evaluate, class Children>
ValueAndLine<State, Score> alpha_beta_impl(
    const State& state,
    std::size_t depth,
    bool maximizing,
    Score alpha,
    Score beta,
    Evaluate& evaluate,
    Children& children,
    std::size_t& visited
){
    ++visited;
    if(depth == 0){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }

    auto next_states = std::invoke(children, state);
    std::optional<ValueAndLine<State, Score>> best;
    for(const auto& child : next_states){
        auto candidate = alpha_beta_impl<State, Score>(
            child, depth - 1, !maximizing, alpha, beta,
            evaluate, children, visited
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)){
            best = std::move(candidate);
        }
        if(maximizing){
            if(alpha < best->score) alpha = best->score;
        }else{
            if(best->score < beta) beta = best->score;
        }
        if(!(alpha < beta)) break;
    }
    if(!best){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }
    best->line.insert(best->line.begin(), state);
    return *std::move(best);
}

template<class State, class Score, class Evaluate, class Children>
ValueAndLine<State, Score> negamax_impl(
    const State& state,
    std::size_t depth,
    Evaluate& evaluate,
    Children& children,
    std::size_t& visited
){
    ++visited;
    if(depth == 0){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }

    auto next_states = std::invoke(children, state);
    std::optional<ValueAndLine<State, Score>> best;
    for(const auto& child : next_states){
        auto candidate = negamax_impl<State, Score>(
            child, depth - 1, evaluate, children, visited
        );
        candidate.score = -candidate.score;
        if(!best || best->score < candidate.score) best = std::move(candidate);
    }
    if(!best){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }
    best->line.insert(best->line.begin(), state);
    return *std::move(best);
}

template<class State, class Score, class Evaluate, class Children>
ValueAndLine<State, Score> pvs_impl(
    const State& state,
    std::size_t depth,
    Score alpha,
    Score beta,
    Evaluate& evaluate,
    Children& children,
    std::size_t& visited
){
    ++visited;
    if(depth == 0){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }

    auto next_states = std::invoke(children, state);
    std::optional<ValueAndLine<State, Score>> best;
    bool first = true;
    for(const auto& child : next_states){
        ValueAndLine<State, Score> candidate = [&]{
            if(first){
                auto value = pvs_impl<State, Score>(
                    child, depth - 1, -beta, -alpha,
                    evaluate, children, visited
                );
                value.score = -value.score;
                return value;
            }
            auto value = pvs_impl<State, Score>(
                child, depth - 1, static_cast<Score>(-alpha - Score{1}), -alpha,
                evaluate, children, visited
            );
            value.score = -value.score;
            if(alpha < value.score && value.score < beta){
                value = pvs_impl<State, Score>(
                    child, depth - 1, -beta, -alpha,
                    evaluate, children, visited
                );
                value.score = -value.score;
            }
            return value;
        }();
        first = false;
        if(!best || best->score < candidate.score) best = std::move(candidate);
        if(alpha < best->score) alpha = best->score;
        if(!(alpha < beta)) break;
    }
    if(!best){
        return ValueAndLine<State, Score>{std::invoke(evaluate, state), {state}};
    }
    best->line.insert(best->line.begin(), state);
    return *std::move(best);
}

} // namespace adversarial_search_internal

template<class State, class Evaluate, class Children>
auto minimax_search(
    const State& root,
    std::size_t depth,
    Evaluate evaluate,
    Children children,
    bool maximizing_player = true
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    GameTreeSearchResult<State, Score> result;
    auto value = adversarial_search_internal::minimax_impl(
        root, depth, maximizing_player, evaluate, children, result.visited_nodes
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    return result;
}

template<class State, class Evaluate, class Children>
auto alpha_beta_search(
    const State& root,
    std::size_t depth,
    Evaluate evaluate,
    Children children,
    bool maximizing_player = true
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    GameTreeSearchResult<State, Score> result;
    auto value = adversarial_search_internal::alpha_beta_impl<State, Score>(
        root, depth, maximizing_player,
        std::numeric_limits<Score>::lowest(), std::numeric_limits<Score>::max(),
        evaluate, children, result.visited_nodes
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    return result;
}

template<class State, class Evaluate, class Children>
auto negamax_search(
    const State& root,
    std::size_t depth,
    Evaluate evaluate,
    Children children
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    static_assert(
        std::is_signed_v<Score>, "negamax_search requires a signed score"
    );
    GameTreeSearchResult<State, Score> result;
    auto value = adversarial_search_internal::negamax_impl<State, Score>(
        root, depth, evaluate, children, result.visited_nodes
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    return result;
}

template<class State, class Evaluate, class Children>
auto principal_variation_search(
    const State& root,
    std::size_t depth,
    Evaluate evaluate,
    Children children
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    static_assert(
        std::is_integral_v<Score> && std::is_signed_v<Score>,
        "principal_variation_search requires a signed integral score"
    );
    GameTreeSearchResult<State, Score> result;
    const Score infinity = std::numeric_limits<Score>::max() / Score{4};
    auto value = adversarial_search_internal::pvs_impl<State, Score>(
        root, depth, -infinity, infinity,
        evaluate, children, result.visited_nodes
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    return result;
}

template<class State, class Evaluate, class Children>
auto negascout_search(
    const State& root,
    std::size_t depth,
    Evaluate evaluate,
    Children children
){
    return principal_variation_search(
        root, depth, std::move(evaluate), std::move(children)
    );
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ADVERSARIAL_SEARCH_HPP_INCLUDED
