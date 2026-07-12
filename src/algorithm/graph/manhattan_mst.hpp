#pragma once

#include <algorithm>
#include <cstdlib>
#include <map>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>

template<class T>
struct ManhattanMstPoint{
    T x;
    T y;
};

template<class T>
struct ManhattanMstEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct ManhattanMstResult{
    T cost;
    std::vector<ManhattanMstEdge<T>> edges;
};

namespace manhattan_mst_internal{

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

} // namespace manhattan_mst_internal

template<class T>
ManhattanMstResult<T> manhattan_mst(std::vector<ManhattanMstPoint<T>> points){
    const int n = static_cast<int>(points.size());
    std::vector<int> order(static_cast<std::size_t>(n));
    std::iota(order.begin(), order.end(), 0);
    std::vector<ManhattanMstEdge<T>> candidates;
    candidates.reserve(static_cast<std::size_t>(4 * n));

    for(int flip = 0; flip < 2; flip++){
        for(int swp = 0; swp < 2; swp++){
            std::sort(order.begin(), order.end(), [&](int a, int b){
                T lhs = points[static_cast<std::size_t>(a)].x + points[static_cast<std::size_t>(a)].y;
                T rhs = points[static_cast<std::size_t>(b)].x + points[static_cast<std::size_t>(b)].y;
                if(lhs != rhs) return lhs < rhs;
                return a < b;
            });
            std::map<T, int> sweep;
            for(int i: order){
                const auto& p = points[static_cast<std::size_t>(i)];
                for(auto it = sweep.lower_bound(-p.y); it != sweep.end(); ){
                    int j = it->second;
                    const auto& q = points[static_cast<std::size_t>(j)];
                    if(p.x - q.x < p.y - q.y) break;
                    T cost = std::abs(p.x - q.x) + std::abs(p.y - q.y);
                    candidates.push_back({i, j, cost});
                    it = sweep.erase(it);
                }
                sweep[-p.y] = i;
            }
            for(auto& p: points) std::swap(p.x, p.y);
        }
        for(auto& p: points) p.x = -p.x;
    }

    std::stable_sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b){
        return a.cost < b.cost;
    });
    manhattan_mst_internal::Dsu dsu(n);
    ManhattanMstResult<T> result;
    result.cost = T(0);
    for(const auto& e: candidates){
        if(dsu.merge(e.from, e.to)){
            result.cost += e.cost;
            result.edges.push_back(e);
        }
    }
    return result;
}
