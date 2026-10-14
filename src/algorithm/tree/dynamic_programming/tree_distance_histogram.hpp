#ifndef CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DISTANCE_HISTOGRAM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DISTANCE_HISTOGRAM_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../fft/transform/convolution.hpp"

// Sum mass[u]*mass[v] over unordered distinct vertex pairs at each distance.
template<int MOD>
std::vector<Modint<MOD>> tree_distance_pair_sums(
    const std::vector<Modint<MOD>>& mass, const std::vector<std::pair<int, int>>& edges){
    using mint = Modint<MOD>;
    const int n = static_cast<int>(mass.size());
    if(static_cast<int>(edges.size()) != (n ? n - 1 : 0)) throw std::invalid_argument("distance histogram tree size");
    std::vector<std::vector<int>> graph(n);
    for(auto [u, v]: edges){
        if(u < 0 || u >= n || v < 0 || v >= n) throw std::out_of_range("distance histogram edge");
        graph[u].push_back(v); graph[v].push_back(u);
    }
    std::vector<mint> answer(n);
    if(n == 0) return answer;
    std::vector<int> validation_parent(n,-2),validation_order{0};
    validation_parent[0]=-1;
    for(int i=0;i<static_cast<int>(validation_order.size());++i){
        int v=validation_order[i];
        for(int u:graph[v])if(u!=validation_parent[v]){
            if(validation_parent[u]!=-2)throw std::invalid_argument("distance histogram cycle");
            validation_parent[u]=v;validation_order.push_back(u);
        }
    }
    if(static_cast<int>(validation_order.size())!=n)throw std::invalid_argument("disconnected distance histogram tree");
    std::vector<int> removed(n), parent(n, -1), size(n), pending{0};
    while(!pending.empty()){
        int start = pending.back(); pending.pop_back();
        std::vector<int> vertices{start}; parent[start] = -1;
        for(int i = 0; i < static_cast<int>(vertices.size()); ++i){
            int v = vertices[i];
            for(int u: graph[v]) if(!removed[u] && u != parent[v]){
                parent[u] = v; vertices.push_back(u);
            }
        }
        for(auto it = vertices.rbegin(); it != vertices.rend(); ++it){
            int v = *it; size[v] = 1;
            for(int u: graph[v]) if(!removed[u] && parent[u] == v) size[v] += size[u];
        }
        int centroid = start, total = static_cast<int>(vertices.size());
        for(int v: vertices){
            int largest = total - size[v];
            for(int u: graph[v]) if(!removed[u] && parent[u] == v) largest = std::max(largest, size[u]);
            if(largest * 2 <= total){ centroid = v; break; }
        }
        removed[centroid] = 1;
        std::vector<mint> all{mass[centroid]};
        for(int neighbor: graph[centroid]) if(!removed[neighbor]){
            pending.push_back(neighbor);
            std::vector<std::pair<int, int>> stack{{neighbor, 1}};
            parent[neighbor] = centroid;
            std::vector<mint> branch(1);
            while(!stack.empty()){
                auto [v, d] = stack.back(); stack.pop_back();
                if(static_cast<int>(branch.size()) <= d) branch.resize(d + 1);
                branch[d] += mass[v];
                for(int u: graph[v]) if(!removed[u] && u != parent[v]){ parent[u] = v; stack.emplace_back(u, d + 1); }
            }
            if(all.size() < branch.size()) all.resize(branch.size());
            for(int i = 1; i < static_cast<int>(branch.size()); ++i) all[i] += branch[i];
            auto subtract = convolution_ntt(branch, branch);
            for(int i = 1; i < std::min(n, static_cast<int>(subtract.size())); ++i) answer[i] -= subtract[i];
        }
        auto add = convolution_ntt(all, all);
        for(int i = 1; i < std::min(n, static_cast<int>(add.size())); ++i) answer[i] += add[i];
    }
    const mint half = mint{2}.inv();
    for(auto& x: answer) x *= half;
    return answer;
}

inline std::vector<long long> tree_distance_histogram(int n, const std::vector<std::pair<int, int>>& edges){
    if(n < 0) throw std::invalid_argument("negative distance histogram size");
    constexpr long long p = 998244353, q = 469762049;
    auto a = tree_distance_pair_sums<p>(std::vector<Modint<p>>(n, 1), edges);
    auto b = tree_distance_pair_sums<q>(std::vector<Modint<q>>(n, 1), edges);
    const auto inverse = Modint<q>{p}.inv();
    std::vector<long long> answer(n);
    for(int i = 0; i < n; ++i) answer[i] = a[i].val() + p * ((b[i] - Modint<q>{a[i].val()}) * inverse).val();
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DISTANCE_HISTOGRAM_HPP_INCLUDED
