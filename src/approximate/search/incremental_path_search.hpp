#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_INCREMENTAL_PATH_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_INCREMENTAL_PATH_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "grid_path_search.hpp"

template<class Cost>
struct IncrementalPathResult{
    std::optional<Cost> cost;
    std::vector<int> path;
    std::size_t expanded = 0;

    explicit operator bool() const noexcept{ return cost.has_value(); }
};

namespace incremental_path_search_internal{

template<class Cost>
class DynamicDirectedGraph{
    std::vector<std::unordered_map<int, Cost>> outgoing_;
    std::vector<std::unordered_map<int, Cost>> incoming_;

    void check_vertex(int vertex) const{
        if(vertex < 0 || static_cast<std::size_t>(vertex) >= outgoing_.size()){
            throw std::out_of_range("incremental planner vertex is out of range");
        }
    }

public:
    explicit DynamicDirectedGraph(int vertex_count)
        : outgoing_(static_cast<std::size_t>(std::max(vertex_count, 0))),
          incoming_(outgoing_.size()){
        if(vertex_count < 0){
            throw std::invalid_argument("vertex_count must be non-negative");
        }
    }

    int vertex_count() const noexcept{
        return static_cast<int>(outgoing_.size());
    }

    const std::unordered_map<int, Cost>& outgoing(int vertex) const{
        check_vertex(vertex);
        return outgoing_[static_cast<std::size_t>(vertex)];
    }

    const std::unordered_map<int, Cost>& incoming(int vertex) const{
        check_vertex(vertex);
        return incoming_[static_cast<std::size_t>(vertex)];
    }

    void set_edge(int from, int to, Cost cost){
        check_vertex(from);
        check_vertex(to);
        if(!(Cost{} < cost)){
            throw std::invalid_argument(
                "incremental planners require strictly positive edge costs"
            );
        }
        outgoing_[static_cast<std::size_t>(from)][to] = cost;
        incoming_[static_cast<std::size_t>(to)][from] = cost;
    }

