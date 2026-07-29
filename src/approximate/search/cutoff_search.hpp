#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_CUTOFF_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_CUTOFF_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

enum class CutoffSearchStop{
    exhausted,
    goal_found,
    depth_limit,
    cost_limit,
    expansion_limit,
    memory_limit
};

template<class State, class Cost>
struct CutoffSearchResult{
    std::optional<Cost> cost;
    std::vector<State> path;
    std::size_t expanded = 0;
    std::size_t generated = 1;
    std::size_t peak_frontier = 1;
    CutoffSearchStop stop = CutoffSearchStop::exhausted;

    explicit operator bool() const noexcept{
        return cost.has_value();
    }
};

template<class State, class Value>
struct BranchAndBoundSearchResult{
    std::optional<State> solution;
    std::optional<Value> objective;
    std::size_t expanded = 0;
    std::size_t generated = 1;
    bool optimal = false;
    CutoffSearchStop stop = CutoffSearchStop::exhausted;

    explicit operator bool() const noexcept{
        return solution.has_value();
    }
};

namespace cutoff_search_internal{

template<class Range>
using range_value_t = std::decay_t<decltype(
    *std::begin(std::declval<std::remove_reference_t<Range>&>())
)>;

template<class State, class Cost>
struct Node{
    State state;
    Cost distance{};
    long double heuristic = 0.0L;
    std::size_t parent = 0;
    std::size_t depth = 0;
    std::size_t order = 0;
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

template<class Heuristic, class State>
long double estimate(Heuristic& heuristic, const State& state){
    const long double value = static_cast<long double>(
        std::invoke(heuristic, state)
    );
    if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
        throw std::invalid_argument(
            "cutoff search requires finite non-negative heuristic values"
        );
    }
    return value;
}

template<class Cost>
void validate_edge(const Cost& cost){
    if(cost < Cost{})[[unlikely]]{
        throw std::invalid_argument("cutoff search does not accept negative edges");
    }
}

template<class State, class Cost>
long double score(const Node<State, Cost>& node){
    return static_cast<long double>(node.distance) + node.heuristic;
}

template<class State, class Cost>
std::size_t best_open(const std::vector<Node<State, Cost>>& nodes){
    std::size_t best = nodes.size();
    for(std::size_t i = 0; i < nodes.size(); ++i){
        if(!nodes[i].open) continue;
        if(best == nodes.size()
           || score(nodes[i]) < score(nodes[best])
           || (score(nodes[i]) == score(nodes[best])
               && nodes[i].order < nodes[best].order)){
            best = i;
        }
    }
    return best;
}

template<class State, class Cost>
std::size_t open_count(const std::vector<Node<State, Cost>>& nodes){
    return static_cast<std::size_t>(std::count_if(
        nodes.begin(), nodes.end(),
        [](const Node<State, Cost>& node){ return node.open; }
    ));
}

}  // namespace cutoff_search_internal

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto bounded_suboptimal_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    long double factor,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = cutoff_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Node = cutoff_search_internal::Node<State, Cost>;
    using Result = CutoffSearchResult<State, Cost>;

    if(factor < 1.0L || !std::isfinite(factor))[[unlikely]]{
        throw std::invalid_argument(
            "bounded_suboptimal_search requires a finite factor >= 1"
        );
    }

    std::vector<Node> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(
        0, std::move(hash), std::move(equal)
    );
    nodes.push_back(Node{
        start, Cost{}, cutoff_search_internal::estimate(heuristic, start),
        0, 0, 0, 0, false, true
    });
    index.emplace(start, 0);
    Result result;
    std::size_t order = 1;

    while(true){
        const std::size_t anchor = cutoff_search_internal::best_open(nodes);
        if(anchor == nodes.size()) break;
        const long double threshold =
            factor * cutoff_search_internal::score(nodes[anchor]);
        std::size_t selected = anchor;
        for(std::size_t i = 0; i < nodes.size(); ++i){
            if(!nodes[i].open
               || cutoff_search_internal::score(nodes[i]) > threshold) continue;
            if(nodes[i].heuristic < nodes[selected].heuristic
               || (nodes[i].heuristic == nodes[selected].heuristic
                   && nodes[i].order < nodes[selected].order)){
                selected = i;
            }
        }
        Node& current = nodes[selected];
        current.open = false;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = cutoff_search_internal::restore_path(nodes, selected);
            result.stop = CutoffSearchStop::goal_found;
            return result;
        }
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t current_depth = current.depth;
        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            cutoff_search_internal::validate_edge(edge_cost);
            const Cost candidate = static_cast<Cost>(current_distance + edge_cost);
            auto iterator = index.find(edge.first);
            if(iterator == index.end()){
                const std::size_t next = nodes.size();
                nodes.push_back(Node{
                    edge.first, candidate,
                    cutoff_search_internal::estimate(heuristic, edge.first),
                    selected, current_depth + 1, order++, 0, true, true
                });
                index.emplace(nodes.back().state, next);
                ++result.generated;
            }else{
                Node& old = nodes[iterator->second];
                if(!(candidate < old.distance)) continue;
                old.distance = candidate;
                old.parent = selected;
                old.depth = current_depth + 1;
                old.has_parent = true;
                old.open = true;
                ++old.version;
            }
        }
        result.peak_frontier = std::max(
            result.peak_frontier,
            cutoff_search_internal::open_count(nodes)
        );
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto depth_bounded_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t maximum_depth,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = cutoff_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Node = cutoff_search_internal::Node<State, Cost>;
    using Result = CutoffSearchResult<State, Cost>;

    std::vector<Node> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(
        0, std::move(hash), std::move(equal)
    );
    nodes.push_back(Node{
        start, Cost{}, cutoff_search_internal::estimate(heuristic, start),
        0, 0, 0, 0, false, true
    });
    index.emplace(start, 0);
    Result result;
    bool pruned = false;
    std::size_t order = 1;
    while(true){
        const std::size_t selected = cutoff_search_internal::best_open(nodes);
        if(selected == nodes.size()) break;
        Node& current = nodes[selected];
        current.open = false;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = cutoff_search_internal::restore_path(nodes, selected);
            result.stop = CutoffSearchStop::goal_found;
            return result;
        }
        if(current.depth == maximum_depth){
            pruned = true;
            continue;
        }
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t next_depth = current.depth + 1;
        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            cutoff_search_internal::validate_edge(edge_cost);
            const Cost candidate = static_cast<Cost>(current_distance + edge_cost);
            auto iterator = index.find(edge.first);
            if(iterator == index.end()){
                const std::size_t next = nodes.size();
                nodes.push_back(Node{
                    edge.first, candidate,
                    cutoff_search_internal::estimate(heuristic, edge.first),
                    selected, next_depth, order++, 0, true, true
                });
                index.emplace(nodes.back().state, next);
                ++result.generated;
            }else{
                Node& old = nodes[iterator->second];
                if(!(candidate < old.distance) || next_depth > maximum_depth) continue;
                old.distance = candidate;
                old.parent = selected;
                old.depth = next_depth;
                old.has_parent = true;
                old.open = true;
            }
        }
        result.peak_frontier = std::max(
            result.peak_frontier,
            cutoff_search_internal::open_count(nodes)
        );
    }
    result.stop = pruned ? CutoffSearchStop::depth_limit
                         : CutoffSearchStop::exhausted;
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic, class Limit,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto cutoff_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Limit maximum_f,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = cutoff_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Node = cutoff_search_internal::Node<State, Cost>;
    using Result = CutoffSearchResult<State, Cost>;
    const long double limit = static_cast<long double>(maximum_f);
    if(!std::isfinite(limit))[[unlikely]]{
        throw std::invalid_argument("cutoff_a_star_search requires a finite cutoff");
    }

    std::vector<Node> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(
        0, std::move(hash), std::move(equal)
    );
    nodes.push_back(Node{
        start, Cost{}, cutoff_search_internal::estimate(heuristic, start),
        0, 0, 0, 0, false, true
    });
    index.emplace(start, 0);
    Result result;
    bool pruned = false;
    std::size_t order = 1;
    while(true){
        const std::size_t selected = cutoff_search_internal::best_open(nodes);
        if(selected == nodes.size()) break;
        Node& current = nodes[selected];
        current.open = false;
        if(cutoff_search_internal::score(current) > limit){
            pruned = true;
            continue;
        }
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = cutoff_search_internal::restore_path(nodes, selected);
            result.stop = CutoffSearchStop::goal_found;
            return result;
        }
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t next_depth = current.depth + 1;
        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            cutoff_search_internal::validate_edge(edge_cost);
            const Cost candidate = static_cast<Cost>(current_distance + edge_cost);
            const long double next_h =
                cutoff_search_internal::estimate(heuristic, edge.first);
            if(static_cast<long double>(candidate) + next_h > limit){
                pruned = true;
                continue;
            }
            auto iterator = index.find(edge.first);
            if(iterator == index.end()){
                const std::size_t next = nodes.size();
                nodes.push_back(Node{
                    edge.first, candidate, next_h, selected, next_depth,
                    order++, 0, true, true
                });
                index.emplace(nodes.back().state, next);
                ++result.generated;
            }else{
                Node& old = nodes[iterator->second];
                if(!(candidate < old.distance)) continue;
                old.distance = candidate;
                old.parent = selected;
                old.depth = next_depth;
                old.has_parent = true;
                old.open = true;
            }
        }
        result.peak_frontier = std::max(
            result.peak_frontier,
            cutoff_search_internal::open_count(nodes)
        );
    }
    result.stop = pruned ? CutoffSearchStop::cost_limit
                         : CutoffSearchStop::exhausted;
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto expansion_limited_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t maximum_expansions,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = cutoff_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Node = cutoff_search_internal::Node<State, Cost>;
    using Result = CutoffSearchResult<State, Cost>;

    std::vector<Node> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(
        0, std::move(hash), std::move(equal)
    );
    nodes.push_back(Node{
        start, Cost{}, cutoff_search_internal::estimate(heuristic, start),
        0, 0, 0, 0, false, true
    });
    index.emplace(start, 0);
    Result result;
    std::size_t order = 1;
    while(true){
        const std::size_t selected = cutoff_search_internal::best_open(nodes);
        if(selected == nodes.size()) break;
        if(result.expanded == maximum_expansions){
            result.stop = CutoffSearchStop::expansion_limit;
            return result;
        }
        Node& current = nodes[selected];
        current.open = false;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = cutoff_search_internal::restore_path(nodes, selected);
            result.stop = CutoffSearchStop::goal_found;
            return result;
        }
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t next_depth = current.depth + 1;
        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            cutoff_search_internal::validate_edge(edge_cost);
            const Cost candidate = static_cast<Cost>(current_distance + edge_cost);
            auto iterator = index.find(edge.first);
            if(iterator == index.end()){
                const std::size_t next = nodes.size();
                nodes.push_back(Node{
                    edge.first, candidate,
                    cutoff_search_internal::estimate(heuristic, edge.first),
                    selected, next_depth, order++, 0, true, true
                });
                index.emplace(nodes.back().state, next);
                ++result.generated;
            }else{
                Node& old = nodes[iterator->second];
                if(!(candidate < old.distance)) continue;
                old.distance = candidate;
                old.parent = selected;
                old.depth = next_depth;
                old.has_parent = true;
                old.open = true;
            }
        }
        result.peak_frontier = std::max(
            result.peak_frontier,
            cutoff_search_internal::open_count(nodes)
        );
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto memory_limited_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t maximum_frontier,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = cutoff_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Node = cutoff_search_internal::Node<State, Cost>;
    using Result = CutoffSearchResult<State, Cost>;
    if(maximum_frontier == 0)[[unlikely]]{
        throw std::invalid_argument(
            "memory_limited_a_star_search requires a positive frontier limit"
        );
    }

    std::vector<Node> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(
        0, std::move(hash), std::move(equal)
    );
    nodes.push_back(Node{
        start, Cost{}, cutoff_search_internal::estimate(heuristic, start),
        0, 0, 0, 0, false, true
    });
    index.emplace(start, 0);
    Result result;
    bool discarded = false;
    std::size_t order = 1;
    while(true){
        const std::size_t selected = cutoff_search_internal::best_open(nodes);
        if(selected == nodes.size()) break;
        Node& current = nodes[selected];
        current.open = false;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = cutoff_search_internal::restore_path(nodes, selected);
            result.stop = CutoffSearchStop::goal_found;
            return result;
        }
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t next_depth = current.depth + 1;
        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            cutoff_search_internal::validate_edge(edge_cost);
            const Cost candidate = static_cast<Cost>(current_distance + edge_cost);
            auto iterator = index.find(edge.first);
            if(iterator == index.end()){
                const std::size_t next = nodes.size();
                nodes.push_back(Node{
                    edge.first, candidate,
                    cutoff_search_internal::estimate(heuristic, edge.first),
                    selected, next_depth, order++, 0, true, true
                });
                index.emplace(nodes.back().state, next);
                ++result.generated;
            }else{
                Node& old = nodes[iterator->second];
                if(!(candidate < old.distance)) continue;
                old.distance = candidate;
                old.parent = selected;
                old.depth = next_depth;
                old.has_parent = true;
                old.open = true;
            }
        }
        while(cutoff_search_internal::open_count(nodes) > maximum_frontier){
            std::size_t worst = nodes.size();
            for(std::size_t i = 0; i < nodes.size(); ++i){
                if(!nodes[i].open) continue;
                if(worst == nodes.size()
                   || cutoff_search_internal::score(nodes[worst])
                        < cutoff_search_internal::score(nodes[i])
                   || (cutoff_search_internal::score(nodes[worst])
                           == cutoff_search_internal::score(nodes[i])
                       && nodes[worst].order < nodes[i].order)){
                    worst = i;
                }
            }
            nodes[worst].open = false;
            discarded = true;
        }
        result.peak_frontier = std::max(
            result.peak_frontier,
            cutoff_search_internal::open_count(nodes)
        );
    }
    result.stop = discarded ? CutoffSearchStop::memory_limit
                            : CutoffSearchStop::exhausted;
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto epsilon_greedy_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    long double epsilon,
    std::size_t maximum_expansions,
    std::uint64_t seed = 0,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = cutoff_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Node = cutoff_search_internal::Node<State, Cost>;
    using Result = CutoffSearchResult<State, Cost>;
    if(epsilon < 0.0L || epsilon > 1.0L || !std::isfinite(epsilon))[[unlikely]]{
        throw std::invalid_argument(
            "epsilon_greedy_search requires epsilon in [0, 1]"
        );
    }

    std::vector<Node> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(
        0, std::move(hash), std::move(equal)
    );
    nodes.push_back(Node{
        start, Cost{}, cutoff_search_internal::estimate(heuristic, start),
        0, 0, 0, 0, false, true
    });
    index.emplace(start, 0);
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<long double> coin(0.0L, 1.0L);
    Result result;
    std::size_t order = 1;
    while(true){
        std::vector<std::size_t> open;
        for(std::size_t i = 0; i < nodes.size(); ++i){
            if(nodes[i].open) open.push_back(i);
        }
        if(open.empty()) break;
        if(result.expanded == maximum_expansions){
            result.stop = CutoffSearchStop::expansion_limit;
            return result;
        }
        std::size_t selected;
        if(coin(engine) < epsilon){
            std::uniform_int_distribution<std::size_t> choose(0, open.size() - 1);
            selected = open[choose(engine)];
        }else{
            selected = cutoff_search_internal::best_open(nodes);
        }
        Node& current = nodes[selected];
        current.open = false;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = cutoff_search_internal::restore_path(nodes, selected);
            result.stop = CutoffSearchStop::goal_found;
            return result;
        }
        const State current_state = current.state;
        const Cost current_distance = current.distance;
        const std::size_t next_depth = current.depth + 1;
        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            cutoff_search_internal::validate_edge(edge_cost);
            const Cost candidate = static_cast<Cost>(current_distance + edge_cost);
            auto iterator = index.find(edge.first);
            if(iterator == index.end()){
                const std::size_t next = nodes.size();
                nodes.push_back(Node{
                    edge.first, candidate,
                    cutoff_search_internal::estimate(heuristic, edge.first),
                    selected, next_depth, order++, 0, true, true
                });
                index.emplace(nodes.back().state, next);
                ++result.generated;
            }else{
                Node& old = nodes[iterator->second];
                if(!(candidate < old.distance)) continue;
                old.distance = candidate;
                old.parent = selected;
                old.depth = next_depth;
                old.has_parent = true;
                old.open = true;
            }
        }
        result.peak_frontier = std::max(result.peak_frontier, open.size());
    }
    return result;
}

