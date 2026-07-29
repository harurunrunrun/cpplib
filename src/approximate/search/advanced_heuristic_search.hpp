#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_HEURISTIC_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_HEURISTIC_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

template<class State, class Cost>
struct AnytimeHeuristicSearchResult{
    std::vector<HeuristicSearchResult<State, Cost>> solutions;
    std::size_t expanded = 0;

    explicit operator bool() const noexcept{
        return !solutions.empty();
    }

    const HeuristicSearchResult<State, Cost>& best() const{
        if(solutions.empty()) throw std::logic_error("the search found no solution");
        return solutions.back();
    }
};

namespace advanced_heuristic_search_internal{

template<class State, class Cost>
struct Node{
    State state;
    Cost distance;
    long double estimate;
    std::size_t parent = 0;
    std::size_t depth = 0;
    std::size_t version = 0;
    bool has_parent = false;
    bool open = true;
};

template<class State, class Cost>
std::vector<State> restore_path(
    const std::vector<Node<State, Cost>>& nodes,
    std::size_t index
){
    std::vector<State> path;
    while(true){
        path.push_back(nodes[index].state);
        if(!nodes[index].has_parent) break;
        index = nodes[index].parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Priority, class Hash, class Equal>
auto priority_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Priority priority,
    bool continue_after_goal,
    bool rebuild_after_improvement,
    Hash hash,
    Equal equal
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = AnytimeHeuristicSearchResult<State, Cost>;

    struct Entry{
        long double key;
        Cost distance;
        std::size_t order;
        std::size_t node;
        std::size_t version;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(rhs.key < lhs.key) return true;
            if(lhs.key < rhs.key) return false;
            return rhs.order < lhs.order;
        }
    };

    std::vector<Node<State, Cost>> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(0, hash, equal);
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    std::optional<Cost> incumbent;
    std::size_t order = 0;
    const long double start_estimate = static_cast<long double>(
        std::invoke(heuristic, start)
    );
    if(start_estimate < 0.0L)[[unlikely]]{
        throw std::invalid_argument("heuristic estimates must be non-negative");
    }
    nodes.push_back(Node<State, Cost>{
        start, Cost{}, start_estimate, 0, 0, 0, false, true
    });
    index.emplace(start, 0);

    const auto key_of = [&](const Node<State, Cost>& node){
        return static_cast<long double>(std::invoke(
            priority, node.state, node.distance, node.estimate, node.depth, incumbent
        ));
    };
    const auto push = [&](std::size_t node_index){
        Node<State, Cost>& node = nodes[node_index];
        node.open = true;
        queue.push(Entry{
            key_of(node), node.distance, order++, node_index, node.version
        });
    };
    push(0);

    Result result;
    while(!queue.empty()){
        const Entry entry = queue.top();
        queue.pop();
        Node<State, Cost>& current = nodes[entry.node];
        if(!current.open || entry.version != current.version
           || entry.distance != current.distance) continue;
        current.open = false;

        if(incumbent
           && !((static_cast<long double>(current.distance) + current.estimate)
                < static_cast<long double>(*incumbent))){
            continue;
        }
        ++result.expanded;
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t current_depth = current.depth;
        if(static_cast<bool>(std::invoke(goal, current_state))){
            if(!incumbent || current_distance < *incumbent){
                incumbent = current_distance;
                HeuristicSearchResult<State, Cost> solution;
                solution.cost = current_distance;
                solution.path = restore_path(nodes, entry.node);
                solution.expanded = result.expanded;
                result.solutions.push_back(std::move(solution));
                if(!continue_after_goal) break;
                if(rebuild_after_improvement){
                    queue = decltype(queue){};
                    for(std::size_t i = 0; i < nodes.size(); ++i){
                        Node<State, Cost>& node = nodes[i];
                        if(static_cast<long double>(node.distance) + node.estimate
                           < static_cast<long double>(*incumbent)){
                            ++node.version;
                            push(i);
                        }else{
                            node.open = false;
                        }
                    }
                }
            }
            continue;
        }

        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument("heuristic search does not accept negative edges");
            }
            const Cost candidate = current_distance + edge_cost;
            const State& next_state = edge.first;
            auto iterator = index.find(next_state);
            std::size_t next_index;
            if(iterator == index.end()){
                const long double estimate = static_cast<long double>(
                    std::invoke(heuristic, next_state)
                );
                if(estimate < 0.0L)[[unlikely]]{
                    throw std::invalid_argument("heuristic estimates must be non-negative");
                }
                next_index = nodes.size();
                nodes.push_back(Node<State, Cost>{
                    next_state, candidate, estimate, entry.node,
                    current_depth + 1, 0, true, true
                });
                index.emplace(nodes.back().state, next_index);
            }else{
                next_index = iterator->second;
                Node<State, Cost>& next = nodes[next_index];
                if(!(candidate < next.distance)) continue;
                next.distance = candidate;
                next.parent = entry.node;
                next.depth = current_depth + 1;
                next.has_parent = true;
                ++next.version;
            }
            Node<State, Cost>& next = nodes[next_index];
            if(incumbent
               && !(static_cast<long double>(candidate) + next.estimate
                    < static_cast<long double>(*incumbent))){
                next.open = false;
                continue;
            }
            push(next_index);
        }
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Secondary, class Eligibility, class Hash, class Equal>
auto focal_search_impl(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Secondary secondary,
    Eligibility eligibility,
    long double factor,
    Hash hash,
    Equal equal
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct AnchorEntry{
        long double key;
        std::size_t order;
        std::size_t node;
        std::size_t version;
    };
    struct AnchorCompare{
        bool operator()(const AnchorEntry& lhs, const AnchorEntry& rhs) const{
            if(rhs.key < lhs.key) return true;
            if(lhs.key < rhs.key) return false;
            return rhs.order < lhs.order;
        }
    };
    struct FocalEntry{
        long double secondary;
        long double eligible;
        std::size_t order;
        std::size_t node;
        std::size_t version;
    };
    struct FocalCompare{
        bool operator()(const FocalEntry& lhs, const FocalEntry& rhs) const{
            if(rhs.secondary < lhs.secondary) return true;
            if(lhs.secondary < rhs.secondary) return false;
            return rhs.order < lhs.order;
        }
    };
    struct PendingCompare{
        bool operator()(const FocalEntry& lhs, const FocalEntry& rhs) const{
            if(rhs.eligible < lhs.eligible) return true;
            if(lhs.eligible < rhs.eligible) return false;
            return rhs.order < lhs.order;
        }
    };

    if(factor < 1.0L)[[unlikely]]{
        throw std::invalid_argument("focal search requires factor >= 1");
    }
    std::vector<Node<State, Cost>> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(0, hash, equal);
    std::priority_queue<AnchorEntry, std::vector<AnchorEntry>, AnchorCompare> anchor;
    std::priority_queue<FocalEntry, std::vector<FocalEntry>, FocalCompare> focal;
    std::priority_queue<FocalEntry, std::vector<FocalEntry>, PendingCompare> pending;
    std::size_t order = 0;

    const auto add = [&](std::size_t node_index){
        Node<State, Cost>& node = nodes[node_index];
        node.open = true;
        const long double f = static_cast<long double>(node.distance) + node.estimate;
        const long double eligible = static_cast<long double>(std::invoke(
            eligibility, node.state, node.distance, node.estimate
        ));
        const long double second = static_cast<long double>(std::invoke(
            secondary, node.state, node.distance, node.estimate
        ));
        anchor.push(AnchorEntry{f, order, node_index, node.version});
        pending.push(FocalEntry{second, eligible, order++, node_index, node.version});
    };
    const long double start_h = static_cast<long double>(std::invoke(heuristic, start));
    if(start_h < 0.0L)[[unlikely]]{
        throw std::invalid_argument("heuristic estimates must be non-negative");
    }
    nodes.push_back(Node<State, Cost>{start, Cost{}, start_h, 0, 0, 0, false, true});
    index.emplace(start, 0);
    add(0);

    Result result;
    while(true){
        while(!anchor.empty()){
            const AnchorEntry& top = anchor.top();
            const Node<State, Cost>& node = nodes[top.node];
            if(node.open && node.version == top.version) break;
            anchor.pop();
        }
        if(anchor.empty()) return result;
        const long double threshold = factor * anchor.top().key;
        while(!pending.empty() && pending.top().eligible <= threshold){
            focal.push(pending.top());
            pending.pop();
        }
        while(!focal.empty()){
            const FocalEntry top = focal.top();
            const Node<State, Cost>& node = nodes[top.node];
            if(!node.open || node.version != top.version){
                focal.pop();
                continue;
            }
            if(top.eligible <= threshold) break;
            focal.pop();
            pending.push(top);
        }
        if(focal.empty()){
            // `eligibility` may be stricter than the anchor lower bound. In that
            // case the anchor entry is always a legal progress step.
            const AnchorEntry top = anchor.top();
            Node<State, Cost>& node = nodes[top.node];
            focal.push(FocalEntry{
                static_cast<long double>(std::invoke(
                    secondary, node.state, node.distance, node.estimate
                )),
                static_cast<long double>(std::invoke(
                    eligibility, node.state, node.distance, node.estimate
                )),
                top.order, top.node, top.version
            });
        }

        const FocalEntry chosen = focal.top();
        focal.pop();
        Node<State, Cost>& current = nodes[chosen.node];
        if(!current.open || current.version != chosen.version) continue;
        current.open = false;
        ++result.expanded;
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t current_depth = current.depth;
        if(static_cast<bool>(std::invoke(goal, current_state))){
            result.cost = current_distance;
            result.path = restore_path(nodes, chosen.node);
            return result;
        }
        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument("focal search does not accept negative edges");
            }
            const Cost candidate = current_distance + edge_cost;
            const State& next_state = edge.first;
            auto iterator = index.find(next_state);
            std::size_t next_index;
            if(iterator == index.end()){
                const long double estimate = static_cast<long double>(
                    std::invoke(heuristic, next_state)
                );
                if(estimate < 0.0L)[[unlikely]]{
                    throw std::invalid_argument("heuristic estimates must be non-negative");
                }
                next_index = nodes.size();
                nodes.push_back(Node<State, Cost>{
                    next_state, candidate, estimate, chosen.node,
                    current_depth + 1, 0, true, true
                });
                index.emplace(nodes.back().state, next_index);
            }else{
                next_index = iterator->second;
                Node<State, Cost>& next = nodes[next_index];
                if(!(candidate < next.distance)) continue;
                next.distance = candidate;
                next.parent = chosen.node;
                next.depth = current_depth + 1;
                next.has_parent = true;
                ++next.version;
            }
            add(next_index);
        }
    }
}

