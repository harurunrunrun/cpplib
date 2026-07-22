#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_DINIC_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_DINIC_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

template<class T>
struct DinicEdge{
    int from;
    int to;
    T cap;
    T flow;
};

template<class T>
struct Dinic{
    int n;
    std::vector<DinicEdge<T>> edges;
    std::vector<std::vector<int>> graph;
    std::vector<int> level;
    std::vector<int> iter;

    explicit Dinic(int n_)
        : n(n_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (Dinic)."
            )
            : n_),
          graph(static_cast<std::size_t>(n)),
          level(static_cast<std::size_t>(n)),
          iter(static_cast<std::size_t>(n)){}

    int add_edge(int from, int to, T cap){
        if(from < 0 || n <= from || to < 0 || n <= to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (Dinic::add_edge).");
        }
        if(cap < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: negative capacity (Dinic::add_edge).");
        }
        int id = static_cast<int>(edges.size());
        graph[static_cast<std::size_t>(from)].push_back(id);
        edges.push_back({from, to, cap, T(0)});
        graph[static_cast<std::size_t>(to)].push_back(id ^ 1);
        edges.push_back({to, from, T(0), T(0)});
        return id;
    }

    bool bfs(int s, int t){
        std::fill(level.begin(), level.end(), -1);
        std::queue<int> que;
        level[static_cast<std::size_t>(s)] = 0;
        que.push(s);
        while(!que.empty()){
            int v = que.front();
            que.pop();
            for(int id: graph[static_cast<std::size_t>(v)]){
                const auto& e = edges[static_cast<std::size_t>(id)];
                if(e.cap - e.flow <= T(0) || level[static_cast<std::size_t>(e.to)] != -1) continue;
                level[static_cast<std::size_t>(e.to)] = level[static_cast<std::size_t>(v)] + 1;
                que.push(e.to);
            }
        }
        return level[static_cast<std::size_t>(t)] != -1;
    }

    T dfs(int v, int t, T f){
        if(v == t) return f;
        for(int& i = iter[static_cast<std::size_t>(v)]; i < static_cast<int>(graph[static_cast<std::size_t>(v)].size()); i++){
            int id = graph[static_cast<std::size_t>(v)][static_cast<std::size_t>(i)];
            auto& e = edges[static_cast<std::size_t>(id)];
            if(e.cap - e.flow <= T(0) || level[static_cast<std::size_t>(v)] >= level[static_cast<std::size_t>(e.to)]) continue;
            T pushed = dfs(e.to, t, std::min(f, e.cap - e.flow));
            if(pushed > T(0)){
                e.flow += pushed;
                edges[static_cast<std::size_t>(id ^ 1)].flow -= pushed;
                return pushed;
            }
        }
        return T(0);
    }

    T max_flow(int s, int t, T flow_limit = std::numeric_limits<T>::max() / 4){
        if(s < 0 || n <= s || t < 0 || n <= t || flow_limit < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (Dinic::max_flow).");
        }
        if(s == t) return T(0);
        T flow = T(0);
        while(flow < flow_limit && bfs(s, t)){
            std::fill(iter.begin(), iter.end(), 0);
            while(flow < flow_limit){
                T pushed = dfs(s, t, flow_limit - flow);
                if(pushed == T(0)) break;
                flow += pushed;
            }
        }
        return flow;
    }

    std::vector<char> min_cut(int s) const{
        if(s < 0 || n <= s)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (Dinic::min_cut).");
        }
        std::vector<char> visited(static_cast<std::size_t>(n), 0);
        std::vector<int> stack{s};
        visited[static_cast<std::size_t>(s)] = 1;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            for(int id: graph[static_cast<std::size_t>(v)]){
                const auto& e = edges[static_cast<std::size_t>(id)];
                if(e.cap - e.flow <= T(0) || visited[static_cast<std::size_t>(e.to)]) continue;
                visited[static_cast<std::size_t>(e.to)] = 1;
                stack.push_back(e.to);
            }
        }
        return visited;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_DINIC_HPP_INCLUDED