template<class State, class Complete, class Branch, class Objective,
         class LowerBound>
auto best_first_branch_and_bound(
    const State& root,
    Complete complete,
    Branch branch,
    Objective objective,
    LowerBound lower_bound
){
    using Value = std::decay_t<std::invoke_result_t<LowerBound&, const State&>>;
    using Result = BranchAndBoundSearchResult<State, Value>;
    struct Entry{
        Value bound;
        std::size_t order;
        State state;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(rhs.bound < lhs.bound) return true;
            if(lhs.bound < rhs.bound) return false;
            return rhs.order < lhs.order;
        }
    };
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    queue.push(Entry{std::invoke(lower_bound, root), 0, root});
    Result result;
    std::size_t order = 1;
    while(!queue.empty()){
        Entry current = queue.top();
        queue.pop();
        if(result.objective && !((current.bound) < *result.objective)) continue;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(complete, current.state))){
            const Value value = static_cast<Value>(
                std::invoke(objective, current.state)
            );
            if(!result.objective || value < *result.objective){
                result.solution = std::move(current.state);
                result.objective = value;
            }
            continue;
        }
        auto children = std::invoke(branch, current.state);
        for(const auto& child : children){
            const Value bound = static_cast<Value>(
                std::invoke(lower_bound, child)
            );
            ++result.generated;
            if(result.objective && !(bound < *result.objective)) continue;
            queue.push(Entry{bound, order++, child});
        }
    }
    result.optimal = true;
    return result;
}

