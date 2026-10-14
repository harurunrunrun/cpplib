#ifndef CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_MINIMUM_INVERSION_TREE_ORDER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_MINIMUM_INVERSION_TREE_ORDER_HPP_INCLUDED

#include <cstdint>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct MinimumInversionTreeOrderResult{
    std::vector<int> order;
    __int128 cost = 0;
};

// Minimize sum_{j<i} c[order[i]]*d[order[j]] with parents before children.
inline MinimumInversionTreeOrderResult minimum_inversion_tree_order(
    const std::vector<int>& parent, const std::vector<std::int64_t>& c,
    const std::vector<std::int64_t>& d){
    const int n = static_cast<int>(parent.size());
    if(c.size() != parent.size() || d.size() != parent.size()) throw std::invalid_argument("tree order weights size");
    MinimumInversionTreeOrderResult result;
    if(n == 0) return result;
    int root = -1;
    std::vector<std::vector<int>> children(n);
    for(int v = 0; v < n; ++v){
        if(c[v] < 0 || d[v] < 0) throw std::invalid_argument("negative tree order weight");
        if(parent[v] == -1){ if(root != -1) throw std::invalid_argument("multiple tree order roots"); root = v; }
        else{
            if(parent[v] < 0 || parent[v] >= n) throw std::out_of_range("tree order parent");
            children[parent[v]].push_back(v);
        }
    }
    if(root == -1) throw std::invalid_argument("missing tree order root");
    std::vector<int> visit{root};
    for(int i = 0; i < static_cast<int>(visit.size()); ++i) for(int v: children[visit[i]]) visit.push_back(v);
    if(static_cast<int>(visit.size()) != n) throw std::invalid_argument("tree order cycle");
    struct Item{ __int128 c, d; int v, generation; };
    struct Less{
        bool operator()(const Item& a, const Item& b) const{
            const bool az = a.c == 0 && a.d == 0, bz = b.c == 0 && b.d == 0;
            if(az != bz) return az;
            const auto x = a.c * b.d, y = b.c * a.d;
            if(x != y) return x < y;
            return a.v > b.v;
        }
    };
    std::priority_queue<Item, std::vector<Item>, Less> queue;
    std::vector<__int128> cs(c.begin(), c.end()), ds(d.begin(), d.end());
    std::vector<int> uf(n, -1), label(n), tail(n), next(n, -1), generation(n);
    std::iota(label.begin(), label.end(), 0); std::iota(tail.begin(), tail.end(), 0);
    auto find = [&](int v){
        int r = v; while(uf[r] >= 0) r = uf[r];
        while(v != r){ int p = uf[v]; uf[v] = r; v = p; }
        return r;
    };
    for(int v = 0; v < n; ++v) if(v != root) queue.push({cs[v], ds[v], v, 0});
    while(!queue.empty()){
        const auto item = queue.top(); queue.pop(); const int v = item.v;
        if(item.generation != generation[v] || label[find(v)] != v) continue;
        int a = find(v), b = find(parent[v]); const int p = label[b];
        next[tail[p]] = v; tail[p] = tail[v]; cs[p] += cs[v]; ds[p] += ds[v];
        if(uf[a] < uf[b]) std::swap(a, b);
        uf[b] += uf[a]; uf[a] = b; label[b] = p;
        ++generation[p];
        if(p != root) queue.push({cs[p], ds[p], p, generation[p]});
    }
    __int128 prefix = 0;
    for(int v = root; v != -1; v = next[v]){
        result.order.push_back(v); result.cost += prefix * c[v]; prefix += d[v];
    }
    return result;
}

inline MinimumInversionTreeOrderResult minimum_inversion_forest_order(
    const std::vector<int>& parent, const std::vector<std::int64_t>& c,
    const std::vector<std::int64_t>& d){
    if(c.size() != parent.size() || d.size() != parent.size()) throw std::invalid_argument("forest order weights size");
    std::vector<int> augmented_parent(parent.size()+1, -1);
    std::vector<std::int64_t> augmented_c(c.size()+1), augmented_d(d.size()+1);
    for(int v=0;v<static_cast<int>(parent.size());++v){
        if(parent[v] < -1 || parent[v] >= static_cast<int>(parent.size())) throw std::out_of_range("forest order parent");
        augmented_parent[v+1]=parent[v]+1; augmented_c[v+1]=c[v]; augmented_d[v+1]=d[v];
    }
    auto result=minimum_inversion_tree_order(augmented_parent,augmented_c,augmented_d);
    result.order.erase(result.order.begin());
    for(int& v:result.order)--v;
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_MINIMUM_INVERSION_TREE_ORDER_HPP_INCLUDED
