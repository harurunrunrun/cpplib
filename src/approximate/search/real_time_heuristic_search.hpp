#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_REAL_TIME_HEURISTIC_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_REAL_TIME_HEURISTIC_SEARCH_HPP_INCLUDED

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

template<class State, class Cost>
struct RealTimeHeuristicSearchResult{
    std::vector<State> path;
    std::optional<Cost> cost;
    std::vector<std::pair<State, Cost>> learned_heuristic;
    std::size_t expanded = 0;
    bool cutoff = false;

    [[nodiscard]] explicit operator bool() const noexcept{
        return cost.has_value();
    }
};

namespace real_time_heuristic_search_internal{

template<class Range>
using range_value_t = std::decay_t<decltype(*std::begin(
    std::declval<Range&>()
))>;

template<class Cost>
void validate_non_negative(Cost value, const char* message){
    if constexpr(std::is_signed_v<Cost>){
        if(value < Cost{}) throw std::invalid_argument(message);
    }
    if constexpr(std::is_floating_point_v<Cost>){
        if(!std::isfinite(value) || value < Cost{}){
            throw std::invalid_argument(message);
        }
    }
}

template<class Cost>
Cost add(Cost lhs, Cost rhs){
    validate_non_negative(lhs, "search values must be finite and non-negative");
    validate_non_negative(rhs, "search values must be finite and non-negative");
    if constexpr(std::is_integral_v<Cost>){
        if(rhs > std::numeric_limits<Cost>::max() - lhs){
            throw std::overflow_error("real-time search cost overflow");
        }
    }
    const Cost result = lhs + rhs;
    validate_non_negative(result, "search values must be finite and non-negative");
    return result;
}

template<class State, class Cost, class Heuristic, class Hash, class Equal>
class LearnedValues{
public:
    LearnedValues(Heuristic& heuristic, Hash hash, Equal equal)
        : heuristic_(heuristic), values_(0, std::move(hash), std::move(equal)){}

    Cost get(const State& state){
        const auto iterator = values_.find(state);
        if(iterator != values_.end()) return iterator->second;
        const Cost value = static_cast<Cost>(std::invoke(heuristic_, state));
        validate_non_negative(
            value, "heuristic estimates must be finite and non-negative"
        );
        values_.emplace(state, value);
        return value;
    }

    void raise(const State& state, Cost value){
        validate_non_negative(
            value, "learned heuristic values must be finite and non-negative"
        );
        const Cost old = get(state);
        if(old < value) values_[state] = value;
    }

    [[nodiscard]] std::vector<std::pair<State, Cost>> entries() const{
        std::vector<std::pair<State, Cost>> result;
        result.reserve(values_.size());
        for(const auto& item : values_) result.push_back(item);
        return result;
    }

private:
    Heuristic& heuristic_;
    std::unordered_map<State, Cost, Hash, Equal> values_;
};

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash, class Equal>
auto one_step_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t max_moves,
    bool second_best_backup,
    Hash hash,
    Equal equal
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = RealTimeHeuristicSearchResult<State, Cost>;

    LearnedValues<State, Cost, Heuristic, Hash, Equal> learned(
        heuristic, hash, equal
    );
    Result result;
    result.path.push_back(start);
    State current = start;
    Cost total{};
    for(std::size_t move = 0; move <= max_moves; ++move){
        if(static_cast<bool>(std::invoke(goal, current))){
            result.cost = total;
            result.learned_heuristic = learned.entries();
            return result;
        }
        if(move == max_moves) break;
        auto adjacent = std::invoke(neighbors, current);
        std::optional<std::pair<State, Cost>> best;
        std::optional<Cost> second;
        ++result.expanded;
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            validate_non_negative(
                edge_cost, "real-time search does not accept negative edges"
            );
            const Cost value = add(edge_cost, learned.get(edge.first));
            if(!best || value < best->second){
                if(best) second = best->second;
                best = std::pair<State, Cost>{edge.first, value};
            }else if(!second || value < *second){
                second = value;
            }
        }
        if(!best) break;
        learned.raise(
            current,
            second_best_backup && second ? *second : best->second
        );
        auto adjacent_again = std::invoke(neighbors, current);
        std::optional<Cost> chosen_cost;
        for(const auto& edge : adjacent_again){
            if(equal(edge.first, best->first)){
                const Cost value = static_cast<Cost>(edge.second);
                if(!chosen_cost || value < *chosen_cost) chosen_cost = value;
            }
        }
        if(!chosen_cost){
            throw std::logic_error("neighbors changed during real-time search");
        }
        total = add(total, *chosen_cost);
        current = best->first;
        result.path.push_back(current);
    }
    result.cutoff = result.path.size() == max_moves + 1;
    result.learned_heuristic = learned.entries();
    return result;
}