template<class Result>
auto final_result(Result&& anytime){
    using Solution = typename decltype(anytime.solutions)::value_type;
    Solution result;
    if(!anytime.solutions.empty()) result = std::move(anytime.solutions.back());
    result.expanded = anytime.expanded;
    return result;
}

} // namespace advanced_heuristic_search_internal

template<class State, class Goal, class Neighbors, class Heuristic, class Weight,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto anytime_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Weight weight,
    Hash hash = {},
    Equal equal = {}
){
    if(weight < Weight{1})[[unlikely]]{
        throw std::invalid_argument("anytime_a_star_search requires weight >= 1");
    }
    const long double w = static_cast<long double>(weight);
    return advanced_heuristic_search_internal::priority_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [w](const State&, const auto& g, long double h, std::size_t,
            const auto&){ return static_cast<long double>(g) + w * h; },
        true, false, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic, class WeightRange,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto ara_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    const WeightRange& weights,
    Hash hash = {},
    Equal equal = {}
){
    std::vector<long double> schedule;
    for(const auto& weight : weights){
        const long double value = static_cast<long double>(weight);
        if(value < 1.0L)[[unlikely]]{
            throw std::invalid_argument("ara_star_search requires every weight >= 1");
        }
        schedule.push_back(value);
    }
    if(schedule.empty())[[unlikely]]{
        throw std::invalid_argument("ara_star_search requires a non-empty schedule");
    }
    for(std::size_t i = 1; i < schedule.size(); ++i){
        if(schedule[i - 1] < schedule[i])[[unlikely]]{
            throw std::invalid_argument("ARA* weights must be non-increasing");
        }
    }
    // ARA* keeps all discovered g-values. The phase is advanced after an
    // incumbent improvement and OPEN is re-keyed instead of restarting.
    auto phase = std::size_t{0};
    std::optional<long double> seen_incumbent;
    return advanced_heuristic_search_internal::priority_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [&schedule, &phase, &seen_incumbent](const State&, const auto& g, long double h,
                            std::size_t, const auto& incumbent){
            if(incumbent){
                const long double value = static_cast<long double>(*incumbent);
                if((!seen_incumbent || value < *seen_incumbent)
                   && phase + 1 < schedule.size()){
                    ++phase;
                    seen_incumbent = value;
                }
            }
            return static_cast<long double>(g) + schedule[phase] * h;
        },
        true, true, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto ana_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Hash hash = {},
    Equal equal = {}
){
    return advanced_heuristic_search_internal::priority_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [](const State&, const auto& g, long double h, std::size_t,
           const auto& incumbent){
            if(!incumbent) return h;
            if(h == 0.0L) return -std::numeric_limits<long double>::infinity();
            return -(static_cast<long double>(*incumbent)
                     - static_cast<long double>(g)) / h;
        },
        true, true, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic, class Secondary,
         class Factor, class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto focal_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Secondary secondary,
    Factor factor,
    Hash hash = {},
    Equal equal = {}
){
    return advanced_heuristic_search_internal::focal_search_impl(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [secondary = std::move(secondary)](
            const State& state, const auto&, long double
        ){ return std::invoke(secondary, state); },
        [](const State&, const auto& g, long double h){
            return static_cast<long double>(g) + h;
        },
        static_cast<long double>(factor), std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic, class Epsilon,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto a_epsilon_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Epsilon epsilon,
    Hash hash = {},
    Equal equal = {}
){
    const long double value = static_cast<long double>(epsilon);
    if(value < 0.0L)[[unlikely]]{
        throw std::invalid_argument("a_epsilon_search requires epsilon >= 0");
    }
    return advanced_heuristic_search_internal::focal_search_impl(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [](const State&, const auto&, long double h){ return h; },
        [](const State&, const auto& g, long double h){
            return static_cast<long double>(g) + h;
        },
        1.0L + value, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class EstimatedHeuristic, class DistanceEstimate, class Factor,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto explicit_estimation_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    EstimatedHeuristic estimated_heuristic,
    DistanceEstimate distance_estimate,
    Factor factor,
    Hash hash = {},
    Equal equal = {}
){
    return advanced_heuristic_search_internal::focal_search_impl(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [distance_estimate = std::move(distance_estimate)](
            const State& state, const auto&, long double
        ){ return std::invoke(distance_estimate, state); },
        [estimated_heuristic = std::move(estimated_heuristic)](
            const State& state, const auto& g, long double
        ){
            return static_cast<long double>(g)
                + static_cast<long double>(std::invoke(estimated_heuristic, state));
        },
        static_cast<long double>(factor), std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Factor, class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto optimistic_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Factor factor,
    Hash hash = {},
    Equal equal = {}
){
    const long double w = static_cast<long double>(factor);
    if(w < 1.0L)[[unlikely]]{
        throw std::invalid_argument("optimistic_search requires factor >= 1");
    }
    return advanced_heuristic_search_internal::focal_search_impl(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [w](const State&, const auto& g, long double h){
            return static_cast<long double>(g) + w * h;
        },
        [](const State&, const auto& g, long double h){
            return static_cast<long double>(g) + h;
        },
        w, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic, class Bound,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto potential_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Bound bound,
    Hash hash = {},
    Equal equal = {}
){
    const long double limit = static_cast<long double>(bound);
    auto anytime = advanced_heuristic_search_internal::priority_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [limit](const State&, const auto& g, long double h, std::size_t,
                const auto&){
            const long double remaining = limit - static_cast<long double>(g);
            return remaining <= 0.0L
                ? std::numeric_limits<long double>::infinity()
                : h / remaining;
        },
        false, false, std::move(hash), std::move(equal)
    );
    auto result = advanced_heuristic_search_internal::final_result(std::move(anytime));
    if(result.cost
       && !(static_cast<long double>(*result.cost) < limit)){
        result.cost.reset();
        result.path.clear();
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristics,
         class Weight, class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto multi_heuristic_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristics heuristics,
    Weight weight,
    Hash hash = {},
    Equal equal = {}
){
    const long double w = static_cast<long double>(weight);
    if(w < 0.0L)[[unlikely]]{
        throw std::invalid_argument("multi_heuristic_a_star_search requires weight >= 0");
    }
    const auto minimum_estimate = [heuristics](const State& state){
        auto values = std::invoke(heuristics, state);
        const auto first = std::begin(values);
        const auto last = std::end(values);
        if(first == last) throw std::invalid_argument("heuristic range must not be empty");
        long double result = static_cast<long double>(*first);
        for(auto iterator = std::next(first); iterator != last; ++iterator){
            result = std::min(result, static_cast<long double>(*iterator));
        }
        return result;
    };
    auto anytime = advanced_heuristic_search_internal::priority_search(
        start, std::move(goal), std::move(neighbors), minimum_estimate,
        [w](const State&, const auto& g, long double h, std::size_t,
            const auto&){ return static_cast<long double>(g) + w * h; },
        false, false, std::move(hash), std::move(equal)
    );
    return advanced_heuristic_search_internal::final_result(std::move(anytime));
}

template<class State, class Goal, class Neighbors, class AnchorHeuristic,
         class AdditionalHeuristics, class Weight1, class Weight2,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto mha_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    AnchorHeuristic anchor_heuristic,
    AdditionalHeuristics additional_heuristics,
    Weight1 weight1,
    Weight2 weight2,
    Hash hash = {},
    Equal equal = {}
){
    const long double w1 = static_cast<long double>(weight1);
    const long double w2 = static_cast<long double>(weight2);
    if(w1 < 1.0L || w2 < 1.0L)[[unlikely]]{
        throw std::invalid_argument("mha_star_search requires both weights >= 1");
    }
    return advanced_heuristic_search_internal::focal_search_impl(
        start, std::move(goal), std::move(neighbors), std::move(anchor_heuristic),
        [additional_heuristics = std::move(additional_heuristics), w1](
            const State& state, const auto& g, long double anchor_h
        ){
            long double result = static_cast<long double>(g) + w1 * anchor_h;
            auto values = std::invoke(additional_heuristics, state);
            for(const auto& value : values){
                result = std::min(
                    result,
                    static_cast<long double>(g)
                        + w1 * static_cast<long double>(value)
                );
            }
            return result;
        },
        [](const State&, const auto& g, long double h){
            return static_cast<long double>(g) + h;
        },
        w2, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Epsilon, class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto dynamic_weighted_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Epsilon epsilon,
    std::size_t expected_solution_depth,
    Hash hash = {},
    Equal equal = {}
){
    const long double value = static_cast<long double>(epsilon);
    if(value < 0.0L || expected_solution_depth == 0)[[unlikely]]{
        throw std::invalid_argument(
            "dynamic_weighted_a_star_search requires epsilon >= 0 and positive depth"
        );
    }
    auto anytime = advanced_heuristic_search_internal::priority_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        [value, expected_solution_depth](
            const State&, const auto& g, long double h, std::size_t depth,
            const auto&
        ){
            const long double progress = std::min(
                1.0L,
                static_cast<long double>(depth)
                    / static_cast<long double>(expected_solution_depth)
            );
            return static_cast<long double>(g)
                + (1.0L + value * (1.0L - progress)) * h;
        },
        false, false, std::move(hash), std::move(equal)
    );
    return advanced_heuristic_search_internal::final_result(std::move(anytime));
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_HEURISTIC_SEARCH_HPP_INCLUDED