template<class State, class Complete, class Branch, class Objective,
         class LowerBound>
auto interrupted_branch_and_bound(
    const State& root,
    Complete complete,
    Branch branch,
    Objective objective,
    LowerBound lower_bound,
    std::size_t maximum_expansions
){
    using Value = std::decay_t<std::invoke_result_t<LowerBound&, const State&>>;
    using Result = BranchAndBoundSearchResult<State, Value>;
    struct Entry{
        Value bound;
        std::size_t order;
        State state;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(rhs.bound < lhs.bound) return true;
            if(lhs.bound < rhs.bound) return false;
            return rhs.order < lhs.order;
        }
    };
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    queue.push(Entry{std::invoke(lower_bound, root), 0, root});
    Result result;
    std::size_t order = 1;
    while(!queue.empty()){
        if(result.expanded == maximum_expansions){
            result.stop = CutoffSearchStop::expansion_limit;
            return result;
        }
        Entry current = queue.top();
        queue.pop();
        if(result.objective && !(current.bound < *result.objective)) continue;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(complete, current.state))){
            const Value value = static_cast<Value>(
                std::invoke(objective, current.state)
            );
            if(!result.objective || value < *result.objective){
                result.solution = std::move(current.state);
                result.objective = value;
            }
            continue;
        }
        auto children = std::invoke(branch, current.state);
        for(const auto& child : children){
            const Value bound = static_cast<Value>(
                std::invoke(lower_bound, child)
            );
            ++result.generated;
            if(result.objective && !(bound < *result.objective)) continue;
            queue.push(Entry{bound, order++, child});
        }
    }
    result.optimal = true;
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_CUTOFF_SEARCH_HPP_INCLUDED
