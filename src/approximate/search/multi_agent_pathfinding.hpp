#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_MULTI_AGENT_PATHFINDING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_MULTI_AGENT_PATHFINDING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct GridLocation{
    int row = 0;
    int column = 0;

    friend bool operator==(const GridLocation&, const GridLocation&) = default;
    friend bool operator<(const GridLocation& lhs, const GridLocation& rhs){
        return std::tie(lhs.row, lhs.column)
             < std::tie(rhs.row, rhs.column);
    }
};

struct GridLocationHash{
    [[nodiscard]] std::size_t operator()(GridLocation location) const noexcept{
        std::uint64_t value =
            static_cast<std::uint32_t>(location.row);
        value = (value << 32U)
              | static_cast<std::uint32_t>(location.column);
        value += 0x9e3779b97f4a7c15ULL;
        value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
        return static_cast<std::size_t>(value ^ (value >> 31U));
    }
};

class MultiAgentGridMap{
public:
    MultiAgentGridMap(
        int rows,
        int columns,
        const std::vector<GridLocation>& blocked = {}
    )
        : rows_(rows), columns_(columns){
        if(rows < 0 || columns < 0){
            throw std::invalid_argument("grid dimensions must be non-negative");
        }
        const std::size_t cell_count =
            static_cast<std::size_t>(rows)
                * static_cast<std::size_t>(columns);
        cells_.assign(cell_count, false);
        for(const GridLocation location : blocked){
            set_blocked(location, true);
        }
    }

    [[nodiscard]] int rows() const noexcept{ return rows_; }
    [[nodiscard]] int columns() const noexcept{ return columns_; }
    [[nodiscard]] int size() const noexcept{ return rows_ * columns_; }

    [[nodiscard]] bool contains(GridLocation location) const noexcept{
        return 0 <= location.row && location.row < rows_
            && 0 <= location.column && location.column < columns_;
    }

    [[nodiscard]] bool passable(GridLocation location) const noexcept{
        return contains(location) && !cells_[index_unchecked(location)];
    }

    void set_blocked(GridLocation location, bool blocked = true){
        validate(location);
        cells_[index_unchecked(location)] = blocked;
    }

    [[nodiscard]] int index(GridLocation location) const{
        validate(location);
        return static_cast<int>(index_unchecked(location));
    }

    [[nodiscard]] GridLocation location(int index) const{
        if(index < 0 || size() <= index){
            throw std::out_of_range("grid index is out of range");
        }
        return GridLocation{index / columns_, index % columns_};
    }

    [[nodiscard]] std::vector<GridLocation> neighbors(
        GridLocation location
    ) const{
        validate(location);
        static constexpr int row_delta[4] = {-1, 0, 1, 0};
        static constexpr int column_delta[4] = {0, 1, 0, -1};
        std::vector<GridLocation> result;
        result.reserve(4);
        for(int direction = 0; direction < 4; ++direction){
            const GridLocation next{
                location.row + row_delta[direction],
                location.column + column_delta[direction]
            };
            if(passable(next)) result.push_back(next);
        }
        return result;
    }

private:
    [[nodiscard]] std::size_t index_unchecked(
        GridLocation location
    ) const noexcept{
        return static_cast<std::size_t>(location.row)
             * static_cast<std::size_t>(columns_)
             + static_cast<std::size_t>(location.column);
    }

    void validate(GridLocation location) const{
        if(!contains(location)){
            throw std::out_of_range("grid location is out of range");
        }
    }

    int rows_;
    int columns_;
    std::vector<bool> cells_;
};

struct TimedGridLocation{
    GridLocation position;
    int time = 0;
};

struct TimedGridPathResult{
    std::vector<TimedGridLocation> path;
    int cost = -1;
    std::size_t expanded = 0;

    [[nodiscard]] explicit operator bool() const noexcept{
        return cost >= 0;
    }
};

namespace multi_agent_pathfinding_internal{

struct TimedVertex{
    GridLocation position;
    int time;
    friend bool operator==(const TimedVertex&, const TimedVertex&) = default;
};

struct TimedVertexHash{
    std::size_t operator()(const TimedVertex& value) const noexcept{
        std::size_t seed = GridLocationHash{}(value.position);
        seed ^= static_cast<std::size_t>(
            static_cast<std::uint32_t>(value.time)
        ) + 0x9e3779b9U + (seed << 6U) + (seed >> 2U);
        return seed;
    }
};

struct TimedEdge{
    GridLocation from;
    GridLocation to;
    int time;
    friend bool operator==(const TimedEdge&, const TimedEdge&) = default;
};

struct TimedEdgeHash{
    std::size_t operator()(const TimedEdge& value) const noexcept{
        std::size_t seed = GridLocationHash{}(value.from);
        seed ^= GridLocationHash{}(value.to)
              + 0x9e3779b9U + (seed << 6U) + (seed >> 2U);
        seed ^= static_cast<std::size_t>(
            static_cast<std::uint32_t>(value.time)
        ) + 0x9e3779b9U + (seed << 6U) + (seed >> 2U);
        return seed;
    }
};

inline int manhattan(GridLocation lhs, GridLocation rhs) noexcept{
    return std::abs(lhs.row - rhs.row)
         + std::abs(lhs.column - rhs.column);
}

inline void validate_endpoint(
    const MultiAgentGridMap& map,
    GridLocation location,
    const char* message
){
    if(!map.passable(location)) throw std::invalid_argument(message);
}

} // namespace multi_agent_pathfinding_internal

