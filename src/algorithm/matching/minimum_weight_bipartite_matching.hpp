#pragma once

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
struct MinimumWeightBipartiteMatchingEdge{
    int left;
    int right;
    T cost;
};

template<class T>
struct MinimumWeightBipartiteMatchingResult{
    int size;
    T cost;
    std::vector<int> left_match;
    std::vector<int> right_match;
};

template<class T>
MinimumWeightBipartiteMatchingResult<T> minimum_weight_bipartite_matching(
    int left_size,
    int right_size,
    const std::vector<MinimumWeightBipartiteMatchingEdge<T>>& input_edges,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(left_size < 0 || right_size < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (minimum_weight_bipartite_matching).");
    }
    const int source = left_size + right_size;
    const int sink = source + 1;
    const int n = sink + 1;

    struct Edge{
        int to;
        int rev;
        int cap;
        T cost;
        int id;
    };
    std::vector<std::vector<Edge>> graph(static_cast<std::size_t>(n));
    auto add_edge = [&](int from, int to, int cap, T cost, int id) -> void {
        int from_rev = static_cast<int>(graph[static_cast<std::size_t>(to)].size());
        int to_rev = static_cast<int>(graph[static_cast<std::size_t>(from)].size());
        graph[static_cast<std::size_t>(from)].push_back({to, from_rev, cap, cost, id});
        graph[static_cast<std::size_t>(to)].push_back({from, to_rev, 0, -cost, -1});
    };

    for(int i = 0; i < left_size; i++) add_edge(source, i, 1, T(0), -1);
    for(int i = 0; i < right_size; i++) add_edge(left_size + i, sink, 1, T(0), -1);
    for(int id = 0; id < static_cast<int>(input_edges.size()); id++){
        const auto& e = input_edges[static_cast<std::size_t>(id)];
        if(e.left < 0 || left_size <= e.left || e.right < 0 || right_size <= e.right)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (minimum_weight_bipartite_matching).");
        }
        add_edge(e.left, left_size + e.right, 1, e.cost, id);
    }

    MinimumWeightBipartiteMatchingResult<T> result;
    result.size = 0;
    result.cost = T(0);
    result.left_match.assign(static_cast<std::size_t>(left_size), -1);
    result.right_match.assign(static_cast<std::size_t>(right_size), -1);

    while(true){
        std::vector<T> dist(static_cast<std::size_t>(n), inf);
        std::vector<char> reachable(static_cast<std::size_t>(n), 0);
        std::vector<int> parent_v(static_cast<std::size_t>(n), -1);
        std::vector<int> parent_e(static_cast<std::size_t>(n), -1);
        dist[static_cast<std::size_t>(source)] = T(0);
        reachable[static_cast<std::size_t>(source)] = 1;
        bool updated = true;
        for(int iter = 0; iter < n && updated; iter++){
            updated = false;
            for(int v = 0; v < n; v++){
                if(!reachable[static_cast<std::size_t>(v)]) continue;
                for(int i = 0; i < static_cast<int>(graph[static_cast<std::size_t>(v)].size()); i++){
                    const auto& e = graph[static_cast<std::size_t>(v)][static_cast<std::size_t>(i)];
                    if(e.cap <= 0) continue;
                    T nd = dist[static_cast<std::size_t>(v)] + e.cost;
                    if(!reachable[static_cast<std::size_t>(e.to)] ||
                       nd < dist[static_cast<std::size_t>(e.to)]){
                        dist[static_cast<std::size_t>(e.to)] = nd;
                        reachable[static_cast<std::size_t>(e.to)] = 1;
                        parent_v[static_cast<std::size_t>(e.to)] = v;
                        parent_e[static_cast<std::size_t>(e.to)] = i;
                        updated = true;
                    }
                }
            }
        }
        if(!reachable[static_cast<std::size_t>(sink)]) break;

        result.size++;
        result.cost += dist[static_cast<std::size_t>(sink)];
        int v = sink;
        while(v != source){
            int pv = parent_v[static_cast<std::size_t>(v)];
            int pe = parent_e[static_cast<std::size_t>(v)];
            auto& e = graph[static_cast<std::size_t>(pv)][static_cast<std::size_t>(pe)];
            e.cap--;
            graph[static_cast<std::size_t>(e.to)][static_cast<std::size_t>(e.rev)].cap++;
            v = pv;
        }
    }

    for(int left = 0; left < left_size; left++){
        for(const auto& e: graph[static_cast<std::size_t>(left)]){
            if(left_size <= e.to && e.to < left_size + right_size && e.cap == 0 && e.id != -1){
                int right = e.to - left_size;
                result.left_match[static_cast<std::size_t>(left)] = right;
                result.right_match[static_cast<std::size_t>(right)] = left;
            }
        }
    }
    return result;
}
