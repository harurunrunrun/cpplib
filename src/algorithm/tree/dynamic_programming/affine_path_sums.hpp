#ifndef CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_AFFINE_PATH_SUMS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_AFFINE_PATH_SUMS_HPP_INCLUDED

#include <stdexcept>
#include <vector>

template<class T>
struct AffineTreeEdge{ int u, v; T multiplier, addend; };

// For every root r, sum the value at each vertex transported toward r.
// Each undirected edge transports x to multiplier*x + addend in either direction.
template<class T>
std::vector<T> affine_path_sums(const std::vector<T>& values,
                              const std::vector<AffineTreeEdge<T>>& edges,
                              const std::vector<T>& vertex_mass = {}){
    const int n = static_cast<int>(values.size());
    if(static_cast<int>(edges.size()) != (n == 0 ? 0 : n - 1)) throw std::invalid_argument("affine path tree size");
    if(n == 0) return {};
    std::vector<std::vector<int>> graph(n);
    for(int e = 0; e < n - 1; ++e){
        auto [u, v, b, c] = edges[e]; (void)b; (void)c;
        if(u < 0 || u >= n || v < 0 || v >= n) throw std::out_of_range("affine path edge");
        graph[u].push_back(e); graph[v].push_back(e);
    }
    std::vector<int> parent(n, -2), order{0}, pe(n, -1), size(n, 1);
    parent[0] = -1;
    for(int i = 0; i < static_cast<int>(order.size()); ++i){
        int v = order[i];
        for(int e: graph[v]) if(e != pe[v]){
            int to = edges[e].u ^ edges[e].v ^ v;
            if(parent[to] != -2) throw std::invalid_argument("affine path cycle");
            parent[to] = v; pe[to] = e; order.push_back(to);
        }
    }
    if(static_cast<int>(order.size()) != n) throw std::invalid_argument("disconnected affine path tree");
    if(!vertex_mass.empty() && vertex_mass.size() != values.size()) throw std::invalid_argument("affine path mass size");
    std::vector<T> mass = vertex_mass.empty() ? std::vector<T>(n, T{1}) : vertex_mass;
    std::vector<T> down(n), answer(n);
    for(int v = 0; v < n; ++v) down[v] = values[v] * mass[v];
    for(int i = n - 1; i > 0; --i){
        int v = order[i], p = parent[v]; const auto& e = edges[pe[v]];
        down[p] += e.multiplier * down[v] + e.addend * mass[v];
        size[p] += size[v];
        mass[p] += mass[v];
    }
    answer[0] = down[0];
    for(int i = 1; i < n; ++i){
        int v = order[i]; const auto& e = edges[pe[v]];
        const T rest = answer[parent[v]] - e.multiplier * down[v] - e.addend * mass[v];
        answer[v] = down[v] + e.multiplier * rest + e.addend * (mass[0] - mass[v]);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_AFFINE_PATH_SUMS_HPP_INCLUDED