class ReservationTable{
public:
    void reserve_vertex(GridLocation position, int time){
        validate_time(time);
        vertices_[position].insert(time);
        maximum_time_ = std::max(maximum_time_, time);
    }

    void reserve_edge(GridLocation from, GridLocation to, int time){
        validate_time(time);
        edges_.insert(
            multi_agent_pathfinding_internal::TimedEdge{from, to, time}
        );
        maximum_time_ = std::max(maximum_time_, time + 1);
    }

    void reserve_path(
        const std::vector<TimedGridLocation>& path,
        int hold_until = -1
    ){
        if(path.empty()) return;
        for(std::size_t i = 0; i < path.size(); ++i){
            reserve_vertex(path[i].position, path[i].time);
            if(i != 0){
                if(path[i].time != path[i - 1].time + 1){
                    throw std::invalid_argument(
                        "reserved path times must be consecutive"
                    );
                }
                reserve_edge(
                    path[i - 1].position, path[i].position, path[i - 1].time
                );
            }
        }
        if(hold_until >= 0){
            if(hold_until < path.back().time){
                throw std::invalid_argument(
                    "hold_until precedes the path arrival"
                );
            }
            for(int time = path.back().time + 1; time <= hold_until; ++time){
                reserve_vertex(path.back().position, time);
                reserve_edge(path.back().position, path.back().position, time - 1);
            }
        }
    }

    [[nodiscard]] bool is_vertex_reserved(
        GridLocation position,
        int time
    ) const{
        if(time < 0) return true;
        const auto iterator = vertices_.find(position);
        return iterator != vertices_.end()
            && iterator->second.contains(time);
    }

    [[nodiscard]] bool is_edge_reserved(
        GridLocation from,
        GridLocation to,
        int time
    ) const{
        if(time < 0) return true;
        return edges_.contains(
            multi_agent_pathfinding_internal::TimedEdge{from, to, time}
        );
    }

    [[nodiscard]] std::vector<std::pair<int, int>> safe_intervals(
        GridLocation position,
        int first_time,
        int last_time
    ) const{
        if(first_time < 0 || last_time < first_time){
            throw std::invalid_argument("invalid safe-interval horizon");
        }
        std::vector<std::pair<int, int>> result;
        int begin = first_time;
        const auto iterator = vertices_.find(position);
        if(iterator != vertices_.end()){
            auto reserved = iterator->second.lower_bound(first_time);
            while(reserved != iterator->second.end() && *reserved <= last_time){
                if(begin < *reserved) result.emplace_back(begin, *reserved - 1);
                begin = *reserved + 1;
                ++reserved;
            }
        }
        if(begin <= last_time) result.emplace_back(begin, last_time);
        return result;
    }

    [[nodiscard]] int maximum_time() const noexcept{
        return maximum_time_;
    }

    [[nodiscard]] std::size_t reserved_vertex_count() const noexcept{
        std::size_t count = 0;
        for(const auto& [position, times] : vertices_){
            static_cast<void>(position);
            count += times.size();
        }
        return count;
    }

    [[nodiscard]] std::size_t reserved_edge_count() const noexcept{
        return edges_.size();
    }

private:
    static void validate_time(int time){
        if(time < 0){
            throw std::invalid_argument("reservation time must be non-negative");
        }
    }

    std::unordered_map<
        GridLocation, std::set<int>, GridLocationHash
    > vertices_;
    std::unordered_set<
        multi_agent_pathfinding_internal::TimedEdge,
        multi_agent_pathfinding_internal::TimedEdgeHash
    > edges_;
    int maximum_time_ = -1;
};

