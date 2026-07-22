#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FORD_FULKERSON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FORD_FULKERSON_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

template<class T>
struct FordFulkersonEdge{
    int to;
    int rev;
    T cap;
};

template<class T>
struct FordFulkerson{
    int n;
    std::vector<std::vector<FordFulkersonEdge<T>>> graph;
    std::vector<char> used;
    std::vector<int> level;
    std::vector<std::size_t> iter;

    explicit FordFulkerson(int n_)
        : n(n_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (FordFulkerson)."
            )
            : n_),
          graph(static_cast<std::size_t>(n)),
          used(static_cast<std::size_t>(n)),
          level(static_cast<std::size_t>(n)),
          iter(static_cast<std::size_t>(n)){}

    void add_edge(int from, int to, T cap){
        if(from < 0 || n <= from || to < 0 || n <= to || cap < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (FordFulkerson::add_edge).");
        }
        int from_rev = static_cast<int>(graph[static_cast<std::size_t>(to)].size());
        int to_rev = static_cast<int>(graph[static_cast<std::size_t>(from)].size());
        if(from == to) from_rev++;
        graph[static_cast<std::size_t>(from)].push_back({to, from_rev, cap});
        graph[static_cast<std::size_t>(to)].push_back({from, to_rev, T(0)});
    }

    T dfs(int v, int t, T f){
        if(v == t) return f;
        used[static_cast<std::size_t>(v)] = 1;
        for(auto& e: graph[static_cast<std::size_t>(v)]){
            if(e.cap <= T(0) || used[static_cast<std::size_t>(e.to)]) continue;
            T pushed = dfs(e.to, t, std::min(f, e.cap));
            if(pushed > T(0)){
                e.cap -= pushed;
                graph[static_cast<std::size_t>(e.to)][static_cast<std::size_t>(e.rev)].cap += pushed;
                return pushed;
            }
        }
        return T(0);
    }

private:
    bool build_level_graph(int source, int sink){
        std::fill(level.begin(), level.end(), -1);
        std::queue<int> queue;
        level[static_cast<std::size_t>(source)] = 0;
        queue.push(source);
        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
                if(edge.cap <= T(0)
                    || level[static_cast<std::size_t>(edge.to)] != -1){
                    continue;
                }
                level[static_cast<std::size_t>(edge.to)] =
                    level[static_cast<std::size_t>(vertex)] + 1;
                queue.push(edge.to);
            }
        }
        return level[static_cast<std::size_t>(sink)] != -1;
    }

    T blocking_flow_dfs(int vertex, int sink, T limit){
        if(vertex == sink) return limit;
        auto& edge_index = iter[static_cast<std::size_t>(vertex)];
        while(edge_index < graph[static_cast<std::size_t>(vertex)].size()){
            auto& edge =
                graph[static_cast<std::size_t>(vertex)][edge_index];
            if(edge.cap > T(0)
                && level[static_cast<std::size_t>(edge.to)]
                    == level[static_cast<std::size_t>(vertex)] + 1){
                const T pushed = blocking_flow_dfs(
                    edge.to, sink, std::min(limit, edge.cap)
                );
                if(pushed > T(0)){
                    edge.cap -= pushed;
                    graph[static_cast<std::size_t>(edge.to)]
                        [static_cast<std::size_t>(edge.rev)].cap += pushed;
                    return pushed;
                }
            }
            ++edge_index;
        }
        return T(0);
    }

public:
    T max_flow(int s, int t, T flow_limit = std::numeric_limits<T>::max() / 4){
        if(s < 0 || n <= s || t < 0 || n <= t || flow_limit < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (FordFulkerson::max_flow).");
        }
        if(s == t) return T(0);
        T flow = T(0);
        while(flow < flow_limit && build_level_graph(s, t)){
            std::fill(iter.begin(), iter.end(), std::size_t(0));
            while(flow < flow_limit){
                const T pushed =
                    blocking_flow_dfs(s, t, flow_limit - flow);
                if(pushed == T(0)) break;
                flow += pushed;
            }
        }
        return flow;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FORD_FULKERSON_HPP_INCLUDED
