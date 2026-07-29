#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_BEAM_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_BEAM_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

namespace advanced_beam_search_internal{

template<class State, class Cost>
using Node = heuristic_search_internal::Node<State, Cost>;

template<class State, class Cost, class Equal>
bool occurs_on_path(
    const std::vector<Node<State, Cost>>& nodes,
    std::size_t index,
    const State& state,
    Equal& equal
){
    while(true){
        if(static_cast<bool>(std::invoke(equal, nodes[index].state, state))) return true;
        if(!nodes[index].has_parent) return false;
        index = nodes[index].parent;
    }
}

template<class Priority>
bool priority_less(const Priority& lhs, const Priority& rhs){
    return lhs < rhs;
}

template<class State, class Cost>
void update_solution(
    HeuristicSearchResult<State, Cost>& result,
    const std::vector<Node<State, Cost>>& nodes,
    std::size_t index
){
    if(result.cost && !((nodes[index].distance) < *result.cost)) return;
    result.cost = nodes[index].distance;
    result.path = heuristic_search_internal::restore_path(nodes, index);
}

} // namespace advanced_beam_search_internal

template<class State, class Goal, class Neighbors, class Heuristic, class URBG,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto stochastic_beam_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t beam_width,
    std::size_t maximum_depth,
    URBG& generator,
    long double temperature = 1.0L,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Node = advanced_beam_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Candidate{
        long double score;
        long double random_key;
        Cost distance;
        std::size_t order;
        std::size_t node;
    };

    if(beam_width == 0)[[unlikely]]{
        throw std::invalid_argument(
            "stochastic_beam_search requires a positive beam width"
        );
    }
    if(!(temperature > 0.0L) || !std::isfinite(temperature))[[unlikely]]{
        throw std::invalid_argument(
            "stochastic_beam_search requires a positive finite temperature"
        );
    }

    std::vector<Node> nodes;
    nodes.push_back(Node{start, Cost{}, 0, false});
    std::vector<std::size_t> beam{0};
    std::unordered_map<State, Cost, Hash, Equal> best_distance(0, hash, equal);
    best_distance.emplace(start, Cost{});
    std::size_t order = 0;
    Result result;

    for(std::size_t depth = 0; depth <= maximum_depth; ++depth){
        for(const std::size_t index : beam){
            if(static_cast<bool>(std::invoke(goal, nodes[index].state))){
                advanced_beam_search_internal::update_solution(result, nodes, index);
            }
        }
        if(result || depth == maximum_depth) return result;

        std::vector<Candidate> candidates;
        std::unordered_map<State, std::size_t, Hash, Equal> candidate_index(
            0, hash, equal
        );
        for(const std::size_t index : beam){
            ++result.expanded;
            const State current_state = nodes[index].state;
            const Cost current_distance = nodes[index].distance;
            auto adjacent = std::invoke(neighbors, current_state);
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "stochastic_beam_search does not accept negative edges"
                    );
                }
                const Cost distance = current_distance + edge_cost;
                const State& next_state = edge.first;
                const auto global_iterator = best_distance.find(next_state);
                if(global_iterator != best_distance.end()
                   && !(distance < global_iterator->second)){
                    continue;
                }
                const long double score = static_cast<long double>(distance)
                    + static_cast<long double>(std::invoke(heuristic, next_state));
                if(!std::isfinite(score))[[unlikely]]{
                    throw std::overflow_error(
                        "stochastic_beam_search priority is not finite"
                    );
                }

                const auto iterator = candidate_index.find(next_state);
                if(iterator != candidate_index.end()){
                    Candidate& old = candidates[iterator->second];
                    if(!(distance < old.distance)) continue;
                    old.distance = distance;
                    old.score = score;
                    nodes[old.node].distance = distance;
                    nodes[old.node].parent = index;
                    continue;
                }
                const std::size_t node = nodes.size();
                nodes.push_back(Node{next_state, distance, index, true});
                candidate_index.emplace(nodes.back().state, candidates.size());
                candidates.push_back(Candidate{
                    score, 0.0L, distance, order++, node
                });
            }
        }
        if(candidates.empty()) return result;

        long double minimum_score = candidates.front().score;
        for(const Candidate& candidate : candidates){
            minimum_score = std::min(minimum_score, candidate.score);
        }
        for(Candidate& candidate : candidates){
            long double uniform = std::generate_canonical<long double, 64>(generator);
            if(!(uniform > 0.0L)) uniform = std::numeric_limits<long double>::min();
            candidate.random_key = (candidate.score - minimum_score) / temperature
                + std::log(-std::log(uniform));
        }
        const std::size_t retained = std::min(beam_width, candidates.size());
        std::partial_sort(
            candidates.begin(), candidates.begin() + retained, candidates.end(),
            [](const Candidate& lhs, const Candidate& rhs){
                if(lhs.random_key < rhs.random_key) return true;
                if(rhs.random_key < lhs.random_key) return false;
                return lhs.order < rhs.order;
            }
        );
        beam.clear();
        beam.reserve(retained);
        for(std::size_t i = 0; i < retained; ++i){
            Candidate& candidate = candidates[i];
            const auto iterator = best_distance.find(nodes[candidate.node].state);
            if(iterator == best_distance.end()){
                best_distance.emplace(nodes[candidate.node].state, candidate.distance);
            }else{
                iterator->second = candidate.distance;
            }
            beam.push_back(candidate.node);
        }
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto beam_stack_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t beam_width,
    std::size_t maximum_depth,
    std::size_t maximum_expansions = std::numeric_limits<std::size_t>::max(),
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using HeuristicValue =
        std::decay_t<std::invoke_result_t<Heuristic&, const State&>>;
    using Priority = std::common_type_t<Cost, HeuristicValue>;
    using Node = advanced_beam_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Candidate{
        Priority priority;
        Cost distance;
        std::size_t order;
        std::size_t node;
    };
    struct Frame{
        std::vector<std::size_t> beam;
        std::vector<Candidate> candidates;
        std::size_t next_offset = 0;
        std::size_t depth = 0;
        bool prepared = false;
    };

    if(beam_width == 0)[[unlikely]]{
        throw std::invalid_argument(
            "beam_stack_search requires a positive beam width"
        );
    }

    std::vector<Node> nodes;
    nodes.push_back(Node{start, Cost{}, 0, false});
    std::vector<Frame> stack;
    stack.push_back(Frame{{0}, {}, 0, 0, false});
    std::size_t order = 0;
    Result result;

    while(!stack.empty()){
        Frame& frame = stack.back();
        if(!frame.prepared){
            for(const std::size_t index : frame.beam){
                if(static_cast<bool>(std::invoke(goal, nodes[index].state))){
                    advanced_beam_search_internal::update_solution(
                        result, nodes, index
                    );
                }
            }
            frame.prepared = true;
            if(frame.depth == maximum_depth
               || result.expanded == maximum_expansions){
                stack.pop_back();
                continue;
            }

            std::unordered_map<State, std::size_t, Hash, Equal> candidate_index(
                0, hash, equal
            );
            bool expansion_limit_reached = false;
            for(const std::size_t index : frame.beam){
                if(result.expanded == maximum_expansions){
                    expansion_limit_reached = true;
                    break;
                }
                ++result.expanded;
                const State current_state = nodes[index].state;
                const Cost current_distance = nodes[index].distance;
                auto adjacent = std::invoke(neighbors, current_state);
                for(const auto& edge : adjacent){
                    const Cost edge_cost = static_cast<Cost>(edge.second);
                    if(edge_cost < Cost{})[[unlikely]]{
                        throw std::invalid_argument(
                            "beam_stack_search does not accept negative edges"
                        );
                    }
                    const State& next_state = edge.first;
                    if(advanced_beam_search_internal::occurs_on_path(
                           nodes, index, next_state, equal)){
                        continue;
                    }
                    const Cost distance = current_distance + edge_cost;
                    const Priority priority = static_cast<Priority>(distance)
                        + static_cast<Priority>(
                            std::invoke(heuristic, next_state)
                        );
                    const auto iterator = candidate_index.find(next_state);
                    if(iterator != candidate_index.end()){
                        Candidate& old = frame.candidates[iterator->second];
                        if(!(distance < old.distance)) continue;
                        old.distance = distance;
                        old.priority = priority;
                        nodes[old.node].distance = distance;
                        nodes[old.node].parent = index;
                        continue;
                    }
                    const std::size_t node = nodes.size();
                    nodes.push_back(Node{next_state, distance, index, true});
                    candidate_index.emplace(
                        nodes.back().state, frame.candidates.size()
                    );
                    frame.candidates.push_back(Candidate{
                        priority, distance, order++, node
                    });
                }
            }
            std::sort(
                frame.candidates.begin(), frame.candidates.end(),
                [](const Candidate& lhs, const Candidate& rhs){
                    if(lhs.priority < rhs.priority) return true;
                    if(rhs.priority < lhs.priority) return false;
                    return lhs.order < rhs.order;
                }
            );
            if(expansion_limit_reached && frame.candidates.empty()){
                stack.pop_back();
                continue;
            }
        }

        if(frame.next_offset == frame.candidates.size()
           || result.expanded == maximum_expansions){
            stack.pop_back();
            continue;
        }
        const std::size_t end = std::min(
            frame.candidates.size(), frame.next_offset + beam_width
        );
        std::vector<std::size_t> next_beam;
        next_beam.reserve(end - frame.next_offset);
        for(std::size_t i = frame.next_offset; i < end; ++i){
            next_beam.push_back(frame.candidates[i].node);
        }
        frame.next_offset = end;
        const std::size_t next_depth = frame.depth + 1;
        stack.push_back(Frame{
            std::move(next_beam), {}, 0, next_depth, false
        });
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto beam_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t beam_width,
    std::size_t maximum_depth,
    std::size_t maximum_expansions = std::numeric_limits<std::size_t>::max(),
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using HeuristicValue =
        std::decay_t<std::invoke_result_t<Heuristic&, const State&>>;
    using Priority = std::common_type_t<Cost, HeuristicValue>;
    using BaseNode = advanced_beam_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Node : BaseNode{
        Priority priority;
        std::size_t depth;
        std::size_t order;
        bool open;
    };
    struct OpenCompare{
        bool operator()(const std::pair<Priority, std::size_t>& lhs,
                        const std::pair<Priority, std::size_t>& rhs) const{
            if(rhs.first < lhs.first) return true;
            if(lhs.first < rhs.first) return false;
            return rhs.second < lhs.second;
        }
    };
    struct WorstCompare{
        const std::vector<Node>* nodes = nullptr;

        bool operator()(std::size_t lhs, std::size_t rhs) const{
            const Node& left = (*nodes)[lhs];
            const Node& right = (*nodes)[rhs];
            if(left.priority < right.priority) return true;
            if(right.priority < left.priority) return false;
            return left.order < right.order;
        }
    };
    struct Bucket{
        std::priority_queue<
            std::size_t, std::vector<std::size_t>, WorstCompare
        > worst;
        std::size_t open_count = 0;

        explicit Bucket(const std::vector<Node>* nodes)
            : worst(WorstCompare{nodes}){}
    };

    if(beam_width == 0)[[unlikely]]{
        throw std::invalid_argument(
            "beam_a_star_search requires a positive beam width"
        );
    }

    std::vector<Node> nodes;
    nodes.reserve(1);
    const Priority start_priority =
        static_cast<Priority>(std::invoke(heuristic, start));
    nodes.push_back(Node{
        BaseNode{start, Cost{}, 0, false},
        start_priority, 0, 0, true
    });
    std::priority_queue<
        std::pair<Priority, std::size_t>,
        std::vector<std::pair<Priority, std::size_t>>,
        OpenCompare
    > open;
    open.emplace(start_priority, 0);
    std::vector<Bucket> buckets;
    buckets.reserve(maximum_depth + 1);
    for(std::size_t i = 0; i <= maximum_depth; ++i){
        buckets.emplace_back(&nodes);
    }
    buckets[0].worst.push(0);
    buckets[0].open_count = 1;
    std::unordered_map<State, Cost, Hash, Equal> best_distance(0, hash, equal);
    best_distance.emplace(start, Cost{});
    std::size_t order = 1;
    Result result;

    const auto remove_stale_worst = [&](Bucket& bucket){
        while(!bucket.worst.empty()
              && !nodes[bucket.worst.top()].open){
            bucket.worst.pop();
        }
    };
    const auto add_node = [&](std::size_t index){
        Bucket& bucket = buckets[nodes[index].depth];
        bucket.worst.push(index);
        ++bucket.open_count;
        open.emplace(nodes[index].priority, index);
        if(bucket.open_count <= beam_width) return;
        remove_stale_worst(bucket);
        const std::size_t removed = bucket.worst.top();
        bucket.worst.pop();
        nodes[removed].open = false;
        --bucket.open_count;
    };

    while(!open.empty() && result.expanded < maximum_expansions){
        const std::size_t index = open.top().second;
        open.pop();
        if(!nodes[index].open) continue;
        nodes[index].open = false;
        --buckets[nodes[index].depth].open_count;
        ++result.expanded;
        const State current_state = nodes[index].state;
        const Cost current_distance = nodes[index].distance;
        const std::size_t current_depth = nodes[index].depth;
        if(static_cast<bool>(std::invoke(goal, current_state))){
            std::vector<BaseNode> base_nodes;
            base_nodes.reserve(nodes.size());
            for(const Node& node : nodes){
                base_nodes.push_back(static_cast<const BaseNode&>(node));
            }
            advanced_beam_search_internal::update_solution(
                result, base_nodes, index
            );
            return result;
        }
        if(current_depth == maximum_depth) continue;

        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "beam_a_star_search does not accept negative edges"
                );
            }
            const Cost distance = current_distance + edge_cost;
            const State& next_state = edge.first;
            const auto iterator = best_distance.find(next_state);
            if(iterator != best_distance.end()
               && !(distance < iterator->second)){
                continue;
            }
            if(iterator == best_distance.end()){
                best_distance.emplace(next_state, distance);
            }else{
                iterator->second = distance;
            }
            const Priority priority = static_cast<Priority>(distance)
                + static_cast<Priority>(std::invoke(heuristic, next_state));
            const std::size_t next_index = nodes.size();
            nodes.push_back(Node{
                BaseNode{next_state, distance, index, true},
                priority, current_depth + 1, order++, true
            });
            add_node(next_index);
        }
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto beam_ida_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t beam_width,
    std::size_t maximum_depth,
    std::size_t maximum_iterations,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Successor{
        State state;
        Cost distance;
        Cost lower_bound;
        std::size_t order;
    };

    if(beam_width == 0)[[unlikely]]{
        throw std::invalid_argument(
            "beam_ida_star_search requires a positive beam width"
        );
    }
    if(maximum_iterations == 0)[[unlikely]]{
        throw std::invalid_argument(
            "beam_ida_star_search requires at least one iteration"
        );
    }

    const Cost initial_estimate =
        static_cast<Cost>(std::invoke(heuristic, start));
    if(initial_estimate < Cost{})[[unlikely]]{
        throw std::invalid_argument(
            "beam_ida_star_search requires a non-negative heuristic"
        );
    }
    Cost bound = initial_estimate;
    std::vector<State> path{start};
    std::unordered_set<State, Hash, Equal> on_path(0, hash, equal);
    on_path.insert(start);
    Result result;
    bool found = false;
    std::size_t order = 0;

    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        std::optional<Cost> next_bound;
        const auto dfs = [&](auto&& self, const State& state, Cost distance,
                             std::size_t depth) -> void{
            const Cost estimate =
                static_cast<Cost>(std::invoke(heuristic, state));
            if(estimate < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "beam_ida_star_search requires a non-negative heuristic"
                );
            }
            const Cost lower_bound = distance + estimate;
            if(bound < lower_bound){
                if(!next_bound || lower_bound < *next_bound){
                    next_bound = lower_bound;
                }
                return;
            }
            ++result.expanded;
            if(static_cast<bool>(std::invoke(goal, state))){
                result.cost = distance;
                result.path = path;
                found = true;
                return;
            }
            if(depth == maximum_depth) return;

            std::vector<Successor> successors;
            auto adjacent = std::invoke(neighbors, state);
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "beam_ida_star_search does not accept negative edges"
                    );
                }
                const State& next_state = edge.first;
                if(on_path.find(next_state) != on_path.end()) continue;
                const Cost next_distance = distance + edge_cost;
                const Cost next_estimate =
                    static_cast<Cost>(std::invoke(heuristic, next_state));
                if(next_estimate < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "beam_ida_star_search requires a non-negative heuristic"
                    );
                }
                successors.push_back(Successor{
                    next_state, next_distance,
                    next_distance + next_estimate, order++
                });
            }
            std::sort(
                successors.begin(), successors.end(),
                [](const Successor& lhs, const Successor& rhs){
                    if(lhs.lower_bound < rhs.lower_bound) return true;
                    if(rhs.lower_bound < lhs.lower_bound) return false;
                    return lhs.order < rhs.order;
                }
            );
            const std::size_t retained =
                std::min(beam_width, successors.size());
            for(std::size_t i = retained; i < successors.size(); ++i){
                if(bound < successors[i].lower_bound
                   && (!next_bound
                       || successors[i].lower_bound < *next_bound)){
                    next_bound = successors[i].lower_bound;
                }
            }
            for(std::size_t i = 0; i < retained && !found; ++i){
                const Successor& successor = successors[i];
                if(bound < successor.lower_bound){
                    if(!next_bound || successor.lower_bound < *next_bound){
                        next_bound = successor.lower_bound;
                    }
                    continue;
                }
                on_path.insert(successor.state);
                path.push_back(successor.state);
                self(self, successor.state, successor.distance, depth + 1);
                path.pop_back();
                on_path.erase(successor.state);
            }
        };

        dfs(dfs, start, Cost{}, 0);
        if(found || !next_bound || !((bound) < *next_bound)) return result;
        bound = *next_bound;
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto chokudai_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t beam_width,
    std::size_t maximum_depth,
    std::size_t iterations,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using HeuristicValue =
        std::decay_t<std::invoke_result_t<Heuristic&, const State&>>;
    using Priority = std::common_type_t<Cost, HeuristicValue>;
    using Node = advanced_beam_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Entry{
        Priority priority;
        Cost distance;
        std::size_t order;
        std::size_t node;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(rhs.priority < lhs.priority) return true;
            if(lhs.priority < rhs.priority) return false;
            return rhs.order < lhs.order;
        }
    };

    if(beam_width == 0)[[unlikely]]{
        throw std::invalid_argument(
            "chokudai_search requires a positive beam width"
        );
    }
    if(iterations == 0)[[unlikely]]{
        throw std::invalid_argument(
            "chokudai_search requires at least one iteration"
        );
    }

    std::vector<Node> nodes;
    nodes.push_back(Node{start, Cost{}, 0, false});
    std::vector<
        std::priority_queue<Entry, std::vector<Entry>, Compare>
    > queues(maximum_depth + 1);
    std::vector<std::unordered_map<State, Cost, Hash, Equal>> best;
    best.reserve(maximum_depth + 1);
    for(std::size_t i = 0; i <= maximum_depth; ++i){
        best.emplace_back(0, hash, equal);
    }
    const Priority start_priority =
        static_cast<Priority>(std::invoke(heuristic, start));
    queues[0].push(Entry{start_priority, Cost{}, 0, 0});
    best[0].emplace(start, Cost{});
    std::size_t order = 1;
    Result result;

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        bool progressed = false;
        for(std::size_t depth = 0; depth <= maximum_depth; ++depth){
            std::size_t selected = 0;
            while(selected < beam_width && !queues[depth].empty()){
                const Entry entry = queues[depth].top();
                queues[depth].pop();
                const auto iterator = best[depth].find(nodes[entry.node].state);
                if(iterator == best[depth].end()
                   || entry.distance != iterator->second){
                    continue;
                }
                best[depth].erase(iterator);
                ++selected;
                progressed = true;
                ++result.expanded;
                const State current_state = nodes[entry.node].state;
                const Cost current_distance = nodes[entry.node].distance;
                if(static_cast<bool>(std::invoke(goal, current_state))){
                    advanced_beam_search_internal::update_solution(
                        result, nodes, entry.node
                    );
                    continue;
                }
                if(depth == maximum_depth) continue;

                auto adjacent = std::invoke(neighbors, current_state);
                for(const auto& edge : adjacent){
                    const Cost edge_cost = static_cast<Cost>(edge.second);
                    if(edge_cost < Cost{})[[unlikely]]{
                        throw std::invalid_argument(
                            "chokudai_search does not accept negative edges"
                        );
                    }
                    const Cost distance = current_distance + edge_cost;
                    const State& next_state = edge.first;
                    const auto old = best[depth + 1].find(next_state);
                    if(old != best[depth + 1].end()
                       && !(distance < old->second)){
                        continue;
                    }
                    if(old == best[depth + 1].end()){
                        best[depth + 1].emplace(next_state, distance);
                    }else{
                        old->second = distance;
                    }
                    const Priority priority = static_cast<Priority>(distance)
                        + static_cast<Priority>(
                            std::invoke(heuristic, next_state)
                        );
                    const std::size_t node = nodes.size();
                    nodes.push_back(Node{
                        next_state, distance, entry.node, true
                    });
                    queues[depth + 1].push(Entry{
                        priority, distance, order++, node
                    });
                }
            }
        }
        if(!progressed) break;
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_BEAM_SEARCH_HPP_INCLUDED