inline TimedGridPathResult safe_interval_path_planning(
    const MultiAgentGridMap& map,
    GridLocation start,
    GridLocation goal,
    const ReservationTable& reservations,
    int start_time,
    int max_time,
    int goal_hold_until = -1
){
    using namespace multi_agent_pathfinding_internal;
    validate_endpoint(map, start, "SIPP start must be a passable grid cell");
    validate_endpoint(map, goal, "SIPP goal must be a passable grid cell");
    if(start_time < 0 || max_time < start_time
       || (goal_hold_until >= 0 && max_time < goal_hold_until)){
        throw std::invalid_argument("invalid SIPP time horizon");
    }
    TimedGridPathResult result;
    if(reservations.is_vertex_reserved(start, start_time)) return result;

    struct StateKey{
        GridLocation position;
        int interval_begin;
        int interval_end;
        bool operator==(const StateKey&) const = default;
    };
    struct StateHash{
        std::size_t operator()(const StateKey& state) const noexcept{
            std::size_t seed = GridLocationHash{}(state.position);
            seed ^= static_cast<std::size_t>(state.interval_begin)
                  + 0x9e3779b9U + (seed << 6U) + (seed >> 2U);
            seed ^= static_cast<std::size_t>(state.interval_end)
                  + 0x9e3779b9U + (seed << 6U) + (seed >> 2U);
            return seed;
        }
    };
    struct Node{
        StateKey state;
        int arrival;
        int parent;
        int departure;
        bool closed;
    };
    struct Entry{
        int estimate;
        int arrival;
        int node;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            return std::tie(lhs.estimate, lhs.arrival)
                 > std::tie(rhs.estimate, rhs.arrival);
        }
    };

    const auto start_intervals =
        reservations.safe_intervals(start, start_time, max_time);
    std::optional<std::pair<int, int>> start_interval;
    for(const auto interval : start_intervals){
        if(interval.first <= start_time && start_time <= interval.second){
            start_interval = interval;
            break;
        }
    }
    if(!start_interval) return result;

    std::vector<Node> nodes;
    std::unordered_map<StateKey, int, StateHash> index;
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    nodes.push_back(Node{
        StateKey{start, start_interval->first, start_interval->second},
        start_time, -1, start_time, false
    });
    index.emplace(nodes.front().state, 0);
    queue.push(Entry{start_time + manhattan(start, goal), start_time, 0});
    int goal_node = -1;
    while(!queue.empty()){
        const Entry entry = queue.top();
        queue.pop();
        Node& current = nodes[static_cast<std::size_t>(entry.node)];
        if(current.closed || current.arrival != entry.arrival) continue;
        current.closed = true;
        const GridLocation current_position = current.state.position;
        const int current_arrival = current.arrival;
        const int current_interval_end = current.state.interval_end;
        ++result.expanded;
        if(current_position == goal
           && (goal_hold_until < 0
               || current_interval_end >= goal_hold_until)){
            goal_node = entry.node;
            break;
        }
        for(const GridLocation next : map.neighbors(current_position)){
            const auto intervals =
                reservations.safe_intervals(next, start_time, max_time);
            for(const auto [interval_begin, interval_end] : intervals){
                int arrival = std::max(current_arrival + 1, interval_begin);
                while(arrival <= interval_end
                      && arrival - 1 <= current_interval_end
                      && reservations.is_edge_reserved(
                          next, current_position, arrival - 1
                      )){
                    ++arrival;
                }
                if(arrival > interval_end
                   || arrival - 1 > current_interval_end){
                    continue;
                }
                const StateKey next_state{
                    next, interval_begin, interval_end
                };
                auto iterator = index.find(next_state);
                int next_node;
                if(iterator == index.end()){
                    next_node = static_cast<int>(nodes.size());
                    nodes.push_back(Node{
                        next_state, arrival, entry.node, arrival - 1, false
                    });
                    index.emplace(nodes.back().state, next_node);
                }else{
                    next_node = iterator->second;
                    Node& old = nodes[static_cast<std::size_t>(next_node)];
                    if(old.arrival <= arrival) continue;
                    old.arrival = arrival;
                    old.parent = entry.node;
                    old.departure = arrival - 1;
                    old.closed = false;
                }
                queue.push(Entry{
                    arrival + manhattan(next, goal), arrival, next_node
                });
            }
        }
    }
    if(goal_node < 0) return result;

    std::vector<int> chain;
    for(int node = goal_node; node >= 0;
        node = nodes[static_cast<std::size_t>(node)].parent){
        chain.push_back(node);
    }
    std::reverse(chain.begin(), chain.end());
    result.path.push_back(TimedGridLocation{start, start_time});
    for(std::size_t i = 1; i < chain.size(); ++i){
        const Node& previous =
            nodes[static_cast<std::size_t>(chain[i - 1])];
        const Node& current =
            nodes[static_cast<std::size_t>(chain[i])];
        for(int time = previous.arrival + 1;
            time <= current.departure;
            ++time){
            result.path.push_back(
                TimedGridLocation{previous.state.position, time}
            );
        }
        result.path.push_back(
            TimedGridLocation{current.state.position, current.arrival}
        );
    }
    result.cost =
        nodes[static_cast<std::size_t>(goal_node)].arrival - start_time;
    return result;
}

inline TimedGridPathResult sipp_search(
    const MultiAgentGridMap& map,
    GridLocation start,
    GridLocation goal,
    const ReservationTable& reservations,
    int start_time,
    int max_time,
    int goal_hold_until = -1
){
    return safe_interval_path_planning(
        map, start, goal, reservations,
        start_time, max_time, goal_hold_until
    );
}

