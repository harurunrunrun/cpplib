#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_KRUSKAL_MST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_KRUSKAL_MST_HPP_INCLUDED

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

template<class T>
struct KruskalEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct KruskalMstResult{
    T cost;
    std::vector<int> edge_ids;
    bool connected;
};

namespace kruskal_mst_internal{

struct Dsu{
    std::vector<int> parent;
    std::vector<int> size;

    explicit Dsu(int n): parent(static_cast<std::size_t>(n)), size(static_cast<std::size_t>(n), 1){
        std::iota(parent.begin(), parent.end(), 0);
    }

    int leader(int x){
        while(parent[static_cast<std::size_t>(x)] != x){
            x = parent[static_cast<std::size_t>(x)] =
                parent[static_cast<std::size_t>(parent[static_cast<std::size_t>(x)])];
        }
        return x;
    }

    bool merge(int a, int b){
        a = leader(a);
        b = leader(b);
        if(a == b) return false;
        if(size[static_cast<std::size_t>(a)] < size[static_cast<std::size_t>(b)]) std::swap(a, b);
        parent[static_cast<std::size_t>(b)] = a;
        size[static_cast<std::size_t>(a)] += size[static_cast<std::size_t>(b)];
        return true;
    }
};

} // namespace kruskal_mst_internal

template<class T>
KruskalMstResult<T> kruskal_mst(int n, const std::vector<KruskalEdge<T>>& edges){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (kruskal_mst).");
    }
    for(const auto& e: edges){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kruskal_mst).");
        }
    }

    std::vector<int> order(edges.size());
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(order.begin(), order.end(), [&](int a, int b){
        return edges[static_cast<std::size_t>(a)].cost < edges[static_cast<std::size_t>(b)].cost;
    });

    kruskal_mst_internal::Dsu dsu(n);
    KruskalMstResult<T> result;
    result.cost = T(0);
    result.connected = true;
    for(int id: order){
        const auto& e = edges[static_cast<std::size_t>(id)];
        if(dsu.merge(e.from, e.to)){
            result.cost += e.cost;
            result.edge_ids.push_back(id);
        }
    }
    result.connected = static_cast<int>(result.edge_ids.size()) == std::max(0, n - 1);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_KRUSKAL_MST_HPP_INCLUDED