template<class State, class Cost>
struct LookaheadNode{
    State state;
    Cost distance{};
    std::size_t parent = 0;
    std::size_t version = 0;
    bool closed = false;
    bool has_parent = false;
};

template<class State, class Cost>
std::vector<std::size_t> index_path(
    const std::vector<LookaheadNode<State, Cost>>& nodes,
    std::size_t index
){
    std::vector<std::size_t> path;
    for(;;){
        path.push_back(index);
        if(!nodes[index].has_parent) break;
        index = nodes[index].parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash, class Equal>
auto limited_learning_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t lookahead,
    std::size_t max_moves,
    bool lss_backup,
    Hash hash,
    Equal equal
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = RealTimeHeuristicSearchResult<State, Cost>;
    using Node = LookaheadNode<State, Cost>;
    if(lookahead == 0){
        throw std::invalid_argument("real-time lookahead must be positive");
    }

    LearnedValues<State, Cost, Heuristic, Hash, Equal> learned(
        heuristic, hash, equal
    );
    Result result;
    result.path.push_back(start);
    State current = start;
    Cost total{};
    std::size_t moves = 0;
    while(moves < max_moves
          && !static_cast<bool>(std::invoke(goal, current))){
        struct Entry{
            Cost key;
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
        std::vector<Node> nodes;
        std::unordered_map<State, std::size_t, Hash, Equal> index(
            0, hash, equal
        );
        std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
        std::size_t order = 0;
        nodes.push_back(Node{current});
        index.emplace(current, 0);
        queue.push(Entry{learned.get(current), Cost{}, order++, 0, 0});
        std::optional<std::size_t> found_goal;
        std::vector<std::size_t> closed;

        while(!queue.empty() && closed.size() < lookahead){
            const Entry entry = queue.top();
            queue.pop();
            Node& node = nodes[entry.node];
            if(node.closed || node.version != entry.version
               || node.distance != entry.distance) continue;
            node.closed = true;
            closed.push_back(entry.node);
            ++result.expanded;
            const State current_state = node.state;
            const Cost current_distance = node.distance;
            if(static_cast<bool>(std::invoke(goal, current_state))){
                found_goal = entry.node;
                break;
            }
            auto adjacent = std::invoke(neighbors, current_state);
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                validate_non_negative(
                    edge_cost, "real-time search does not accept negative edges"
                );
                const Cost candidate = add(current_distance, edge_cost);
                auto iterator = index.find(edge.first);
                std::size_t next_index;
                if(iterator == index.end()){
                    next_index = nodes.size();
                    nodes.push_back(Node{
                        edge.first, candidate, entry.node, 0, false, true
                    });
                    index.emplace(nodes.back().state, next_index);
                }else{
                    next_index = iterator->second;
                    Node& next = nodes[next_index];
                    if(!(candidate < next.distance)) continue;
                    next.distance = candidate;
                    next.parent = entry.node;
                    next.has_parent = true;
                    next.closed = false;
                    ++next.version;
                }
                const Cost key = add(
                    nodes[next_index].distance,
                    learned.get(nodes[next_index].state)
                );
                queue.push(Entry{
                    key, nodes[next_index].distance, order++,
                    next_index, nodes[next_index].version
                });
            }
        }

        std::optional<std::size_t> frontier;
        Cost frontier_key{};
        while(!queue.empty()){
            const Entry entry = queue.top();
            queue.pop();
            const Node& node = nodes[entry.node];
            if(node.closed || node.version != entry.version
               || node.distance != entry.distance) continue;
            frontier = entry.node;
            frontier_key = entry.key;
            break;
        }
        if(found_goal){
            frontier = found_goal;
            frontier_key = nodes[*found_goal].distance;
        }
        if(!frontier) break;

        if(!lss_backup){
            for(const std::size_t node_index : closed){
                const Cost distance = nodes[node_index].distance;
                if(distance < frontier_key){
                    learned.raise(
                        nodes[node_index].state, frontier_key - distance
                    );
                }
            }
        }else{
            struct ReverseEntry{
                Cost value;
                std::size_t node;
            };
            struct ReverseCompare{
                bool operator()(
                    const ReverseEntry& lhs,
                    const ReverseEntry& rhs
                ) const{
                    return rhs.value < lhs.value;
                }
            };
            std::vector<std::vector<std::pair<std::size_t, Cost>>> reverse(
                nodes.size()
            );
            for(std::size_t from = 0; from < nodes.size(); ++from){
                if(!nodes[from].closed) continue;
                auto adjacent = std::invoke(neighbors, nodes[from].state);
                for(const auto& edge : adjacent){
                    const auto iterator = index.find(edge.first);
                    if(iterator != index.end()){
                        reverse[iterator->second].emplace_back(
                            from, static_cast<Cost>(edge.second)
                        );
                    }
                }
            }
            const Cost infinity = std::numeric_limits<Cost>::max();
            std::vector<Cost> backup(nodes.size(), infinity);
            std::priority_queue<
                ReverseEntry, std::vector<ReverseEntry>, ReverseCompare
            > reverse_queue;
            for(std::size_t i = 0; i < nodes.size(); ++i){
                if(nodes[i].closed) continue;
                backup[i] = learned.get(nodes[i].state);
                reverse_queue.push(ReverseEntry{backup[i], i});
            }
            if(found_goal){
                backup[*found_goal] = Cost{};
                reverse_queue.push(ReverseEntry{Cost{}, *found_goal});
            }
            while(!reverse_queue.empty()){
                const ReverseEntry entry = reverse_queue.top();
                reverse_queue.pop();
                if(backup[entry.node] != entry.value) continue;
                for(const auto& [predecessor, edge_cost] :
                    reverse[entry.node]){
                    const Cost candidate = add(entry.value, edge_cost);
                    if(candidate < backup[predecessor]){
                        backup[predecessor] = candidate;
                        reverse_queue.push(
                            ReverseEntry{candidate, predecessor}
                        );
                    }
                }
            }
            for(const std::size_t node_index : closed){
                if(backup[node_index] != infinity){
                    learned.raise(nodes[node_index].state, backup[node_index]);
                }
            }
        }

        const std::vector<std::size_t> plan = index_path(nodes, *frontier);
        if(plan.size() < 2) break;
        const std::size_t execute =
            found_goal ? plan.size() - 1 : std::size_t{1};
        for(std::size_t i = 1;
            i <= execute && moves < max_moves;
            ++i, ++moves){
            const State& from = nodes[plan[i - 1]].state;
            const State& to = nodes[plan[i]].state;
            std::optional<Cost> edge_cost;
            auto adjacent = std::invoke(neighbors, from);
            for(const auto& edge : adjacent){
                if(equal(edge.first, to)){
                    const Cost value = static_cast<Cost>(edge.second);
                    if(!edge_cost || value < *edge_cost) edge_cost = value;
                }
            }
            if(!edge_cost){
                throw std::logic_error("neighbors changed during real-time search");
            }
            total = add(total, *edge_cost);
            current = to;
            result.path.push_back(current);
        }
    }
    if(static_cast<bool>(std::invoke(goal, current))){
        result.cost = total;
    }else if(moves == max_moves){
        result.cutoff = true;
    }
    result.learned_heuristic = learned.entries();
    return result;
}

} // namespace real_time_heuristic_search_internal

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto lrta_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t max_moves,
    Hash hash = {},
    Equal equal = {}
){
    return real_time_heuristic_search_internal::one_step_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        max_moves, false, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto rta_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t max_moves,
    Hash hash = {},
    Equal equal = {}
){
    return real_time_heuristic_search_internal::one_step_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        max_moves, true, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto rtaa_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t lookahead,
    std::size_t max_moves,
    Hash hash = {},
    Equal equal = {}
){
    return real_time_heuristic_search_internal::limited_learning_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        lookahead, max_moves, false, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto lss_lrta_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t lookahead,
    std::size_t max_moves,
    Hash hash = {},
    Equal equal = {}
){
    return real_time_heuristic_search_internal::limited_learning_search(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        lookahead, max_moves, true, std::move(hash), std::move(equal)
    );
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_REAL_TIME_HEURISTIC_SEARCH_HPP_INCLUDED
