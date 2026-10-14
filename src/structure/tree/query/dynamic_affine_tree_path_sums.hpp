#ifndef CPPLIB_SRC_STRUCTURE_TREE_QUERY_DYNAMIC_AFFINE_TREE_PATH_SUMS_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_QUERY_DYNAMIC_AFFINE_TREE_PATH_SUMS_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../../algorithm/tree/decomposition/heavy_light_decomposition.hpp"
#include "../../../algorithm/tree/dynamic_programming/affine_path_sums.hpp"

// Dynamic rerooting DP on a fixed tree. Both traversal directions of an edge
// use the same affine map. Multipliers may be zero: no inversion is needed.
template<class T>
class DynamicAffineTreePathSums{
    struct Aggregate{ T down_a{1}, down_b{}, up_a{1}, up_b{}; };
    int n, base = 1;
    HeavyLightDecomposition hld;
    std::vector<T> values, light, head_value, mass, subtree_mass;
    std::vector<AffineTreeEdge<T>> edges;
    std::vector<int> parent_edge, edge_child, heavy, chain_end;
    std::vector<Aggregate> tree;
    static Aggregate combine(const Aggregate& l, const Aggregate& r){
        return {l.down_a * r.down_a, l.down_a * r.down_b + l.down_b,
                r.up_a * l.up_a, r.up_a * l.up_b + r.up_b};
    }
    Aggregate make(int v) const{
        int h = heavy[v];
        if(h == -1) return {T{}, light[v], T{1}, T{}};
        const auto& e = edges[parent_edge[h]];
        return {e.multiplier, light[v] + e.addend * subtree_mass[h],
                e.multiplier, e.multiplier * light[v] + e.addend * (subtree_mass[0] - subtree_mass[h])};
    }
    void set(int v){
        int p = hld.in(v) + base; tree[p] = make(v);
        while((p >>= 1) != 0) tree[p] = combine(tree[2*p], tree[2*p+1]);
    }
    Aggregate prod(int l, int r) const{
        Aggregate a, b;
        for(l += base, r += base; l < r; l >>= 1, r >>= 1){
            if(l & 1) a = combine(a, tree[l++]);
            if(r & 1) b = combine(tree[--r], b);
        }
        return combine(a, b);
    }
    T down(int v) const{ return prod(hld.in(v), chain_end[hld.head(v)]).down_b; }
    void propagate(int v){
        while(true){
            int h = hld.head(v); T old = head_value[h];
            set(v); T now = down(h); head_value[h] = now;
            int p = hld.parent(h); if(p == -1) break;
            light[p] += edges[parent_edge[h]].multiplier * (now - old); v = p;
        }
    }
    void check_vertex(int v) const{ if(v < 0 || v >= n) throw std::out_of_range("dynamic affine tree vertex"); }
public:
    DynamicAffineTreePathSums(const std::vector<T>& a, const std::vector<AffineTreeEdge<T>>& initial_edges,
                             const std::vector<T>& vertex_mass = {}):
        n(static_cast<int>(a.size())), hld(n), values(a), light(a), head_value(n),
        mass(vertex_mass.empty() ? std::vector<T>(a.size(), T{1}) : vertex_mass), subtree_mass(mass),
        edges(initial_edges), parent_edge(n, -1), edge_child(edges.size()), heavy(n, -1), chain_end(n){
        if(mass.size() != a.size()) throw std::invalid_argument("dynamic affine tree mass size");
        if(static_cast<int>(edges.size()) != (n ? n-1 : 0)) throw std::invalid_argument("dynamic affine tree edge count");
        for(const auto& e: edges) hld.add_edge(e.u, e.v);
        hld.build();
        while(base < n) base <<= 1;
        tree.resize(2 * base);
        for(int e = 0; e < static_cast<int>(edges.size()); ++e){
            int v = hld.parent(edges[e].u) == edges[e].v ? edges[e].u : edges[e].v;
            parent_edge[v] = e; edge_child[e] = v;
            int p = hld.parent(v); if(hld.head(p) == hld.head(v)) heavy[p] = v;
        }
        for(int i = 0; i < n; ++i){ int v = hld[i]; chain_end[hld.head(v)] = i + 1; }
        std::vector<T> dp(n);
        for(int v = 0; v < n; ++v) dp[v] = light[v] = a[v] * mass[v];
        for(int i = n - 1; i > 0; --i){
            int v = hld[i], p = hld.parent(v); const auto& e = edges[parent_edge[v]];
            const T contribution = e.multiplier * dp[v] + e.addend * subtree_mass[v];
            dp[p] += contribution; if(heavy[p] != v) light[p] += contribution;
            subtree_mass[p] += subtree_mass[v];
        }
        for(int v = 0; v < n; ++v){ tree[base + hld.in(v)] = make(v); head_value[v] = dp[v]; }
        for(int p = base - 1; p; --p) tree[p] = combine(tree[2*p], tree[2*p+1]);
    }
    int size() const{ return n; }
    void vertex_set(int v, const T& x){
        check_vertex(v); light[v] += mass[v] * (x - values[v]); values[v] = x; propagate(v);
    }
    void edge_set(int id, const T& multiplier, const T& addend){
        if(id < 0 || id >= static_cast<int>(edges.size())) throw std::out_of_range("dynamic affine tree edge");
        const int v = edge_child[id], p = hld.parent(v);
        if(heavy[p] != v) light[p] += (multiplier - edges[id].multiplier) * head_value[v]
                                      + (addend - edges[id].addend) * subtree_mass[v];
        edges[id].multiplier = multiplier; edges[id].addend = addend; propagate(p);
    }
    T path_sum(int root = 0) const{
        check_vertex(root);
        std::vector<std::pair<int, int>> chains;
        for(int v = root; v != -1; v = hld.parent(hld.head(v))) chains.emplace_back(hld.head(v), v);
        std::reverse(chains.begin(), chains.end());
        T outside{};
        int previous = -1;
        for(auto [h, v]: chains){
            if(previous != -1){
                const auto& e = edges[parent_edge[h]];
                const T excluding = outside + down(previous) - e.multiplier * head_value[h] - e.addend * subtree_mass[h];
                outside = e.multiplier * excluding + e.addend * (subtree_mass[0] - subtree_mass[h]);
            }
            const auto transform = prod(hld.in(h), hld.in(v));
            outside = transform.up_a * outside + transform.up_b; previous = v;
        }
        return outside + down(root);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_QUERY_DYNAMIC_AFFINE_TREE_PATH_SUMS_HPP_INCLUDED
