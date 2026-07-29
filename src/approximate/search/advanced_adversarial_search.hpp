#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_ADVERSARIAL_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_ADVERSARIAL_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
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

namespace advanced_adversarial_search {

struct SearchStatistics {
    std::size_t visited_nodes = 0;
    std::size_t evaluated_nodes = 0;
    std::size_t cutoffs = 0;
    std::size_t re_searches = 0;
    std::size_t reduced_searches = 0;
    std::size_t heuristic_prunes = 0;
    std::size_t generated_nodes = 0;
    std::size_t simulations = 0;

    SearchStatistics& operator+=(const SearchStatistics& other) {
        visited_nodes += other.visited_nodes;
        evaluated_nodes += other.evaluated_nodes;
        cutoffs += other.cutoffs;
        re_searches += other.re_searches;
        reduced_searches += other.reduced_searches;
        heuristic_prunes += other.heuristic_prunes;
        generated_nodes += other.generated_nodes;
        simulations += other.simulations;
        return *this;
    }
};

template<class State, class Score>
struct SearchResult {
    Score score{};
    std::vector<State> principal_variation;
    SearchStatistics statistics;
    std::size_t completed_depth = 0;
};

template<class State, class Score>
struct MultiPlayerSearchResult {
    std::vector<Score> scores;
    std::vector<State> principal_variation;
    SearchStatistics statistics;
    std::size_t completed_depth = 0;
};

struct NullMoveOptions {
    std::size_t reduction = 2;
    std::size_t minimum_depth = 3;
};

struct LateMoveReductionOptions {
    std::size_t full_depth_moves = 2;
    std::size_t reduction = 1;
    std::size_t minimum_depth = 3;
};

template<class Score>
struct FutilityOptions {
    Score margin{};
    std::size_t maximum_depth = 1;
};

template<class Score>
struct RazoringOptions {
    Score margin{};
    std::size_t maximum_depth = 2;
    std::size_t quiescence_depth = 4;
};

template<class Score>
struct ProbCutOptions {
    Score margin{};
    std::size_t reduction = 2;
    std::size_t minimum_depth = 3;
};

template<class Score>
struct MultiProbCutOptions {
    std::vector<ProbCutOptions<Score>> probes;
};

namespace detail {

template<class State, class Score>
struct ValueLine {
    Score score{};
    std::vector<State> line;
};

template<class Range>
using range_value_t = std::decay_t<decltype(
    *std::begin(std::declval<std::remove_reference_t<Range>&>())
)>;

template<class State, class Range>
std::vector<State> collect_states(Range&& range) {
    std::vector<State> result;
    if constexpr(std::is_rvalue_reference_v<Range&&>) {
        for(auto& state : range) result.emplace_back(std::move(state));
    } else {
        for(const auto& state : range) result.emplace_back(state);
    }
    return result;
}

template<class Score>
constexpr Score negative_infinity() {
    if constexpr(std::numeric_limits<Score>::has_infinity) {
        return -std::numeric_limits<Score>::infinity();
    } else {
        return std::numeric_limits<Score>::lowest() / Score{4};
    }
}

template<class Score>
constexpr Score positive_infinity() {
    if constexpr(std::numeric_limits<Score>::has_infinity) {
        return std::numeric_limits<Score>::infinity();
    } else {
        return std::numeric_limits<Score>::max() / Score{4};
    }
}

template<class State, class Score>
void prepend(ValueLine<State, Score>& value, const State& state) {
    value.line.insert(value.line.begin(), state);
}

template<class State, class Score, class Evaluate>
ValueLine<State, Score> evaluate_leaf(
    const State& state, Evaluate& evaluate, SearchStatistics& statistics
) {
    ++statistics.evaluated_nodes;
    return {static_cast<Score>(std::invoke(evaluate, state)), {state}};
}

template<class State, class Score, class Evaluate, class Children>
ValueLine<State, Score> alpha_beta(
    const State& state, std::size_t depth, bool maximizing,
    Score alpha, Score beta, Evaluate& evaluate, Children& children,
    SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);

    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = alpha_beta<State, Score>(
            child, depth - 1, !maximizing, alpha, beta,
            evaluate, children, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) {
            best = std::move(candidate);
        }
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children>
ValueLine<State, Score> preferred_alpha_beta(
    const State& state, std::size_t depth, std::size_t ply, bool maximizing,
    Score alpha, Score beta, const std::vector<State>& preferred,
    Evaluate& evaluate, Children& children, SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    if(ply + 1 < preferred.size()) {
        const auto iterator = std::find(next.begin(), next.end(), preferred[ply + 1]);
        if(iterator != next.end()) std::iter_swap(next.begin(), iterator);
    }

    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = preferred_alpha_beta<State, Score>(
            child, depth - 1, ply + 1, !maximizing, alpha, beta, preferred,
            evaluate, children, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) {
            best = std::move(candidate);
        }
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class NoisyChildren>
ValueLine<State, Score> quiescence(
    const State& state, std::size_t depth, bool maximizing,
    Score alpha, Score beta, Evaluate& evaluate, NoisyChildren& noisy_children,
    SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    auto stand_pat = evaluate_leaf<State, Score>(state, evaluate, statistics);
    if(maximizing) {
        if(!(stand_pat.score < beta)) {
            ++statistics.cutoffs;
            return stand_pat;
        }
        if(alpha < stand_pat.score) alpha = stand_pat.score;
    } else {
        if(!(alpha < stand_pat.score)) {
            ++statistics.cutoffs;
            return stand_pat;
        }
        if(stand_pat.score < beta) beta = stand_pat.score;
    }
    if(depth == 0) return stand_pat;

    auto next = collect_states<State>(std::invoke(noisy_children, state));
    statistics.generated_nodes += next.size();
    ValueLine<State, Score> best = std::move(stand_pat);
    for(const auto& child : next) {
        auto candidate = quiescence<State, Score>(
            child, depth - 1, !maximizing, alpha, beta,
            evaluate, noisy_children, statistics
        );
        if(maximizing ? best.score < candidate.score : candidate.score < best.score) {
            best = std::move(candidate);
            prepend(best, state);
        }
        if(maximizing) {
            if(alpha < best.score) alpha = best.score;
        } else if(best.score < beta) {
            beta = best.score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    return best;
}

template<class State, class Score, class Evaluate, class Children, class NullMove>
ValueLine<State, Score> null_move(
    const State& state, std::size_t depth, bool maximizing,
    Score alpha, Score beta, bool allow_null, const NullMoveOptions& options,
    Evaluate& evaluate, Children& children, NullMove& null_transition,
    SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);

    if(allow_null && depth >= options.minimum_depth) {
        const State passed = std::invoke(null_transition, state);
        const std::size_t next_depth =
            depth > options.reduction + 1 ? depth - options.reduction - 1 : 0;
        auto probe = null_move<State, Score>(
            passed, next_depth, !maximizing, alpha, beta, false, options,
            evaluate, children, null_transition, statistics
        );
        if((maximizing && !(probe.score < beta))
           || (!maximizing && !(alpha < probe.score))) {
            ++statistics.cutoffs;
            ++statistics.heuristic_prunes;
            prepend(probe, state);
            return probe;
        }
    }

    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = null_move<State, Score>(
            child, depth - 1, !maximizing, alpha, beta, true, options,
            evaluate, children, null_transition, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) {
            best = std::move(candidate);
        }
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children>
ValueLine<State, Score> late_move_reduction(
    const State& state, std::size_t depth, bool maximizing,
    Score alpha, Score beta, const LateMoveReductionOptions& options,
    Evaluate& evaluate, Children& children, SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);

    std::optional<ValueLine<State, Score>> best;
    for(std::size_t index = 0; index < next.size(); ++index) {
        const bool reduce = depth >= options.minimum_depth
                         && index >= options.full_depth_moves
                         && options.reduction != 0
                         && depth > options.reduction + 1;
        const std::size_t searched_depth =
            reduce ? depth - 1 - options.reduction : depth - 1;
        if(reduce) ++statistics.reduced_searches;
        auto candidate = late_move_reduction<State, Score>(
            next[index], searched_depth, !maximizing, alpha, beta, options,
            evaluate, children, statistics
        );
        const bool improves = !best
            || (maximizing ? best->score < candidate.score
                           : candidate.score < best->score);
        if(reduce && improves && alpha < candidate.score && candidate.score < beta) {
            ++statistics.re_searches;
            candidate = late_move_reduction<State, Score>(
                next[index], depth - 1, !maximizing, alpha, beta, options,
                evaluate, children, statistics
            );
        }
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) {
            best = std::move(candidate);
        }
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children>
ValueLine<State, Score> futility(
    const State& state, std::size_t depth, bool maximizing,
    Score alpha, Score beta, const FutilityOptions<Score>& options,
    Evaluate& evaluate, Children& children, SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    if(depth <= options.maximum_depth) {
        auto static_value = evaluate_leaf<State, Score>(state, evaluate, statistics);
        const bool futile = maximizing
            ? static_cast<Score>(static_value.score + options.margin) <= alpha
            : beta <= static_cast<Score>(static_value.score - options.margin);
        if(futile) {
            ++statistics.heuristic_prunes;
            return static_value;
        }
    }
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = futility<State, Score>(
            child, depth - 1, !maximizing, alpha, beta, options,
            evaluate, children, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) best = std::move(candidate);
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children, class NoisyChildren>
ValueLine<State, Score> razoring(
    const State& state, std::size_t depth, bool maximizing,
    Score alpha, Score beta, const RazoringOptions<Score>& options,
    Evaluate& evaluate, Children& children, NoisyChildren& noisy_children,
    SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) {
        --statistics.visited_nodes;
        return quiescence<State, Score>(
            state, options.quiescence_depth, maximizing, alpha, beta,
            evaluate, noisy_children, statistics
        );
    }
    if(depth <= options.maximum_depth) {
        const Score static_score = static_cast<Score>(std::invoke(evaluate, state));
        ++statistics.evaluated_nodes;
        const bool near_bound = maximizing
            ? static_cast<Score>(static_score + options.margin) <= alpha
            : beta <= static_cast<Score>(static_score - options.margin);
        if(near_bound) {
            auto probe = quiescence<State, Score>(
                state, options.quiescence_depth, maximizing, alpha, beta,
                evaluate, noisy_children, statistics
            );
            if((maximizing && probe.score <= alpha)
               || (!maximizing && beta <= probe.score)) {
                ++statistics.heuristic_prunes;
                return probe;
            }
        }
    }
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = razoring<State, Score>(
            child, depth - 1, !maximizing, alpha, beta, options,
            evaluate, children, noisy_children, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) best = std::move(candidate);
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children>
ValueLine<State, Score> probcut(
    const State& state, std::size_t depth, bool maximizing,
    Score alpha, Score beta, const std::vector<ProbCutOptions<Score>>& probes,
    Evaluate& evaluate, Children& children, SearchStatistics& statistics,
    bool allow_probe
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    if(allow_probe) {
        for(const auto& option : probes) {
            if(option.reduction == 0 || depth < option.minimum_depth
               || depth <= option.reduction) continue;
            const std::size_t shallow_depth = depth - option.reduction;
            if(maximizing) {
                const Score threshold = static_cast<Score>(beta + option.margin);
                auto trial = probcut<State, Score>(
                    state, shallow_depth, maximizing,
                    static_cast<Score>(threshold - Score{1}), threshold, probes,
                    evaluate, children, statistics, false
                );
                if(!(trial.score < threshold)) {
                    ++statistics.cutoffs;
                    ++statistics.heuristic_prunes;
                    return trial;
                }
            } else {
                const Score threshold = static_cast<Score>(alpha - option.margin);
                auto trial = probcut<State, Score>(
                    state, shallow_depth, maximizing,
                    threshold, static_cast<Score>(threshold + Score{1}), probes,
                    evaluate, children, statistics, false
                );
                if(!(threshold < trial.score)) {
                    ++statistics.cutoffs;
                    ++statistics.heuristic_prunes;
                    return trial;
                }
            }
        }
    }
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = probcut<State, Score>(
            child, depth - 1, !maximizing, alpha, beta, probes,
            evaluate, children, statistics, true
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) best = std::move(candidate);
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

enum class OrderingKind { killer, history, countermove };

} // namespace detail

template<class Key, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class KillerHeuristic {
public:
    explicit KillerHeuristic(std::size_t maximum_ply = 0)
        : killers_(maximum_ply) {}

    void clear() { killers_.clear(); }
    void reserve_depth(std::size_t depth) {
        if(killers_.size() <= depth) killers_.resize(depth + 1);
    }
    void record(std::size_t ply, const Key& key) {
        reserve_depth(ply);
        auto& row = killers_[ply];
        if(row[0] && equal_(*row[0], key)) return;
        row[1] = std::move(row[0]);
        row[0] = key;
    }
    int rank(std::size_t ply, const Key& key) const {
        if(ply >= killers_.size()) return 0;
        if(killers_[ply][0] && equal_(*killers_[ply][0], key)) return 2;
        if(killers_[ply][1] && equal_(*killers_[ply][1], key)) return 1;
        return 0;
    }

private:
    std::vector<std::array<std::optional<Key>, 2>> killers_;
    [[no_unique_address]] Equal equal_;
};

template<class Key, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class HistoryHeuristic {
public:
    void clear() { scores_.clear(); }
    void record(const Key& key, std::size_t depth) {
        const std::uint64_t increment =
            static_cast<std::uint64_t>(depth) * static_cast<std::uint64_t>(depth);
        auto& value = scores_[key];
        if(std::numeric_limits<std::uint64_t>::max() - value < increment) {
            for(auto& entry : scores_) entry.second /= 2;
        }
        scores_[key] += increment;
    }
    std::uint64_t score(const Key& key) const {
        const auto iterator = scores_.find(key);
        return iterator == scores_.end() ? 0 : iterator->second;
    }

private:
    std::unordered_map<Key, std::uint64_t, Hash, Equal> scores_;
};

template<class Key, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class CountermoveHeuristic {
public:
    void clear() { replies_.clear(); }
    void record(const Key& previous, const Key& reply) { replies_[previous] = reply; }
    bool is_reply(const Key& previous, const Key& reply) const {
        const auto iterator = replies_.find(previous);
        return iterator != replies_.end() && equal_(iterator->second, reply);
    }

private:
    std::unordered_map<Key, Key, Hash, Equal> replies_;
    [[no_unique_address]] Equal equal_;
};

namespace detail {

template<OrderingKind Kind, class State, class Score, class Evaluate,
         class Children, class MoveKey, class Key, class Table>
ValueLine<State, Score> ordered_alpha_beta(
    const State& state, std::size_t depth, std::size_t ply, bool maximizing,
    Score alpha, Score beta, Evaluate& evaluate, Children& children,
    MoveKey& move_key, Table& table,
    const std::optional<Key>& previous, SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);

    std::stable_sort(next.begin(), next.end(), [&](const State& left, const State& right) {
        const Key left_key = std::invoke(move_key, state, left);
        const Key right_key = std::invoke(move_key, state, right);
        if constexpr(Kind == OrderingKind::killer) {
            return table.rank(ply, right_key) < table.rank(ply, left_key);
        } else if constexpr(Kind == OrderingKind::history) {
            return table.score(right_key) < table.score(left_key);
        } else {
            const int left_rank = previous && table.is_reply(*previous, left_key);
            const int right_rank = previous && table.is_reply(*previous, right_key);
            return right_rank < left_rank;
        }
    });

    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        const Key key = std::invoke(move_key, state, child);
        auto candidate = ordered_alpha_beta<Kind, State, Score>(
            child, depth - 1, ply + 1, !maximizing, alpha, beta,
            evaluate, children, move_key, table,
            std::optional<Key>{key}, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) best = std::move(candidate);
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            if constexpr(Kind == OrderingKind::killer) {
                table.record(ply, key);
            } else if constexpr(Kind == OrderingKind::history) {
                table.record(key, depth);
            } else if(previous) {
                table.record(*previous, key);
            }
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children>
ValueLine<State, Score> beam_minimax(
    const State& state, std::size_t depth, bool maximizing, std::size_t width,
    Evaluate& evaluate, Children& children, SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    if(next.size() > width) {
        const auto compare = [&](const State& left, const State& right) {
            const Score a = static_cast<Score>(std::invoke(evaluate, left));
            const Score b = static_cast<Score>(std::invoke(evaluate, right));
            statistics.evaluated_nodes += 2;
            return maximizing ? b < a : a < b;
        };
        std::partial_sort(next.begin(), next.begin() + static_cast<std::ptrdiff_t>(width),
                          next.end(), compare);
        statistics.heuristic_prunes += next.size() - width;
        next.resize(width);
    }
    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = beam_minimax<State, Score>(
            child, depth - 1, !maximizing, width, evaluate, children, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) best = std::move(candidate);
    }
    prepend(*best, state);
    return std::move(*best);
}

} // namespace detail

template<class State, class Evaluate, class Children>
auto mtdf_search(
    const State& root, std::size_t depth,
    std::decay_t<std::invoke_result_t<Evaluate&, const State&>> first_guess,
    Evaluate evaluate, Children children, bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    static_assert(std::is_integral_v<Score> && std::is_signed_v<Score>);
    SearchResult<State, Score> result;
    Score lower = detail::negative_infinity<Score>();
    Score upper = detail::positive_infinity<Score>();
    Score guess = first_guess;
    detail::ValueLine<State, Score> value;
    while(lower < upper) {
        const Score beta = guess == lower ? static_cast<Score>(guess + Score{1}) : guess;
        SearchStatistics pass;
        value = detail::alpha_beta<State, Score>(
            root, depth, maximizing_player, static_cast<Score>(beta - Score{1}), beta,
            evaluate, children, pass
        );
        result.statistics += pass;
        ++result.statistics.re_searches;
        guess = value.score;
        if(guess < beta) upper = guess;
        else lower = guess;
    }
    result.score = guess;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children>
auto aspiration_search(
    const State& root, std::size_t depth,
    std::decay_t<std::invoke_result_t<Evaluate&, const State&>> guess,
    std::decay_t<std::invoke_result_t<Evaluate&, const State&>> initial_window,
    Evaluate evaluate, Children children, bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(!(Score{} < initial_window)) {
        throw std::invalid_argument("aspiration_search requires a positive window");
    }
    SearchResult<State, Score> result;
    Score window = initial_window;
    const Score minimum = detail::negative_infinity<Score>();
    const Score maximum = detail::positive_infinity<Score>();
    detail::ValueLine<State, Score> value;
    while(true) {
        const Score alpha = guess - window < minimum ? minimum
            : static_cast<Score>(guess - window);
        const Score beta = maximum - window < guess ? maximum
            : static_cast<Score>(guess + window);
        SearchStatistics pass;
        value = detail::alpha_beta<State, Score>(
            root, depth, maximizing_player, alpha, beta,
            evaluate, children, pass
        );
        result.statistics += pass;
        if(alpha < value.score && value.score < beta) break;
        ++result.statistics.re_searches;
        if(alpha == minimum && beta == maximum) break;
        window = maximum / Score{2} < window ? maximum
            : static_cast<Score>(window + window);
    }
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children>
auto iterative_deepening_search(
    const State& root, std::size_t maximum_depth,
    Evaluate evaluate, Children children, bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    SearchResult<State, Score> result;
    std::vector<State> preferred;
    for(std::size_t depth = 0; depth <= maximum_depth; ++depth) {
        SearchStatistics pass;
        auto value = detail::preferred_alpha_beta<State, Score>(
            root, depth, 0, maximizing_player,
            detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
            preferred, evaluate, children, pass
        );
        result.statistics += pass;
        result.score = value.score;
        preferred = std::move(value.line);
        result.completed_depth = depth;
    }
    result.principal_variation = std::move(preferred);
    return result;
}

template<class State, class Evaluate, class NoisyChildren>
auto quiescence_search(
    const State& root, std::size_t maximum_noisy_depth,
    Evaluate evaluate, NoisyChildren noisy_children,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    SearchResult<State, Score> result;
    auto value = detail::quiescence<State, Score>(
        root, maximum_noisy_depth, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        evaluate, noisy_children, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = maximum_noisy_depth;
    return result;
}

template<class State, class Evaluate, class Children, class NullMove>
auto null_move_pruning_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    NullMove null_transition, NullMoveOptions options = {},
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(options.reduction == 0 || options.minimum_depth == 0)
        throw std::invalid_argument("null_move_pruning_search requires positive depths");
    SearchResult<State, Score> result;
    auto value = detail::null_move<State, Score>(
        root, depth, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(), true,
        options, evaluate, children, null_transition, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children>
auto late_move_reduction_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    LateMoveReductionOptions options = {}, bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(options.reduction == 0 || options.full_depth_moves == 0)
        throw std::invalid_argument("late_move_reduction_search requires positive settings");
    SearchResult<State, Score> result;
    auto value = detail::late_move_reduction<State, Score>(
        root, depth, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        options, evaluate, children, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children>
auto futility_pruning_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    FutilityOptions<std::decay_t<std::invoke_result_t<Evaluate&, const State&>>> options,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(options.margin < Score{} || options.maximum_depth == 0)
        throw std::invalid_argument("futility_pruning_search requires valid settings");
    SearchResult<State, Score> result;
    auto value = detail::futility<State, Score>(
        root, depth, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        options, evaluate, children, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children, class NoisyChildren>
auto razoring_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    NoisyChildren noisy_children,
    RazoringOptions<std::decay_t<std::invoke_result_t<Evaluate&, const State&>>> options,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(options.margin < Score{} || options.maximum_depth == 0)
        throw std::invalid_argument("razoring_search requires valid settings");
    SearchResult<State, Score> result;
    auto value = detail::razoring<State, Score>(
        root, depth, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        options, evaluate, children, noisy_children, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children>
auto probcut_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    ProbCutOptions<std::decay_t<std::invoke_result_t<Evaluate&, const State&>>> options,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(options.margin < Score{} || options.reduction == 0)
        throw std::invalid_argument("probcut_search requires valid settings");
    SearchResult<State, Score> result;
    const std::vector<ProbCutOptions<Score>> probes{options};
    auto value = detail::probcut<State, Score>(
        root, depth, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        probes, evaluate, children, result.statistics, true
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children>
auto multi_probcut_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    MultiProbCutOptions<std::decay_t<std::invoke_result_t<Evaluate&, const State&>>> options,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(options.probes.empty()) {
        throw std::invalid_argument("multi_probcut_search requires at least one probe");
    }
    for(const auto& probe : options.probes) {
        if(probe.margin < Score{} || probe.reduction == 0)
            throw std::invalid_argument("multi_probcut_search requires valid probes");
    }
    SearchResult<State, Score> result;
    auto value = detail::probcut<State, Score>(
        root, depth, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        options.probes, evaluate, children, result.statistics, true
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children, class MoveKey,
         class Key, class Hash, class Equal>
auto killer_heuristic_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    MoveKey move_key, KillerHeuristic<Key, Hash, Equal>& heuristic,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    SearchResult<State, Score> result;
    auto value = detail::ordered_alpha_beta<
        detail::OrderingKind::killer, State, Score
    >(
        root, depth, 0, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        evaluate, children, move_key, heuristic,
        std::optional<Key>{}, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children, class MoveKey,
         class Key, class Hash, class Equal>
auto history_heuristic_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    MoveKey move_key, HistoryHeuristic<Key, Hash, Equal>& heuristic,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    SearchResult<State, Score> result;
    auto value = detail::ordered_alpha_beta<
        detail::OrderingKind::history, State, Score
    >(
        root, depth, 0, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        evaluate, children, move_key, heuristic,
        std::optional<Key>{}, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children, class MoveKey,
         class Key, class Hash, class Equal>
auto countermove_heuristic_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    MoveKey move_key, CountermoveHeuristic<Key, Hash, Equal>& heuristic,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    SearchResult<State, Score> result;
    auto value = detail::ordered_alpha_beta<
        detail::OrderingKind::countermove, State, Score
    >(
        root, depth, 0, maximizing_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        evaluate, children, move_key, heuristic,
        std::optional<Key>{}, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children>
auto beam_minimax_search(
    const State& root, std::size_t depth, std::size_t beam_width,
    Evaluate evaluate, Children children, bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(beam_width == 0) throw std::invalid_argument("beam_minimax_search requires width > 0");
    SearchResult<State, Score> result;
    auto value = detail::beam_minimax<State, Score>(
        root, depth, maximizing_player, beam_width,
        evaluate, children, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

enum class ExpectimaxNode { maximum, minimum, chance };

namespace detail {

template<class State, class Score, class Evaluate, class Children,
         class NodeType, class Probability>
ValueLine<State, Score> expectimax(
    const State& state, std::size_t depth, Evaluate& evaluate, Children& children,
    NodeType& node_type, Probability& probability, SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    const ExpectimaxNode type = std::invoke(node_type, state);
    if(type == ExpectimaxNode::chance) {
        std::vector<ValueLine<State, Score>> values;
        values.reserve(next.size());
        std::vector<long double> weights;
        weights.reserve(next.size());
        long double total = 0;
        for(const auto& child : next) {
            const long double weight =
                static_cast<long double>(std::invoke(probability, state, child));
            if(weight < 0 || !std::isfinite(weight))
                throw std::invalid_argument("expectimax_search requires finite non-negative probabilities");
            total += weight;
            weights.push_back(weight);
            values.push_back(expectimax<State, Score>(
                child, depth - 1, evaluate, children, node_type, probability, statistics
            ));
        }
        if(!(0 < total))
            throw std::invalid_argument("expectimax_search requires positive total probability");
        long double expected = 0;
        std::size_t representative = 0;
        for(std::size_t i = 0; i < values.size(); ++i) {
            expected += weights[i] * static_cast<long double>(values[i].score);
            if(weights[representative] < weights[i]) representative = i;
        }
        auto result = std::move(values[representative]);
        result.score = static_cast<Score>(expected / total);
        prepend(result, state);
        return result;
    }
    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = expectimax<State, Score>(
            child, depth - 1, evaluate, children, node_type, probability, statistics
        );
        const bool maximum = type == ExpectimaxNode::maximum;
        if(!best || (maximum ? best->score < candidate.score
                            : candidate.score < best->score)) best = std::move(candidate);
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children, class Player>
MultiPlayerSearchResult<State, Score> maxn(
    const State& state, std::size_t depth, std::size_t player_count,
    Evaluate& evaluate, Children& children, Player& player,
    SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    auto leaf = [&] {
        ++statistics.evaluated_nodes;
        auto scores = std::invoke(evaluate, state);
        std::vector<Score> values(std::begin(scores), std::end(scores));
        if(values.size() != player_count)
            throw std::invalid_argument("maxn_search evaluation size must equal player_count");
        return MultiPlayerSearchResult<State, Score>{
            std::move(values), {state}, {}, depth
        };
    };
    if(depth == 0) return leaf();
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return leaf();
    const std::size_t current = static_cast<std::size_t>(std::invoke(player, state));
    if(player_count <= current)
        throw std::out_of_range("maxn_search player index is out of range");
    std::optional<MultiPlayerSearchResult<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = maxn<State, Score>(
            child, depth - 1, player_count, evaluate, children, player, statistics
        );
        if(!best || best->scores[current] < candidate.scores[current])
            best = std::move(candidate);
    }
    best->principal_variation.insert(best->principal_variation.begin(), state);
    best->statistics = {};
    return std::move(*best);
}

template<class State, class Score, class Evaluate, class Children, class Player>
ValueLine<State, Score> paranoid(
    const State& state, std::size_t depth, std::size_t root_player,
    Score alpha, Score beta, Evaluate& evaluate, Children& children,
    Player& player, SearchStatistics& statistics
) {
    ++statistics.visited_nodes;
    if(depth == 0) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    auto next = collect_states<State>(std::invoke(children, state));
    statistics.generated_nodes += next.size();
    if(next.empty()) return evaluate_leaf<State, Score>(state, evaluate, statistics);
    const bool maximizing =
        static_cast<std::size_t>(std::invoke(player, state)) == root_player;
    std::optional<ValueLine<State, Score>> best;
    for(const auto& child : next) {
        auto candidate = paranoid<State, Score>(
            child, depth - 1, root_player, alpha, beta,
            evaluate, children, player, statistics
        );
        if(!best || (maximizing ? best->score < candidate.score
                               : candidate.score < best->score)) best = std::move(candidate);
        if(maximizing) {
            if(alpha < best->score) alpha = best->score;
        } else if(best->score < beta) {
            beta = best->score;
        }
        if(!(alpha < beta)) {
            ++statistics.cutoffs;
            break;
        }
    }
    prepend(*best, state);
    return std::move(*best);
}

template<class State, class Score, class Children, class Evaluate, class RandomEngine>
std::pair<Score, std::vector<State>> random_rollout(
    State state, std::size_t maximum_depth, Children& children,
    Evaluate& evaluate, RandomEngine& engine, SearchStatistics& statistics
) {
    std::vector<State> line{state};
    for(std::size_t depth = 0; depth < maximum_depth; ++depth) {
        ++statistics.visited_nodes;
        auto next = collect_states<State>(std::invoke(children, state));
        statistics.generated_nodes += next.size();
        if(next.empty()) break;
        std::uniform_int_distribution<std::size_t> distribution(0, next.size() - 1);
        state = std::move(next[distribution(engine)]);
        line.push_back(state);
    }
    ++statistics.evaluated_nodes;
    return {static_cast<Score>(std::invoke(evaluate, state)), std::move(line)};
}

} // namespace detail

template<class State, class Evaluate, class Children, class NodeType, class Probability>
auto expectimax_search(
    const State& root, std::size_t depth, Evaluate evaluate, Children children,
    NodeType node_type, Probability probability
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    static_assert(std::is_arithmetic_v<Score>);
    SearchResult<State, Score> result;
    auto value = detail::expectimax<State, Score>(
        root, depth, evaluate, children, node_type, probability, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children, class Player>
auto maxn_search(
    const State& root, std::size_t depth, std::size_t player_count,
    Evaluate evaluate, Children children, Player player
) {
    using Range = std::invoke_result_t<Evaluate&, const State&>;
    using Score = detail::range_value_t<Range>;
    if(player_count < 2) throw std::invalid_argument("maxn_search requires at least two players");
    SearchStatistics statistics;
    auto result = detail::maxn<State, Score>(
        root, depth, player_count, evaluate, children, player, statistics
    );
    result.statistics = statistics;
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children, class Player>
auto paranoid_search(
    const State& root, std::size_t depth, std::size_t root_player,
    Evaluate evaluate, Children children, Player player
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    SearchResult<State, Score> result;
    auto value = detail::paranoid<State, Score>(
        root, depth, root_player,
        detail::negative_infinity<Score>(), detail::positive_infinity<Score>(),
        evaluate, children, player, result.statistics
    );
    result.score = value.score;
    result.principal_variation = std::move(value.line);
    result.completed_depth = depth;
    return result;
}

template<class State, class Evaluate, class Children, class RandomEngine>
auto flat_monte_carlo_search(
    const State& root, std::size_t simulations, std::size_t maximum_rollout_depth,
    Evaluate evaluate, Children children, RandomEngine& random_engine,
    bool maximizing_player = true
) {
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    SearchResult<State, double> result;
    auto roots = detail::collect_states<State>(std::invoke(children, root));
    result.statistics.generated_nodes += roots.size();
    if(roots.empty() || simulations == 0) {
        result.score = static_cast<double>(std::invoke(evaluate, root));
        result.principal_variation = {root};
        ++result.statistics.evaluated_nodes;
        return result;
    }
    std::vector<long double> totals(roots.size());
    std::vector<std::size_t> counts(roots.size());
    std::vector<std::vector<State>> representative(roots.size());
    for(std::size_t simulation = 0; simulation < simulations; ++simulation) {
        const std::size_t index = simulation % roots.size();
        auto [score, line] = detail::random_rollout<State, Score>(
            roots[index], maximum_rollout_depth, children,
            evaluate, random_engine, result.statistics
        );
        totals[index] += static_cast<long double>(score);
        ++counts[index];
        if(representative[index].empty()) representative[index] = std::move(line);
        ++result.statistics.simulations;
    }
    std::size_t best = 0;
    for(std::size_t i = 1; i < roots.size(); ++i) {
        if(counts[i] == 0) continue;
        if(counts[best] == 0) {
            best = i;
            continue;
        }
        const long double left = totals[best] / counts[best];
        const long double right = totals[i] / counts[i];
        if(maximizing_player ? left < right : right < left) best = i;
    }
    result.score = static_cast<double>(totals[best] / counts[best]);
    result.principal_variation = {root};
    result.principal_variation.insert(
        result.principal_variation.end(),
        representative[best].begin(), representative[best].end()
    );
    result.completed_depth = maximum_rollout_depth + 1;
    return result;
}

template<class State, class Evaluate, class Children, class RandomEngine>
auto monte_carlo_search(
    const State& root, std::size_t iterations, std::size_t maximum_tree_depth,
    Evaluate evaluate, Children children, RandomEngine& random_engine,
    double exploration = 1.4142135623730950488,
    bool maximizing_player = true
) {
    using RawScore = std::decay_t<std::invoke_result_t<Evaluate&, const State&>>;
    if(exploration < 0 || !std::isfinite(exploration))
        throw std::invalid_argument("monte_carlo_search requires finite non-negative exploration");
    struct Node {
        State state;
        std::size_t parent = 0;
        std::size_t depth = 0;
        std::vector<std::size_t> children;
        std::vector<State> unexpanded;
        std::size_t visits = 0;
        long double total = 0;
    };
    SearchResult<State, double> result;
    auto root_children = detail::collect_states<State>(std::invoke(children, root));
    result.statistics.generated_nodes += root_children.size();
    std::vector<Node> nodes;
    nodes.push_back(Node{root, 0, 0, {}, std::move(root_children), 0, 0});
    for(std::size_t iteration = 0; iteration < iterations; ++iteration) {
        std::size_t current = 0;
        while(nodes[current].depth < maximum_tree_depth) {
            if(!nodes[current].unexpanded.empty()) {
                std::uniform_int_distribution<std::size_t> distribution(
                    0, nodes[current].unexpanded.size() - 1
                );
                const std::size_t position = distribution(random_engine);
                State child_state = std::move(nodes[current].unexpanded[position]);
                if(position + 1 != nodes[current].unexpanded.size())
                    nodes[current].unexpanded[position] =
                        std::move(nodes[current].unexpanded.back());
                nodes[current].unexpanded.pop_back();
                auto next = detail::collect_states<State>(std::invoke(children, child_state));
                result.statistics.generated_nodes += next.size();
                const std::size_t child = nodes.size();
                nodes.push_back(Node{
                    std::move(child_state), current, nodes[current].depth + 1,
                    {}, std::move(next), 0, 0
                });
                nodes[current].children.push_back(child);
                current = child;
                break;
            }
            if(nodes[current].children.empty()) break;
            std::size_t selected = nodes[current].children.front();
            double selected_value = -std::numeric_limits<double>::infinity();
            const bool maximize = ((nodes[current].depth % 2 == 0) == maximizing_player);
            const long double logarithm =
                std::log(static_cast<long double>(std::max<std::size_t>(1, nodes[current].visits)));
            for(const std::size_t child : nodes[current].children) {
                const long double mean = nodes[child].total / nodes[child].visits;
                const long double exploitation = maximize ? mean : -mean;
                const long double bonus = exploration * std::sqrt(
                    logarithm / nodes[child].visits
                );
                const double value = static_cast<double>(exploitation + bonus);
                if(selected_value < value) {
                    selected_value = value;
                    selected = child;
                }
            }
            current = selected;
        }
        auto [score, ignored] = detail::random_rollout<State, RawScore>(
            nodes[current].state,
            maximum_tree_depth - std::min(maximum_tree_depth, nodes[current].depth),
            children, evaluate, random_engine, result.statistics
        );
        while(true) {
            ++nodes[current].visits;
            nodes[current].total += static_cast<long double>(score);
            if(current == 0) break;
            current = nodes[current].parent;
        }
        ++result.statistics.simulations;
    }
    result.principal_variation = {root};
    std::size_t current = 0;
    while(!nodes[current].children.empty()) {
        std::size_t selected = nodes[current].children.front();
        for(const std::size_t child : nodes[current].children) {
            if(nodes[selected].visits < nodes[child].visits) selected = child;
        }
        result.principal_variation.push_back(nodes[selected].state);
        current = selected;
    }
    if(result.principal_variation.size() == 1 || nodes[current].visits == 0) {
        result.score = static_cast<double>(std::invoke(evaluate, root));
        ++result.statistics.evaluated_nodes;
    } else {
        const std::size_t selected = nodes[0].children.empty() ? 0 :
            *std::max_element(
                nodes[0].children.begin(), nodes[0].children.end(),
                [&](std::size_t left, std::size_t right) {
                    return nodes[left].visits < nodes[right].visits;
                }
            );
        result.score = static_cast<double>(nodes[selected].total / nodes[selected].visits);
    }
    result.statistics.visited_nodes += nodes.size();
    result.completed_depth = maximum_tree_depth;
    return result;
}

} // namespace advanced_adversarial_search

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_ADVERSARIAL_SEARCH_HPP_INCLUDED
