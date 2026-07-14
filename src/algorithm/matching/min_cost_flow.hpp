#pragma once

#include <algorithm>
#include <limits>
#include <stdexcept>
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
        while(result.flow < flow_limit){
            std::vector<T> dist(static_cast<std::size_t>(n), inf);
            std::vector<char> reached(static_cast<std::size_t>(n), 0);
            std::vector<int> parent_v(static_cast<std::size_t>(n), -1);
            std::vector<int> parent_e(static_cast<std::size_t>(n), -1);
            dist[static_cast<std::size_t>(source)] = T(0);
            reached[static_cast<std::size_t>(source)] = 1;
            bool updated = true;
            for(int iter = 0; iter < n && updated; iter++){
                updated = false;
                for(int v = 0; v < n; v++){
                    if(!reached[static_cast<std::size_t>(v)]) continue;
                    for(int i = 0; i < static_cast<int>(graph[static_cast<std::size_t>(v)].size()); i++){
                        const auto& e = graph[static_cast<std::size_t>(v)][static_cast<std::size_t>(i)];
                        if(e.cap <= T(0)) continue;
                        T nd = dist[static_cast<std::size_t>(v)] + e.cost;
                        if(!reached[static_cast<std::size_t>(e.to)] ||
                           nd < dist[static_cast<std::size_t>(e.to)]){
                            dist[static_cast<std::size_t>(e.to)] = nd;
                            reached[static_cast<std::size_t>(e.to)] = 1;
                            parent_v[static_cast<std::size_t>(e.to)] = v;
                            parent_e[static_cast<std::size_t>(e.to)] = i;
                            updated = true;
                        }
                    }
                }
            }
            if(updated)[[unlikely]]{
                throw std::runtime_error("library assertion fault: reachable negative cycle (MinCostFlow::min_cost_flow).");
            }
            if(!reached[static_cast<std::size_t>(sink)]) break;

            T add = flow_limit - result.flow;
            for(int v = sink; v != source; v = parent_v[static_cast<std::size_t>(v)]){
                const auto& e = graph[static_cast<std::size_t>(parent_v[static_cast<std::size_t>(v)])][static_cast<std::size_t>(parent_e[static_cast<std::size_t>(v)])];
                add = std::min(add, e.cap);
            }
            for(int v = sink; v != source; v = parent_v[static_cast<std::size_t>(v)]){
                int pv = parent_v[static_cast<std::size_t>(v)];
                int pe = parent_e[static_cast<std::size_t>(v)];
                auto& e = graph[static_cast<std::size_t>(pv)][static_cast<std::size_t>(pe)];
                e.cap -= add;
                graph[static_cast<std::size_t>(e.to)][static_cast<std::size_t>(e.rev)].cap += add;
                if(edges[static_cast<std::size_t>(e.id)].from == pv){
                    edges[static_cast<std::size_t>(e.id)].flow += add;
                }else{
                    edges[static_cast<std::size_t>(e.id)].flow -= add;
                }
            }
            result.flow += add;
            result.cost += add * dist[static_cast<std::size_t>(sink)];
        }
        return result;
    }
};
