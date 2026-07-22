#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MIN_COST_FLOW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MIN_COST_FLOW_HPP_INCLUDED

#include <algorithm>
#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct MinCostFlowEdge{
    int from;
    int to;
    T cap;
    T flow;
    T cost;
};

template<class T>
struct MinCostFlowResult{
    T flow;
    T cost;
};

template<class T>
struct MinCostFlow{
    struct InternalEdge{
        int to;
        int rev;
        T cap;
        T cost;
        int id;
    };

    int n;
    std::vector<MinCostFlowEdge<T>> edges;
    std::vector<std::vector<InternalEdge>> graph;

    explicit MinCostFlow(int n_)
        : n(n_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (MinCostFlow)."
            )
            : n_),
          graph(static_cast<std::size_t>(n)){}

    int add_edge(int from, int to, T cap, T cost){
        if(from < 0 || n <= from || to < 0 || n <= to || cap < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (MinCostFlow::add_edge).");
        }
        int id = static_cast<int>(edges.size());
        edges.push_back({from, to, cap, T(0), cost});
        int from_rev = static_cast<int>(graph[static_cast<std::size_t>(to)].size());
        int to_rev = static_cast<int>(graph[static_cast<std::size_t>(from)].size());
        if(from == to) from_rev++;
        graph[static_cast<std::size_t>(from)].push_back({to, from_rev, cap, cost, id});
        graph[static_cast<std::size_t>(to)].push_back({from, to_rev, T(0), -cost, id});
        return id;
    }

private:
    std::vector<T> initial_potentials(int source, T inf) const{
        std::vector<T> distance(static_cast<std::size_t>(n), inf);
        std::vector<char> reached(static_cast<std::size_t>(n), 0);
        distance[static_cast<std::size_t>(source)] = T(0);
        reached[static_cast<std::size_t>(source)] = 1;
        for(int iteration = 0; iteration < n; ++iteration){
            bool updated = false;
            for(int vertex = 0; vertex < n; ++vertex){
                if(!reached[static_cast<std::size_t>(vertex)]) continue;
                for(const InternalEdge& edge:
                    graph[static_cast<std::size_t>(vertex)]){
                    if(edge.cap <= T(0)) continue;
                    const T candidate =
                        distance[static_cast<std::size_t>(vertex)] + edge.cost;
                    if(reached[static_cast<std::size_t>(edge.to)]
                        && !(candidate < distance[static_cast<std::size_t>(edge.to)])){
                        continue;
                    }
                    distance[static_cast<std::size_t>(edge.to)] = candidate;
                    reached[static_cast<std::size_t>(edge.to)] = 1;
                    updated = true;
                }
            }
            if(!updated) break;
            if(iteration + 1 == n)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: reachable negative cycle "
                    "(MinCostFlow::min_cost_flow)."
                );
            }
        }

        std::vector<T> potential(static_cast<std::size_t>(n), T(0));
        for(int vertex = 0; vertex < n; ++vertex){
            if(reached[static_cast<std::size_t>(vertex)]){
                potential[static_cast<std::size_t>(vertex)] =
                    distance[static_cast<std::size_t>(vertex)];
            }
        }
        return potential;
    }