    void remove_edge(int from, int to){
        check_vertex(from);
        check_vertex(to);
        outgoing_[static_cast<std::size_t>(from)].erase(to);
        incoming_[static_cast<std::size_t>(to)].erase(from);
    }
};

template<class Cost>
Cost default_infinity(){
    if constexpr(std::numeric_limits<Cost>::has_infinity){
        return std::numeric_limits<Cost>::infinity();
    }else{
        return std::numeric_limits<Cost>::max() / Cost{4};
    }
}

template<class Cost>
Cost safe_add(Cost first, Cost second, Cost infinity){
    if(!(first < infinity) || !(second < infinity)) return infinity;
    if constexpr(std::numeric_limits<Cost>::is_integer){
        if(second > Cost{} && first > infinity - second) return infinity;
    }
    const Cost result = first + second;
    return result < infinity ? result : infinity;
}

template<class Cost>
struct Key{
    Cost first;
    Cost second;
};

template<class Cost>
bool key_less(const Key<Cost>& lhs, const Key<Cost>& rhs){
    return lhs.first < rhs.first
        || (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

template<class Cost>
struct QueueEntry{
    Key<Cost> key;
    std::size_t version;
    std::size_t order;
    int vertex;
};

template<class Cost>
struct QueueCompare{
    bool operator()(
        const QueueEntry<Cost>& lhs,
        const QueueEntry<Cost>& rhs
    ) const{
        if(key_less(rhs.key, lhs.key)) return true;
        if(key_less(lhs.key, rhs.key)) return false;
        return lhs.order > rhs.order;
    }
};

inline void check_endpoint(int vertex_count, int vertex, const char* name){
    if(vertex < 0 || vertex >= vertex_count){
        throw std::out_of_range(name);
    }
}

} // namespace incremental_path_search_internal

template<class Cost>
class LifelongPlanningAStar{
    using Graph = incremental_path_search_internal::DynamicDirectedGraph<Cost>;
    using Key = incremental_path_search_internal::Key<Cost>;
    using Entry = incremental_path_search_internal::QueueEntry<Cost>;
    using Queue = std::priority_queue<
        Entry,
        std::vector<Entry>,
        incremental_path_search_internal::QueueCompare<Cost>
    >;

    Graph graph_;
    int start_;
    int goal_;
    std::function<Cost(int, int)> heuristic_;
    Cost infinity_;
    std::vector<Cost> g_;
    std::vector<Cost> rhs_;
    std::vector<std::size_t> version_;
    Queue queue_;
    std::size_t order_ = 0;
    std::size_t expanded_last_ = 0;

    Cost add(Cost first, Cost second) const{
        return incremental_path_search_internal::safe_add(
            first, second, infinity_
        );
    }

    bool inconsistent(int vertex) const{
        return g_[static_cast<std::size_t>(vertex)]
            != rhs_[static_cast<std::size_t>(vertex)];
    }

    Key calculate_key(int vertex) const{
        const Cost minimum = std::min(
            g_[static_cast<std::size_t>(vertex)],
            rhs_[static_cast<std::size_t>(vertex)]
        );
        return Key{add(minimum, heuristic_(vertex, goal_)), minimum};
    }

    void push(int vertex){
        const std::size_t index = static_cast<std::size_t>(vertex);
        ++version_[index];
        if(inconsistent(vertex)){
            queue_.push(Entry{
                calculate_key(vertex), version_[index], order_++, vertex
            });
        }
    }

    void update_vertex(int vertex){
        if(vertex != start_){
            Cost best = infinity_;
            for(const auto& [predecessor, cost] : graph_.incoming(vertex)){
                best = std::min(
                    best,
                    add(g_[static_cast<std::size_t>(predecessor)], cost)
                );
            }
            rhs_[static_cast<std::size_t>(vertex)] = best;
        }
        push(vertex);
    }

    void discard_stale(){
        while(!queue_.empty()){
            const Entry& entry = queue_.top();
            if(entry.version
                   == version_[static_cast<std::size_t>(entry.vertex)]
               && inconsistent(entry.vertex)){
                break;
            }
            queue_.pop();
        }
    }

    void compute_shortest_path(){
        expanded_last_ = 0;
        while(true){
            discard_stale();
            const Key goal_key = calculate_key(goal_);
            if(queue_.empty()
               || (!incremental_path_search_internal::key_less(
                       queue_.top().key, goal_key
                   )
                   && !inconsistent(goal_))){
                break;
            }
            const Entry entry = queue_.top();
            queue_.pop();
            const int vertex = entry.vertex;
            ++expanded_last_;
            const std::size_t index = static_cast<std::size_t>(vertex);
            if(g_[index] > rhs_[index]){
                g_[index] = rhs_[index];
                ++version_[index];
                for(const auto& [successor, cost] : graph_.outgoing(vertex)){
                    static_cast<void>(cost);
                    update_vertex(successor);
                }
            }else{
                g_[index] = infinity_;
                update_vertex(vertex);
                for(const auto& [successor, cost] : graph_.outgoing(vertex)){
                    static_cast<void>(cost);
                    update_vertex(successor);
                }
            }
        }
    }

public:
    LifelongPlanningAStar(
        int vertex_count,
        int start,
        int goal,
        std::function<Cost(int, int)> heuristic = {},
        Cost infinity = incremental_path_search_internal::default_infinity<Cost>()
    )
        : graph_(vertex_count),
          start_(start),
          goal_(goal),
          heuristic_(
              heuristic
                  ? std::move(heuristic)
                  : std::function<Cost(int, int)>(
                      [](int, int){ return Cost{}; }
                  )
          ),
          infinity_(infinity),
          g_(static_cast<std::size_t>(vertex_count), infinity),
          rhs_(g_),
          version_(g_.size(), 0){
        incremental_path_search_internal::check_endpoint(
            vertex_count, start, "LPA* start is out of range"
        );
        incremental_path_search_internal::check_endpoint(
            vertex_count, goal, "LPA* goal is out of range"
        );
        if(!(Cost{} < infinity_)){
            throw std::invalid_argument("LPA* infinity must be positive");
        }
        rhs_[static_cast<std::size_t>(start_)] = Cost{};
        push(start_);
    }

    void set_edge(int from, int to, Cost cost){
        graph_.set_edge(from, to, cost);
        update_vertex(to);
    }

    void remove_edge(int from, int to){
        graph_.remove_edge(from, to);
        update_vertex(to);
    }

    IncrementalPathResult<Cost> replan(){
        compute_shortest_path();
        IncrementalPathResult<Cost> result;
        result.expanded = expanded_last_;
        if(!(g_[static_cast<std::size_t>(goal_)] < infinity_)) return result;
        result.cost = g_[static_cast<std::size_t>(goal_)];
        std::vector<int> reverse_path{goal_};
        int current = goal_;
        for(int guard = 0; current != start_ && guard < graph_.vertex_count();
            ++guard){
            Cost best = infinity_;
            int predecessor = -1;
            for(const auto& [candidate, cost] : graph_.incoming(current)){
                const Cost value =
                    add(g_[static_cast<std::size_t>(candidate)], cost);
                if(value < best){
                    best = value;
                    predecessor = candidate;
                }
            }
            if(predecessor < 0) return IncrementalPathResult<Cost>{};
            current = predecessor;
            reverse_path.push_back(current);
        }
        if(current != start_) return IncrementalPathResult<Cost>{};
        std::reverse(reverse_path.begin(), reverse_path.end());
        result.path = std::move(reverse_path);
        return result;
    }

    Cost g_value(int vertex) const{
        incremental_path_search_internal::check_endpoint(
            graph_.vertex_count(), vertex, "LPA* vertex is out of range"
        );
        return g_[static_cast<std::size_t>(vertex)];
    }
};

template<class Cost>
using LPAStar = LifelongPlanningAStar<Cost>;

template<class Cost>
class DStar{
    using Graph = incremental_path_search_internal::DynamicDirectedGraph<Cost>;
    using Key = incremental_path_search_internal::Key<Cost>;
    using Entry = incremental_path_search_internal::QueueEntry<Cost>;
    using Queue = std::priority_queue<
        Entry,
        std::vector<Entry>,
        incremental_path_search_internal::QueueCompare<Cost>
    >;

    Graph graph_;
    int start_;
    int goal_;
    Cost infinity_;
    std::vector<Cost> g_;
    std::vector<Cost> rhs_;
    std::vector<std::size_t> version_;
    Queue queue_;
    std::size_t order_ = 0;
    std::size_t expanded_last_ = 0;

    Cost add(Cost first, Cost second) const{
        return incremental_path_search_internal::safe_add(
            first, second, infinity_
        );
    }

    bool inconsistent(int vertex) const{
        return g_[static_cast<std::size_t>(vertex)]
            != rhs_[static_cast<std::size_t>(vertex)];
    }

    Key calculate_key(int vertex) const{
        const Cost minimum = std::min(
            g_[static_cast<std::size_t>(vertex)],
            rhs_[static_cast<std::size_t>(vertex)]
        );
        return Key{minimum, minimum};
    }

    void push(int vertex){
        const std::size_t index = static_cast<std::size_t>(vertex);
        ++version_[index];
        if(inconsistent(vertex)){
            queue_.push(Entry{
                calculate_key(vertex), version_[index], order_++, vertex
            });
        }
    }

    void update_vertex(int vertex){
        if(vertex != goal_){
            Cost best = infinity_;
            for(const auto& [successor, cost] : graph_.outgoing(vertex)){
                best = std::min(
                    best,
                    add(cost, g_[static_cast<std::size_t>(successor)])
                );
            }
            rhs_[static_cast<std::size_t>(vertex)] = best;
        }
        push(vertex);
    }

    void discard_stale(){
        while(!queue_.empty()){
            const Entry& entry = queue_.top();
            if(entry.version
                   == version_[static_cast<std::size_t>(entry.vertex)]
               && inconsistent(entry.vertex)){
                break;
            }
            queue_.pop();
        }
    }

    void compute_shortest_path(){
        expanded_last_ = 0;
        while(true){
            discard_stale();
            const Key start_key = calculate_key(start_);
            if(queue_.empty()
               || (!incremental_path_search_internal::key_less(
                       queue_.top().key, start_key
                   )
                   && !inconsistent(start_))){
                break;
            }
            const Entry entry = queue_.top();
            queue_.pop();
            const int vertex = entry.vertex;
            ++expanded_last_;
            const std::size_t index = static_cast<std::size_t>(vertex);
            if(g_[index] > rhs_[index]){
                g_[index] = rhs_[index];
                ++version_[index];
                for(const auto& [predecessor, cost] : graph_.incoming(vertex)){
                    static_cast<void>(cost);
                    update_vertex(predecessor);
                }
            }else{
                g_[index] = infinity_;
                update_vertex(vertex);
                for(const auto& [predecessor, cost] : graph_.incoming(vertex)){
                    static_cast<void>(cost);
                    update_vertex(predecessor);
                }
            }
        }
    }

    IncrementalPathResult<Cost> make_result() const{
        IncrementalPathResult<Cost> result;
        result.expanded = expanded_last_;
        if(!(g_[static_cast<std::size_t>(start_)] < infinity_)) return result;
        result.cost = g_[static_cast<std::size_t>(start_)];
        result.path.push_back(start_);
        int current = start_;
        for(int guard = 0; current != goal_ && guard < graph_.vertex_count();
            ++guard){
            Cost best = infinity_;
            int successor = -1;
            for(const auto& [candidate, cost] : graph_.outgoing(current)){
                const Cost value =
                    add(cost, g_[static_cast<std::size_t>(candidate)]);
                if(value < best){
                    best = value;
                    successor = candidate;
                }
            }
            if(successor < 0) return IncrementalPathResult<Cost>{};
            current = successor;
            result.path.push_back(current);
        }
        if(current != goal_) return IncrementalPathResult<Cost>{};
        return result;
    }

public:
    DStar(
        int vertex_count,
        int start,
        int goal,
        Cost infinity = incremental_path_search_internal::default_infinity<Cost>()
    )
        : graph_(vertex_count),
          start_(start),
          goal_(goal),
          infinity_(infinity),
          g_(static_cast<std::size_t>(vertex_count), infinity),
          rhs_(g_),
          version_(g_.size(), 0){
        incremental_path_search_internal::check_endpoint(
            vertex_count, start, "D* start is out of range"
        );
        incremental_path_search_internal::check_endpoint(
            vertex_count, goal, "D* goal is out of range"
        );
        if(!(Cost{} < infinity_)){
            throw std::invalid_argument("D* infinity must be positive");
        }
        rhs_[static_cast<std::size_t>(goal_)] = Cost{};
        push(goal_);
    }

    void set_edge(int from, int to, Cost cost){
        graph_.set_edge(from, to, cost);
        update_vertex(from);
    }

    void remove_edge(int from, int to){
        graph_.remove_edge(from, to);
        update_vertex(from);
    }

    void move_start(int start){
        incremental_path_search_internal::check_endpoint(
            graph_.vertex_count(), start, "D* start is out of range"
        );
        start_ = start;
    }

    IncrementalPathResult<Cost> replan(){
        compute_shortest_path();
        return make_result();
    }
};

template<class Cost>
class FocusedDStar{
    using Graph = incremental_path_search_internal::DynamicDirectedGraph<Cost>;
    using Key = incremental_path_search_internal::Key<Cost>;
    using Entry = incremental_path_search_internal::QueueEntry<Cost>;
    using Queue = std::priority_queue<
        Entry,
        std::vector<Entry>,
        incremental_path_search_internal::QueueCompare<Cost>
    >;

    Graph graph_;
    int start_;
    int goal_;
    std::function<Cost(int, int)> heuristic_;
    Cost infinity_;
    std::vector<Cost> g_;
    std::vector<Cost> rhs_;
    std::vector<std::size_t> version_;
    Queue queue_;
    std::size_t order_ = 0;
    std::size_t expanded_last_ = 0;

    Cost add(Cost first, Cost second) const{
        return incremental_path_search_internal::safe_add(
            first, second, infinity_
        );
    }

    bool inconsistent(int vertex) const{
        return g_[static_cast<std::size_t>(vertex)]
            != rhs_[static_cast<std::size_t>(vertex)];
    }

    Key calculate_key(int vertex) const{
        const Cost minimum = std::min(
            g_[static_cast<std::size_t>(vertex)],
            rhs_[static_cast<std::size_t>(vertex)]
        );
        return Key{add(minimum, heuristic_(start_, vertex)), minimum};
    }

    void push(int vertex){
        const std::size_t index = static_cast<std::size_t>(vertex);
        ++version_[index];
        if(inconsistent(vertex)){
            queue_.push(Entry{
                calculate_key(vertex), version_[index], order_++, vertex
            });
        }
    }

    void rebuild_queue(){
        queue_ = Queue{};
        for(int vertex = 0; vertex < graph_.vertex_count(); ++vertex){
            push(vertex);
        }
    }

    void update_vertex(int vertex){
        if(vertex != goal_){
            Cost best = infinity_;
            for(const auto& [successor, cost] : graph_.outgoing(vertex)){
                best = std::min(
                    best,
                    add(cost, g_[static_cast<std::size_t>(successor)])
                );
            }
            rhs_[static_cast<std::size_t>(vertex)] = best;
        }
        push(vertex);
    }

    void discard_stale(){
        while(!queue_.empty()){
            const Entry& entry = queue_.top();
            if(entry.version
                   == version_[static_cast<std::size_t>(entry.vertex)]
               && inconsistent(entry.vertex)){
                break;
            }
            queue_.pop();
        }
    }

    void compute_shortest_path(){
        expanded_last_ = 0;
        while(true){
            discard_stale();
            const Key start_key = calculate_key(start_);
            if(queue_.empty()
               || (!incremental_path_search_internal::key_less(
                       queue_.top().key, start_key
                   )
                   && !inconsistent(start_))){
                break;
            }
            const Entry entry = queue_.top();
            queue_.pop();
            const int vertex = entry.vertex;
            ++expanded_last_;
            const std::size_t index = static_cast<std::size_t>(vertex);
            if(g_[index] > rhs_[index]){
                g_[index] = rhs_[index];
                ++version_[index];
                for(const auto& [predecessor, cost] : graph_.incoming(vertex)){
                    static_cast<void>(cost);
                    update_vertex(predecessor);
                }
            }else{
                g_[index] = infinity_;
                update_vertex(vertex);
                for(const auto& [predecessor, cost] : graph_.incoming(vertex)){
                    static_cast<void>(cost);
                    update_vertex(predecessor);
                }
            }
        }
    }

public:
    FocusedDStar(
        int vertex_count,
        int start,
        int goal,
        std::function<Cost(int, int)> heuristic,
        Cost infinity = incremental_path_search_internal::default_infinity<Cost>()
    )
        : graph_(vertex_count),
          start_(start),
          goal_(goal),
          heuristic_(std::move(heuristic)),
          infinity_(infinity),
          g_(static_cast<std::size_t>(vertex_count), infinity),
          rhs_(g_),
          version_(g_.size(), 0){
        incremental_path_search_internal::check_endpoint(
            vertex_count, start, "Focused D* start is out of range"
        );
        incremental_path_search_internal::check_endpoint(
            vertex_count, goal, "Focused D* goal is out of range"
        );
        if(!heuristic_){
            throw std::invalid_argument("Focused D* requires a heuristic");
        }
        if(!(Cost{} < infinity_)){
            throw std::invalid_argument("Focused D* infinity must be positive");
        }
        rhs_[static_cast<std::size_t>(goal_)] = Cost{};
        push(goal_);
    }

    void set_edge(int from, int to, Cost cost){
        graph_.set_edge(from, to, cost);
        update_vertex(from);
    }

    void remove_edge(int from, int to){
        graph_.remove_edge(from, to);
        update_vertex(from);
    }

    void move_start(int start){
        incremental_path_search_internal::check_endpoint(
            graph_.vertex_count(), start, "Focused D* start is out of range"
        );
        start_ = start;
        rebuild_queue();
    }

    IncrementalPathResult<Cost> replan(){
        compute_shortest_path();
        IncrementalPathResult<Cost> result;
        result.expanded = expanded_last_;
        if(!(g_[static_cast<std::size_t>(start_)] < infinity_)) return result;
        result.cost = g_[static_cast<std::size_t>(start_)];
        result.path.push_back(start_);
        int current = start_;
        for(int guard = 0; current != goal_ && guard < graph_.vertex_count();
            ++guard){
            Cost best = infinity_;
            int successor = -1;
            for(const auto& [candidate, cost] : graph_.outgoing(current)){
                const Cost value =
                    add(cost, g_[static_cast<std::size_t>(candidate)]);
                if(value < best){
                    best = value;
                    successor = candidate;
                }
            }
            if(successor < 0) return IncrementalPathResult<Cost>{};
            current = successor;
            result.path.push_back(current);
        }
        if(current != goal_) return IncrementalPathResult<Cost>{};
        return result;
    }
};

template<class Cost>
class DStarLite{
    using Graph = incremental_path_search_internal::DynamicDirectedGraph<Cost>;
    using Key = incremental_path_search_internal::Key<Cost>;
    using Entry = incremental_path_search_internal::QueueEntry<Cost>;
    using Queue = std::priority_queue<
        Entry,
        std::vector<Entry>,
        incremental_path_search_internal::QueueCompare<Cost>
    >;

    Graph graph_;
    int start_;
    int goal_;
    int last_start_;
    std::function<Cost(int, int)> heuristic_;
    Cost infinity_;
    Cost key_modifier_{};
    std::vector<Cost> g_;
    std::vector<Cost> rhs_;
    std::vector<std::size_t> version_;
    Queue queue_;
    std::size_t order_ = 0;
    std::size_t expanded_last_ = 0;

    Cost add(Cost first, Cost second) const{
        return incremental_path_search_internal::safe_add(
            first, second, infinity_
        );
    }

    bool inconsistent(int vertex) const{
        return g_[static_cast<std::size_t>(vertex)]
            != rhs_[static_cast<std::size_t>(vertex)];
    }

    Key calculate_key(int vertex) const{
        const Cost minimum = std::min(
            g_[static_cast<std::size_t>(vertex)],
            rhs_[static_cast<std::size_t>(vertex)]
        );
        return Key{
            add(add(minimum, heuristic_(start_, vertex)), key_modifier_),
            minimum
        };
    }

    void push(int vertex){
        const std::size_t index = static_cast<std::size_t>(vertex);
        ++version_[index];
        if(inconsistent(vertex)){
            queue_.push(Entry{
                calculate_key(vertex), version_[index], order_++, vertex
            });
        }
    }

    void update_vertex(int vertex){
        if(vertex != goal_){
            Cost best = infinity_;
            for(const auto& [successor, cost] : graph_.outgoing(vertex)){
                best = std::min(
                    best,
                    add(cost, g_[static_cast<std::size_t>(successor)])
                );
            }
            rhs_[static_cast<std::size_t>(vertex)] = best;
        }
        push(vertex);
    }

    void discard_stale(){
        while(!queue_.empty()){
            const Entry& entry = queue_.top();
            if(entry.version
                   == version_[static_cast<std::size_t>(entry.vertex)]
               && inconsistent(entry.vertex)){
                break;
            }
            queue_.pop();
        }
    }

    void compute_shortest_path(){
        expanded_last_ = 0;
        while(true){
            discard_stale();
            const Key start_key = calculate_key(start_);
            if(queue_.empty()
               || (!incremental_path_search_internal::key_less(
                       queue_.top().key, start_key
                   )
                   && !inconsistent(start_))){
                break;
            }
            Entry entry = queue_.top();
            queue_.pop();
            const Key current_key = calculate_key(entry.vertex);
            if(incremental_path_search_internal::key_less(
                   entry.key, current_key
               )){
                push(entry.vertex);
                continue;
            }
            const int vertex = entry.vertex;
            ++expanded_last_;
            const std::size_t index = static_cast<std::size_t>(vertex);
            if(g_[index] > rhs_[index]){
                g_[index] = rhs_[index];
                ++version_[index];
                for(const auto& [predecessor, cost] : graph_.incoming(vertex)){
                    static_cast<void>(cost);
                    update_vertex(predecessor);
                }
            }else{
                g_[index] = infinity_;
                update_vertex(vertex);
                for(const auto& [predecessor, cost] : graph_.incoming(vertex)){
                    static_cast<void>(cost);
                    update_vertex(predecessor);
                }
            }
        }
    }

public:
    DStarLite(
        int vertex_count,
        int start,
        int goal,
        std::function<Cost(int, int)> heuristic,
        Cost infinity = incremental_path_search_internal::default_infinity<Cost>()
    )
        : graph_(vertex_count),
          start_(start),
          goal_(goal),
          last_start_(start),
          heuristic_(std::move(heuristic)),
          infinity_(infinity),
          g_(static_cast<std::size_t>(vertex_count), infinity),
          rhs_(g_),
          version_(g_.size(), 0){
        incremental_path_search_internal::check_endpoint(
            vertex_count, start, "D* Lite start is out of range"
        );
        incremental_path_search_internal::check_endpoint(
            vertex_count, goal, "D* Lite goal is out of range"
        );
        if(!heuristic_){
            throw std::invalid_argument("D* Lite requires a heuristic");
        }
        if(!(Cost{} < infinity_)){
            throw std::invalid_argument("D* Lite infinity must be positive");
        }
        rhs_[static_cast<std::size_t>(goal_)] = Cost{};
        push(goal_);
    }

    void set_edge(int from, int to, Cost cost){
        graph_.set_edge(from, to, cost);
        update_vertex(from);
    }

    void remove_edge(int from, int to){
        graph_.remove_edge(from, to);
        update_vertex(from);
    }

    void move_start(int start){
        incremental_path_search_internal::check_endpoint(
            graph_.vertex_count(), start, "D* Lite start is out of range"
        );
        key_modifier_ = add(
            key_modifier_, heuristic_(last_start_, start)
        );
        start_ = start;
        last_start_ = start;
    }

    IncrementalPathResult<Cost> replan(){
        compute_shortest_path();
        IncrementalPathResult<Cost> result;
        result.expanded = expanded_last_;
        if(!(g_[static_cast<std::size_t>(start_)] < infinity_)) return result;
        result.cost = g_[static_cast<std::size_t>(start_)];
        result.path.push_back(start_);
        int current = start_;
        for(int guard = 0; current != goal_ && guard < graph_.vertex_count();
            ++guard){
            Cost best = infinity_;
            int successor = -1;
            for(const auto& [candidate, cost] : graph_.outgoing(current)){
                const Cost value =
                    add(cost, g_[static_cast<std::size_t>(candidate)]);
                if(value < best){
                    best = value;
                    successor = candidate;
                }
            }
            if(successor < 0) return IncrementalPathResult<Cost>{};
            current = successor;
            result.path.push_back(current);
        }
        if(current != goal_) return IncrementalPathResult<Cost>{};
        return result;
    }
};

struct FieldDStarResult{
    std::optional<double> discrete_cost;
    std::optional<double> interpolated_cost;
    std::vector<GridPoint> grid_path;
    std::vector<GridPoint> field_path;
    std::size_t expanded = 0;

    explicit operator bool() const noexcept{
        return discrete_cost.has_value();
    }
};

class FieldDStar{
    GridMap grid_;
    GridPoint start_;
    GridPoint goal_;
    DStarLite<double> planner_;

    int vertex(GridPoint point) const noexcept{
        return point.row * grid_.column_count() + point.column;
    }

    GridPoint point(int vertex_index) const noexcept{
        return GridPoint{
            vertex_index / grid_.column_count(),
            vertex_index % grid_.column_count()
        };
    }

    bool legal_arc(GridPoint from, GridPoint to) const{
        if(!grid_.is_passable(from) || !grid_.is_passable(to)) return false;
        const int dr = std::abs(from.row - to.row);
        const int dc = std::abs(from.column - to.column);
        if(std::max(dr, dc) != 1) return false;
        if(dr == 1 && dc == 1){
            return grid_.is_passable(GridPoint{from.row, to.column})
                && grid_.is_passable(GridPoint{to.row, from.column});
        }
        return true;
    }

    double arc_cost(GridPoint from, GridPoint to) const noexcept{
        return std::hypot(
            static_cast<double>(from.row - to.row),
            static_cast<double>(from.column - to.column)
        );
    }

    void refresh_arc(GridPoint from, GridPoint to){
        if(!grid_.contains(from) || !grid_.contains(to)) return;
        if(legal_arc(from, to)){
            planner_.set_edge(vertex(from), vertex(to), arc_cost(from, to));
        }else{
            planner_.remove_edge(vertex(from), vertex(to));
        }
    }

    void refresh_neighborhood(GridPoint changed){
        for(int row = changed.row - 1; row <= changed.row + 1; ++row){
            for(int column = changed.column - 1;
                column <= changed.column + 1;
                ++column){
                const GridPoint from{row, column};
                if(!grid_.contains(from)) continue;
                for(const GridPoint direction :
                    grid_path_search_internal::directions){
                    refresh_arc(
                        from,
                        GridPoint{
                            from.row + direction.row,
                            from.column + direction.column
                        }
                    );
                }
            }
        }
    }

    static std::function<double(int, int)> make_heuristic(int columns){
        return [columns](int first, int second){
            const GridPoint a{first / columns, first % columns};
            const GridPoint b{second / columns, second % columns};
            return std::hypot(
                static_cast<double>(a.row - b.row),
                static_cast<double>(a.column - b.column)
            );
        };
    }

public:
    FieldDStar(GridMap grid, GridPoint start, GridPoint goal)
        : grid_(std::move(grid)),
          start_(start),
          goal_(goal),
          planner_(
              grid_.row_count() * grid_.column_count(),
              vertex(start),
              vertex(goal),
              make_heuristic(grid_.column_count())
          ){
        if(grid_.row_count() == 0 || grid_.column_count() == 0){
            throw std::invalid_argument("Field D* requires a non-empty grid");
        }
        if(!grid_.contains(start_) || !grid_.contains(goal_)){
            throw std::out_of_range("Field D* endpoint is outside the grid");
        }
        for(int row = 0; row < grid_.row_count(); ++row){
            for(int column = 0; column < grid_.column_count(); ++column){
                const GridPoint from{row, column};
                for(const GridPoint direction :
                    grid_path_search_internal::directions){
                    const GridPoint to{
                        row + direction.row,
                        column + direction.column
                    };
                    if(legal_arc(from, to)){
                        planner_.set_edge(
                            vertex(from), vertex(to), arc_cost(from, to)
                        );
                    }
                }
            }
        }
    }

    const GridMap& grid() const noexcept{ return grid_; }

    void set_passable(GridPoint changed, bool passable){
        grid_.set_passable(changed, passable);
        refresh_neighborhood(changed);
    }

    void move_start(GridPoint start){
        if(!grid_.contains(start)){
            throw std::out_of_range("Field D* start is outside the grid");
        }
        start_ = start;
        planner_.move_start(vertex(start_));
    }

    FieldDStarResult replan(){
        const IncrementalPathResult<double> discrete = planner_.replan();
        FieldDStarResult result;
        result.expanded = discrete.expanded;
        if(!discrete) return result;
        result.discrete_cost = discrete.cost;
        result.grid_path.reserve(discrete.path.size());
        for(const int vertex_index : discrete.path){
            result.grid_path.push_back(point(vertex_index));
        }
        result.field_path.push_back(result.grid_path.front());
        std::size_t anchor = 0;
        double interpolated_cost = 0.0;
        while(anchor + 1 < result.grid_path.size()){
            std::size_t next = result.grid_path.size() - 1;
            while(next > anchor + 1
                  && !grid_line_of_sight(
                      grid_, result.grid_path[anchor], result.grid_path[next]
                  )){
                --next;
            }
            interpolated_cost += grid_path_search_internal::segment_cost(
                result.grid_path[anchor], result.grid_path[next]
            );
            result.field_path.push_back(result.grid_path[next]);
            anchor = next;
        }
        result.interpolated_cost = interpolated_cost;
        return result;
    }
};

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_INCREMENTAL_PATH_SEARCH_HPP_INCLUDED