namespace multi_agent_pathfinding_internal{

inline std::vector<GridLocation> bfs_inside_cluster(
    const MultiAgentGridMap& map,
    GridLocation source,
    GridLocation target,
    int cluster_size
){
    const int cluster_row = source.row / cluster_size;
    const int cluster_column = source.column / cluster_size;
    if(target.row / cluster_size != cluster_row
       || target.column / cluster_size != cluster_column){
        return {};
    }
    const int row_begin = cluster_row * cluster_size;
    const int row_end = std::min(map.rows(), row_begin + cluster_size);
    const int column_begin = cluster_column * cluster_size;
    const int column_end = std::min(map.columns(), column_begin + cluster_size);
    const int width = column_end - column_begin;
    const auto local_index = [&](GridLocation location){
        return (location.row - row_begin) * width
             + location.column - column_begin;
    };
    const auto local_location = [&](int index){
        return GridLocation{
            row_begin + index / width,
            column_begin + index % width
        };
    };
    std::vector<int> parent(
        static_cast<std::size_t>((row_end - row_begin) * width), -1
    );
    std::queue<GridLocation> queue;
    parent[static_cast<std::size_t>(local_index(source))] = local_index(source);
    queue.push(source);
    while(!queue.empty()){
        const GridLocation current = queue.front();
        queue.pop();
        if(current == target) break;
        for(const GridLocation next : map.neighbors(current)){
            if(next.row < row_begin || row_end <= next.row
               || next.column < column_begin || column_end <= next.column){
                continue;
            }
            const int next_index = local_index(next);
            if(parent[static_cast<std::size_t>(next_index)] >= 0) continue;
            parent[static_cast<std::size_t>(next_index)] =
                local_index(current);
            queue.push(next);
        }
    }
    const int target_index = local_index(target);
    if(parent[static_cast<std::size_t>(target_index)] < 0) return {};
    std::vector<GridLocation> path;
    for(int index = target_index;;){
        path.push_back(local_location(index));
        const int previous = parent[static_cast<std::size_t>(index)];
        if(previous == index) break;
        index = previous;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace multi_agent_pathfinding_internal

inline TimedGridPathResult hpa_star_search(
    const MultiAgentGridMap& map,
    GridLocation start,
    GridLocation goal,
    int cluster_size = 8
){
    using namespace multi_agent_pathfinding_internal;
    validate_endpoint(map, start, "HPA* start must be a passable grid cell");
    validate_endpoint(map, goal, "HPA* goal must be a passable grid cell");
    if(cluster_size <= 0){
        throw std::invalid_argument("HPA* cluster_size must be positive");
    }
    std::vector<GridLocation> abstract_locations;
    std::unordered_map<GridLocation, int, GridLocationHash> abstract_index;
    auto add_abstract = [&](GridLocation location){
        const auto [iterator, inserted] = abstract_index.emplace(
            location, static_cast<int>(abstract_locations.size())
        );
        if(inserted) abstract_locations.push_back(location);
        return iterator->second;
    };
    const int start_node = add_abstract(start);
    const int goal_node = add_abstract(goal);
    for(int row = 0; row < map.rows(); ++row){
        for(int column = 0; column < map.columns(); ++column){
            const GridLocation from{row, column};
            if(!map.passable(from)) continue;
            for(const GridLocation to : map.neighbors(from)){
                if(from.row / cluster_size != to.row / cluster_size
                   || from.column / cluster_size != to.column / cluster_size){
                    add_abstract(from);
                    add_abstract(to);
                }
            }
        }
    }
    struct AbstractEdge{
        int to;
        int cost;
    };
    std::vector<std::vector<AbstractEdge>> graph(abstract_locations.size());
    for(std::size_t from = 0; from < abstract_locations.size(); ++from){
        const GridLocation source = abstract_locations[from];
        const int cluster_row = source.row / cluster_size;
        const int cluster_column = source.column / cluster_size;
        const int row_begin = cluster_row * cluster_size;
        const int row_end = std::min(map.rows(), row_begin + cluster_size);
        const int column_begin = cluster_column * cluster_size;
        const int column_end = std::min(
            map.columns(), column_begin + cluster_size
        );
        const int width = column_end - column_begin;
        const auto local_index = [&](GridLocation location){
            return (location.row - row_begin) * width
                 + location.column - column_begin;
        };
        std::vector<int> local_distance(
            static_cast<std::size_t>((row_end - row_begin) * width), -1
        );
        std::queue<GridLocation> local_queue;
        local_distance[static_cast<std::size_t>(local_index(source))] = 0;
        local_queue.push(source);
        while(!local_queue.empty()){
            const GridLocation current = local_queue.front();
            local_queue.pop();
            for(const GridLocation next : map.neighbors(current)){
                if(next.row < row_begin || row_end <= next.row
                   || next.column < column_begin
                   || column_end <= next.column){
                    continue;
                }
                int& distance = local_distance[
                    static_cast<std::size_t>(local_index(next))
                ];
                if(distance >= 0) continue;
                distance = local_distance[
                    static_cast<std::size_t>(local_index(current))
                ] + 1;
                local_queue.push(next);
            }
        }
        for(std::size_t to = 0; to < abstract_locations.size(); ++to){
            if(from == to) continue;
            const GridLocation lhs = abstract_locations[from];
            const GridLocation rhs = abstract_locations[to];
            if(lhs.row / cluster_size == rhs.row / cluster_size
               && lhs.column / cluster_size == rhs.column / cluster_size){
                const int cost = local_distance[
                    static_cast<std::size_t>(local_index(rhs))
                ];
                if(cost >= 0){
                    graph[from].push_back(AbstractEdge{
                        static_cast<int>(to), cost
                    });
                }
            }else if(manhattan(lhs, rhs) == 1){
                graph[from].push_back(
                    AbstractEdge{static_cast<int>(to), 1}
                );
            }
        }
    }
    const int infinity = std::numeric_limits<int>::max() / 4;
    std::vector<int> distance(abstract_locations.size(), infinity);
    std::vector<int> parent(abstract_locations.size(), -1);
    std::vector<int> parent_edge(abstract_locations.size(), -1);
    using Entry = std::tuple<int, int, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    distance[static_cast<std::size_t>(start_node)] = 0;
    queue.emplace(manhattan(start, goal), 0, start_node);
    TimedGridPathResult result;
    while(!queue.empty()){
        const auto [estimate, current_distance, vertex] = queue.top();
        static_cast<void>(estimate);
        queue.pop();
        if(current_distance != distance[static_cast<std::size_t>(vertex)]){
            continue;
        }
        ++result.expanded;
        if(vertex == goal_node) break;
        for(int edge_index = 0;
            edge_index < static_cast<int>(
                graph[static_cast<std::size_t>(vertex)].size()
            );
            ++edge_index){
            const AbstractEdge& edge =
                graph[static_cast<std::size_t>(vertex)]
                     [static_cast<std::size_t>(edge_index)];
            const int candidate = current_distance + edge.cost;
            if(candidate < distance[static_cast<std::size_t>(edge.to)]){
                distance[static_cast<std::size_t>(edge.to)] = candidate;
                parent[static_cast<std::size_t>(edge.to)] = vertex;
                parent_edge[static_cast<std::size_t>(edge.to)] = edge_index;
                queue.emplace(
                    candidate + manhattan(
                        abstract_locations[static_cast<std::size_t>(edge.to)],
                        goal
                    ),
                    candidate, edge.to
                );
            }
        }
    }
    if(distance[static_cast<std::size_t>(goal_node)] == infinity) return result;
    std::vector<std::pair<int, int>> segments;
    for(int vertex = goal_node; vertex != start_node;){
        const int previous = parent[static_cast<std::size_t>(vertex)];
        if(previous < 0) return TimedGridPathResult{};
        segments.emplace_back(
            previous, parent_edge[static_cast<std::size_t>(vertex)]
        );
        vertex = previous;
    }
    std::reverse(segments.begin(), segments.end());
    std::vector<GridLocation> path{start};
    for(const auto [vertex, edge_index] : segments){
        const AbstractEdge& edge =
            graph[static_cast<std::size_t>(vertex)]
                 [static_cast<std::size_t>(edge_index)];
        const GridLocation from =
            abstract_locations[static_cast<std::size_t>(vertex)];
        const GridLocation to =
            abstract_locations[static_cast<std::size_t>(edge.to)];
        const std::vector<GridLocation> segment = edge.cost == 1
            && manhattan(from, to) == 1
            ? std::vector<GridLocation>{from, to}
            : bfs_inside_cluster(map, from, to, cluster_size);
        if(segment.empty()) return TimedGridPathResult{};
        path.insert(path.end(), std::next(segment.begin()), segment.end());
    }
    result.path.reserve(path.size());
    for(int time = 0; time < static_cast<int>(path.size()); ++time){
        result.path.push_back(TimedGridLocation{
            path[static_cast<std::size_t>(time)], time
        });
    }
    result.cost = static_cast<int>(path.size()) - 1;
    return result;
}

struct MultiAgentPathResult{
    std::vector<std::vector<GridLocation>> paths;
    int sum_of_costs = -1;
    int makespan = -1;
    std::size_t expanded = 0;
    bool cutoff = false;

    [[nodiscard]] explicit operator bool() const noexcept{
        return sum_of_costs >= 0;
    }
};

namespace multi_agent_pathfinding_internal{

inline void validate_agents(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals
){
    if(starts.size() != goals.size()){
        throw std::invalid_argument(
            "multi-agent starts and goals must have equal sizes"
        );
    }
    for(std::size_t agent = 0; agent < starts.size(); ++agent){
        validate_endpoint(
            map, starts[agent], "agent start must be a passable grid cell"
        );
        validate_endpoint(
            map, goals[agent], "agent goal must be a passable grid cell"
        );
    }
    std::unordered_set<GridLocation, GridLocationHash> distinct_starts;
    for(const GridLocation start : starts){
        if(!distinct_starts.insert(start).second){
            throw std::invalid_argument("agent starts must be distinct");
        }
    }
}

inline std::vector<GridLocation> untimed(
    const std::vector<TimedGridLocation>& path
){
    std::vector<GridLocation> result;
    result.reserve(path.size());
    for(const auto& state : path) result.push_back(state.position);
    return result;
}

inline void finalize_result(MultiAgentPathResult& result){
    result.sum_of_costs = 0;
    result.makespan = 0;
    for(const auto& path : result.paths){
        const int cost = static_cast<int>(path.size()) - 1;
        result.sum_of_costs += cost;
        result.makespan = std::max(result.makespan, cost);
    }
}

inline GridLocation at_time(
    const std::vector<GridLocation>& path,
    int time
){
    return path[
        std::min<std::size_t>(
            static_cast<std::size_t>(time), path.size() - 1
        )
    ];
}

struct Constraint{
    int agent;
    int time;
    GridLocation from;
    GridLocation to;
    bool edge;
};

struct ConstraintLookup{
    std::unordered_set<TimedVertex, TimedVertexHash> vertices;
    std::unordered_set<TimedEdge, TimedEdgeHash> edges;
    int latest = 0;
};

inline ConstraintLookup make_constraint_lookup(
    int agent,
    const std::vector<Constraint>& constraints
){
    ConstraintLookup result;
    for(const Constraint& constraint : constraints){
        if(constraint.agent != agent) continue;
        result.latest = std::max(result.latest, constraint.time + constraint.edge);
        if(constraint.edge){
            result.edges.insert(TimedEdge{
                constraint.from, constraint.to, constraint.time
            });
        }else{
            result.vertices.insert(TimedVertex{
                constraint.to, constraint.time
            });
        }
    }
    return result;
}

struct LowLevelResult{
    std::vector<GridLocation> path;
    std::size_t expanded = 0;
};

inline LowLevelResult constrained_a_star(
    const MultiAgentGridMap& map,
    GridLocation start,
    GridLocation goal,
    int agent,
    const std::vector<Constraint>& constraints,
    int max_time
){
    const ConstraintLookup forbidden =
        make_constraint_lookup(agent, constraints);
    LowLevelResult result;
    if(forbidden.vertices.contains(TimedVertex{start, 0})) return result;
    struct Node{
        GridLocation position;
        int time;
        int parent;
    };
    struct Entry{
        int estimate;
        int time;
        int node;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            return std::tie(lhs.estimate, lhs.time)
                 > std::tie(rhs.estimate, rhs.time);
        }
    };
    std::vector<Node> nodes{{start, 0, -1}};
    std::unordered_map<TimedVertex, int, TimedVertexHash> index;
    index.emplace(TimedVertex{start, 0}, 0);
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    queue.push(Entry{manhattan(start, goal), 0, 0});
    int goal_node = -1;
    while(!queue.empty()){
        const Entry entry = queue.top();
        queue.pop();
        const Node current = nodes[static_cast<std::size_t>(entry.node)];
        if(current.time != entry.time) continue;
        ++result.expanded;
        if(current.position == goal && current.time >= forbidden.latest){
            goal_node = entry.node;
            break;
        }
        if(current.time == max_time) continue;
        std::vector<GridLocation> next_positions =
            map.neighbors(current.position);
        next_positions.push_back(current.position);
        for(const GridLocation next : next_positions){
            const int next_time = current.time + 1;
            if(forbidden.vertices.contains(TimedVertex{next, next_time})
               || forbidden.edges.contains(TimedEdge{
                   current.position, next, current.time
               })){
                continue;
            }
            const TimedVertex state{next, next_time};
            if(index.contains(state)) continue;
            const int next_node = static_cast<int>(nodes.size());
            nodes.push_back(Node{next, next_time, entry.node});
            index.emplace(state, next_node);
            queue.push(Entry{
                next_time + manhattan(next, goal), next_time, next_node
            });
        }
    }
    if(goal_node < 0) return result;
    for(int node = goal_node; node >= 0;
        node = nodes[static_cast<std::size_t>(node)].parent){
        result.path.push_back(nodes[static_cast<std::size_t>(node)].position);
    }
    std::reverse(result.path.begin(), result.path.end());
    return result;
}

struct Conflict{
    int first;
    int second;
    int time;
    GridLocation first_from;
    GridLocation first_to;
    GridLocation second_from;
    GridLocation second_to;
    bool edge;
};

inline std::vector<Conflict> all_conflicts(
    const std::vector<std::vector<GridLocation>>& paths
){
    std::vector<Conflict> result;
    int makespan = 0;
    for(const auto& path : paths){
        makespan = std::max(makespan, static_cast<int>(path.size()) - 1);
    }
    for(int time = 0; time <= makespan; ++time){
        for(int first = 0; first < static_cast<int>(paths.size()); ++first){
            for(int second = first + 1;
                second < static_cast<int>(paths.size());
                ++second){
                const GridLocation first_position =
                    at_time(paths[static_cast<std::size_t>(first)], time);
                const GridLocation second_position =
                    at_time(paths[static_cast<std::size_t>(second)], time);
                if(first_position == second_position){
                    result.push_back(Conflict{
                        first, second, time,
                        first_position, first_position,
                        second_position, second_position, false
                    });
                }else if(time < makespan){
                    const GridLocation first_next =
                        at_time(paths[static_cast<std::size_t>(first)], time + 1);
                    const GridLocation second_next =
                        at_time(paths[static_cast<std::size_t>(second)], time + 1);
                    if(first_position == second_next
                       && second_position == first_next){
                        result.push_back(Conflict{
                            first, second, time,
                            first_position, first_next,
                            second_position, second_next, true
                        });
                    }
                }
            }
        }
    }
    return result;
}

inline Constraint constraint_for(
    const Conflict& conflict,
    int agent
){
    if(!conflict.edge){
        return Constraint{
            agent, conflict.time, conflict.first_to, conflict.first_to, false
        };
    }
    if(agent == conflict.first){
        return Constraint{
            agent, conflict.time,
            conflict.first_from, conflict.first_to, true
        };
    }
    return Constraint{
        agent, conflict.time,
        conflict.second_from, conflict.second_to, true
    };
}

struct HighLevelNode{
    std::vector<std::vector<GridLocation>> paths;
    std::vector<Constraint> constraints;
    int cost = 0;
    std::size_t conflicts = 0;
    std::size_t order = 0;
};

inline int path_cost(
    const std::vector<std::vector<GridLocation>>& paths
){
    int result = 0;
    for(const auto& path : paths){
        result += static_cast<int>(path.size()) - 1;
    }
    return result;
}

inline std::optional<HighLevelNode> make_root(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    int max_time,
    std::size_t& low_expanded
){
    HighLevelNode root;
    root.paths.resize(starts.size());
    for(int agent = 0; agent < static_cast<int>(starts.size()); ++agent){
        LowLevelResult path = constrained_a_star(
            map, starts[static_cast<std::size_t>(agent)],
            goals[static_cast<std::size_t>(agent)], agent, {}, max_time
        );
        low_expanded += path.expanded;
        if(path.path.empty()) return std::nullopt;
        root.paths[static_cast<std::size_t>(agent)] = std::move(path.path);
    }
    root.cost = path_cost(root.paths);
    root.conflicts = all_conflicts(root.paths).size();
    return root;
}

inline std::optional<HighLevelNode> make_child(
    const HighLevelNode& parent,
    const Conflict& conflict,
    int agent,
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    int max_time,
    std::size_t& low_expanded
){
    HighLevelNode child = parent;
    child.constraints.push_back(constraint_for(conflict, agent));
    LowLevelResult path = constrained_a_star(
        map, starts[static_cast<std::size_t>(agent)],
        goals[static_cast<std::size_t>(agent)],
        agent, child.constraints, max_time
    );
    low_expanded += path.expanded;
    if(path.path.empty()) return std::nullopt;
    child.paths[static_cast<std::size_t>(agent)] = std::move(path.path);
    child.cost = path_cost(child.paths);
    child.conflicts = all_conflicts(child.paths).size();
    return child;
}

enum class CbsMode{
    bounded,
    enhanced,
    ecbs
};

inline MultiAgentPathResult cbs_impl(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    std::size_t max_high_level_nodes,
    int max_time,
    CbsMode mode,
    long double weight
){
    validate_agents(map, starts, goals);
    if(max_high_level_nodes == 0 || max_time < 0){
        throw std::invalid_argument("CBS limits must be positive/non-negative");
    }
    if(weight < 1.0L || !std::isfinite(weight)){
        throw std::invalid_argument("ECBS weight must be finite and at least one");
    }
    MultiAgentPathResult result;
    if(starts.empty()){
        result.paths = {};
        result.sum_of_costs = 0;
        result.makespan = 0;
        return result;
    }
    std::size_t low_expanded = 0;
    auto root = make_root(map, starts, goals, max_time, low_expanded);
    if(!root){
        result.expanded = low_expanded;
        return result;
    }
    std::vector<HighLevelNode> open;
    root->order = 0;
    open.push_back(std::move(*root));
    std::size_t order = 1;
    std::size_t high_expanded = 0;
    while(!open.empty() && high_expanded < max_high_level_nodes){
        std::size_t selected = 0;
        if(mode == CbsMode::ecbs){
            int minimum_cost = open.front().cost;
            for(const auto& node : open){
                minimum_cost = std::min(minimum_cost, node.cost);
            }
            const long double bound = weight * minimum_cost;
            for(std::size_t i = 1; i < open.size(); ++i){
                if(static_cast<long double>(open[i].cost) <= bound
                   && (static_cast<long double>(open[selected].cost) > bound
                       || std::tie(open[i].conflicts, open[i].cost, open[i].order)
                          < std::tie(
                              open[selected].conflicts,
                              open[selected].cost,
                              open[selected].order
                          ))){
                    selected = i;
                }
            }
        }else{
            for(std::size_t i = 1; i < open.size(); ++i){
                if(std::tie(open[i].cost, open[i].conflicts, open[i].order)
                   < std::tie(
                       open[selected].cost,
                       open[selected].conflicts,
                       open[selected].order
                   )){
                    selected = i;
                }
            }
        }
        HighLevelNode node = std::move(open[selected]);
        open[selected] = std::move(open.back());
        open.pop_back();
        ++high_expanded;
        std::vector<Conflict> conflicts = all_conflicts(node.paths);
        if(conflicts.empty()){
            result.paths = std::move(node.paths);
            finalize_result(result);
            result.expanded = high_expanded + low_expanded;
            return result;
        }
        Conflict chosen = conflicts.front();
        if(mode == CbsMode::enhanced){
            int best_class = -1;
            int best_time = std::numeric_limits<int>::max();
            for(const Conflict& candidate : conflicts){
                int increases = 0;
                for(const int agent : {candidate.first, candidate.second}){
                    std::size_t ignored = 0;
                    auto child = make_child(
                        node, candidate, agent, map, starts, goals,
                        max_time, ignored
                    );
                    low_expanded += ignored;
                    if(!child || child->cost > node.cost) ++increases;
                }
                if(best_class < increases
                   || (best_class == increases
                       && candidate.time < best_time)){
                    best_class = increases;
                    best_time = candidate.time;
                    chosen = candidate;
                }
            }
        }
        std::vector<HighLevelNode> children;
        for(const int agent : {chosen.first, chosen.second}){
            auto child = make_child(
                node, chosen, agent, map, starts, goals,
                max_time, low_expanded
            );
            if(child){
                child->order = order++;
                children.push_back(std::move(*child));
            }
        }
        if(mode == CbsMode::enhanced){
            auto bypass = std::find_if(
                children.begin(), children.end(),
                [&](const HighLevelNode& child){
                    return child.cost == node.cost
                        && child.conflicts < node.conflicts;
                }
            );
            if(bypass != children.end()){
                open.push_back(std::move(*bypass));
                continue;
            }
        }
        for(auto& child : children) open.push_back(std::move(child));
    }
    result.expanded = high_expanded + low_expanded;
    result.cutoff = !open.empty() && high_expanded == max_high_level_nodes;
    return result;
}

inline std::vector<TimedGridLocation> window_plan(
    const MultiAgentGridMap& map,
    GridLocation start,
    GridLocation goal,
    const ReservationTable& reservations,
    int start_time,
    int horizon
){
    struct Node{
        GridLocation position;
        int time;
        int parent;
    };
    struct Entry{
        int estimate;
        int time;
        int node;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            return std::tie(lhs.estimate, lhs.time)
                 > std::tie(rhs.estimate, rhs.time);
        }
    };
    if(reservations.is_vertex_reserved(start, start_time)) return {};
    std::vector<Node> nodes{{start, start_time, -1}};
    std::unordered_map<TimedVertex, int, TimedVertexHash> index;
    index.emplace(TimedVertex{start, start_time}, 0);
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    queue.push(Entry{manhattan(start, goal), start_time, 0});
    int chosen = 0;
    while(!queue.empty()){
        const Entry entry = queue.top();
        queue.pop();
        const Node current = nodes[static_cast<std::size_t>(entry.node)];
        if(current.time != entry.time) continue;
        const Node& old_chosen = nodes[static_cast<std::size_t>(chosen)];
        if(std::tuple{
               manhattan(current.position, goal),
               current.time
           } < std::tuple{
               manhattan(old_chosen.position, goal),
               old_chosen.time
           }){
            chosen = entry.node;
        }
        if(current.position == goal || current.time == horizon){
            if(current.position == goal) chosen = entry.node;
            if(current.position == goal) break;
            continue;
        }
        auto candidates = map.neighbors(current.position);
        candidates.push_back(current.position);
        for(const GridLocation next : candidates){
            const int next_time = current.time + 1;
            if(reservations.is_vertex_reserved(next, next_time)
               || reservations.is_edge_reserved(
                   next, current.position, current.time
               )){
                continue;
            }
            const TimedVertex key{next, next_time};
            if(index.contains(key)) continue;
            const int next_node = static_cast<int>(nodes.size());
            nodes.push_back(Node{next, next_time, entry.node});
            index.emplace(key, next_node);
            queue.push(Entry{
                next_time + manhattan(next, goal), next_time, next_node
            });
        }
    }
    std::vector<TimedGridLocation> path;
    for(int node = chosen; node >= 0;
        node = nodes[static_cast<std::size_t>(node)].parent){
        const Node& value = nodes[static_cast<std::size_t>(node)];
        path.push_back(TimedGridLocation{value.position, value.time});
    }
    std::reverse(path.begin(), path.end());
    if(path.empty()) return path;
    while(path.back().time < horizon){
        const int next_time = path.back().time + 1;
        if(reservations.is_vertex_reserved(path.back().position, next_time)){
            return {};
        }
        path.push_back(TimedGridLocation{path.back().position, next_time});
    }
    return path;
}

} // namespace multi_agent_pathfinding_internal

inline MultiAgentPathResult cooperative_a_star_search(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    int max_time,
    const ReservationTable& initial_reservations = {}
){
    using namespace multi_agent_pathfinding_internal;
    validate_agents(map, starts, goals);
    if(max_time < 0){
        throw std::invalid_argument("Cooperative A* max_time must be non-negative");
    }
    MultiAgentPathResult result;
    result.paths.resize(starts.size());
    ReservationTable reservations = initial_reservations;
    for(std::size_t agent = 0; agent < starts.size(); ++agent){
        TimedGridPathResult path = sipp_search(
            map, starts[agent], goals[agent], reservations,
            0, max_time, max_time
        );
        result.expanded += path.expanded;
        if(!path) return MultiAgentPathResult{};
        reservations.reserve_path(path.path, max_time);
        result.paths[agent] = untimed(path.path);
    }
    finalize_result(result);
    return result;
}

inline MultiAgentPathResult windowed_hierarchical_cooperative_a_star_search(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    int window,
    int max_steps,
    const ReservationTable& initial_reservations = {}
){
    using namespace multi_agent_pathfinding_internal;
    validate_agents(map, starts, goals);
    if(window <= 0 || max_steps < 0){
        throw std::invalid_argument("WHCA* needs window > 0 and max_steps >= 0");
    }
    MultiAgentPathResult result;
    result.paths.resize(starts.size());
    std::vector<GridLocation> current = starts;
    for(std::size_t agent = 0; agent < starts.size(); ++agent){
        result.paths[agent].push_back(starts[agent]);
    }
    for(int time = 0; time < max_steps; ++time){
        if(current == goals){
            finalize_result(result);
            return result;
        }
        ReservationTable reservations = initial_reservations;
        std::vector<std::vector<TimedGridLocation>> plans(starts.size());
        for(std::size_t agent = 0; agent < starts.size(); ++agent){
            plans[agent] = window_plan(
                map, current[agent], goals[agent], reservations,
                time, std::min(max_steps, time + window)
            );
            ++result.expanded;
            if(plans[agent].size() < 2) return MultiAgentPathResult{};
            reservations.reserve_path(plans[agent]);
        }
        for(std::size_t agent = 0; agent < starts.size(); ++agent){
            current[agent] = plans[agent][1].position;
            result.paths[agent].push_back(current[agent]);
        }
    }
    if(current == goals){
        finalize_result(result);
    }else{
        result.cutoff = true;
    }
    return result;
}

inline MultiAgentPathResult whca_star_search(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    int window,
    int max_steps,
    const ReservationTable& initial_reservations = {}
){
    return windowed_hierarchical_cooperative_a_star_search(
        map, starts, goals, window, max_steps, initial_reservations
    );
}

inline MultiAgentPathResult bounded_conflict_based_search(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    std::size_t max_high_level_nodes,
    int max_time
){
    return multi_agent_pathfinding_internal::cbs_impl(
        map, starts, goals, max_high_level_nodes, max_time,
        multi_agent_pathfinding_internal::CbsMode::bounded, 1.0L
    );
}

inline MultiAgentPathResult enhanced_conflict_based_search(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    std::size_t max_high_level_nodes,
    int max_time
){
    return multi_agent_pathfinding_internal::cbs_impl(
        map, starts, goals, max_high_level_nodes, max_time,
        multi_agent_pathfinding_internal::CbsMode::enhanced, 1.0L
    );
}

inline MultiAgentPathResult enhanced_cbs_search(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    std::size_t max_high_level_nodes,
    int max_time
){
    return enhanced_conflict_based_search(
        map, starts, goals, max_high_level_nodes, max_time
    );
}

inline MultiAgentPathResult ecbs_search(
    const MultiAgentGridMap& map,
    const std::vector<GridLocation>& starts,
    const std::vector<GridLocation>& goals,
    long double weight,
    std::size_t max_high_level_nodes,
    int max_time
){
    return multi_agent_pathfinding_internal::cbs_impl(
        map, starts, goals, max_high_level_nodes, max_time,
        multi_agent_pathfinding_internal::CbsMode::ecbs, weight
    );
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_MULTI_AGENT_PATHFINDING_HPP_INCLUDED
