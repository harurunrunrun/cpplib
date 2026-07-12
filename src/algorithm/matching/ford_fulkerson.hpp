#pragma once

#include <algorithm>
#include <limits>
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

    explicit FordFulkerson(int n_): n(n_), graph(static_cast<std::size_t>(n_)), used(static_cast<std::size_t>(n_)){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (FordFulkerson).");
        }
    }

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

    T max_flow(int s, int t, T flow_limit = std::numeric_limits<T>::max() / 4){
        if(s < 0 || n <= s || t < 0 || n <= t || flow_limit < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (FordFulkerson::max_flow).");
        }
        if(s == t) return T(0);
        T flow = T(0);
        while(flow < flow_limit){
            std::fill(used.begin(), used.end(), 0);
            T pushed = dfs(s, t, flow_limit - flow);
            if(pushed == T(0)) break;
            flow += pushed;
        }
        return flow;
    }
};