public:
    MinCostFlowResult<T> min_cost_flow(
        int source,
        int sink,
        T flow_limit = std::numeric_limits<T>::max() / 4,
        T inf = std::numeric_limits<T>::max() / 4
    ){
        if(source < 0 || n <= source || sink < 0 || n <= sink || source == sink || flow_limit < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (MinCostFlow::min_cost_flow).");
        }
        MinCostFlowResult<T> result{T(0), T(0)};
        if(flow_limit == T(0)) return result;
        std::vector<T> potential = initial_potentials(source, inf);
        std::vector<T> distance(static_cast<std::size_t>(n));
        std::vector<char> reached(static_cast<std::size_t>(n));
        std::vector<int> parent_vertex(static_cast<std::size_t>(n));
        std::vector<int> parent_edge(static_cast<std::size_t>(n));
        using QueueEntry = std::pair<T, int>;
        while(result.flow < flow_limit){
            std::fill(distance.begin(), distance.end(), inf);
            std::fill(reached.begin(), reached.end(), 0);
            std::fill(parent_vertex.begin(), parent_vertex.end(), -1);
            std::fill(parent_edge.begin(), parent_edge.end(), -1);
            std::priority_queue<
                QueueEntry,
                std::vector<QueueEntry>,
                std::greater<QueueEntry>
            > queue;
            distance[static_cast<std::size_t>(source)] = T(0);
            reached[static_cast<std::size_t>(source)] = 1;
            queue.push({T(0), source});
            while(!queue.empty()){
                const auto [current_distance, vertex] = queue.top();
                queue.pop();
                if(distance[static_cast<std::size_t>(vertex)]
                    < current_distance){
                    continue;
                }
                const auto& adjacent = graph[static_cast<std::size_t>(vertex)];
                for(int edge_index = 0;
                    edge_index < static_cast<int>(adjacent.size());
                    ++edge_index){
                    const InternalEdge& edge =
                        adjacent[static_cast<std::size_t>(edge_index)];
                    if(edge.cap <= T(0)) continue;
                    const T reduced_cost = edge.cost
                        + potential[static_cast<std::size_t>(vertex)]
                        - potential[static_cast<std::size_t>(edge.to)];
                    const T candidate = current_distance + reduced_cost;
                    if(reached[static_cast<std::size_t>(edge.to)]
                        && !(candidate < distance[static_cast<std::size_t>(edge.to)])){
                        continue;
                    }
                    distance[static_cast<std::size_t>(edge.to)] = candidate;
                    reached[static_cast<std::size_t>(edge.to)] = 1;
                    parent_vertex[static_cast<std::size_t>(edge.to)] = vertex;
                    parent_edge[static_cast<std::size_t>(edge.to)] = edge_index;
                    queue.push({candidate, edge.to});
                }
            }
            if(!reached[static_cast<std::size_t>(sink)]) break;
            for(int vertex = 0; vertex < n; ++vertex){
                if(reached[static_cast<std::size_t>(vertex)]){
                    potential[static_cast<std::size_t>(vertex)] +=
                        distance[static_cast<std::size_t>(vertex)];
                }
            }

            T add = flow_limit - result.flow;
            for(int vertex = sink; vertex != source;
                vertex = parent_vertex[static_cast<std::size_t>(vertex)]){
                const int previous =
                    parent_vertex[static_cast<std::size_t>(vertex)];
                const int edge_index =
                    parent_edge[static_cast<std::size_t>(vertex)];
                add = std::min(add, graph[static_cast<std::size_t>(previous)]
                    [static_cast<std::size_t>(edge_index)].cap);
            }
            for(int vertex = sink; vertex != source;
                vertex = parent_vertex[static_cast<std::size_t>(vertex)]){
                const int previous =
                    parent_vertex[static_cast<std::size_t>(vertex)];
                const int edge_index =
                    parent_edge[static_cast<std::size_t>(vertex)];
                auto& e = graph[static_cast<std::size_t>(previous)]
                    [static_cast<std::size_t>(edge_index)];
                e.cap -= add;
                graph[static_cast<std::size_t>(e.to)][static_cast<std::size_t>(e.rev)].cap += add;
                if(edges[static_cast<std::size_t>(e.id)].from == previous){
                    edges[static_cast<std::size_t>(e.id)].flow += add;
                }else{
                    edges[static_cast<std::size_t>(e.id)].flow -= add;
                }
            }
            result.flow += add;
            result.cost += add * (
                potential[static_cast<std::size_t>(sink)]
                - potential[static_cast<std::size_t>(source)]);
        }
        return result;
    }

    std::optional<T> min_cost_for_exact_flow(
        int source,
        int sink,
        T required_flow,
        T inf = std::numeric_limits<T>::max() / 4
    ){
        const auto result = min_cost_flow(source, sink, required_flow, inf);
        if(result.flow != required_flow) return std::nullopt;
        return result.cost;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MIN_COST_FLOW_HPP_INCLUDED
