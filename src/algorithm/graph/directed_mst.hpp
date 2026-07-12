#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

template<class T>
struct DirectedMstEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct DirectedMstResult{
    T cost;
    bool exists;
};

template<class T>
DirectedMstResult<T> directed_mst(
    int n,
    int root,
    std::vector<DirectedMstEdge<T>> edges,
    T inf
){
    if(n <= 0 || root < 0 || n <= root)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (directed_mst).");
    }
    for(const auto& e: edges){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (directed_mst).");
        }
    }

    DirectedMstResult<T> result{T(0), true};
    while(true){
        std::vector<T> in(static_cast<std::size_t>(n), inf);
        std::vector<int> parent(static_cast<std::size_t>(n), -1);
        for(const auto& e: edges){
            if(e.from != e.to && e.cost < in[static_cast<std::size_t>(e.to)]){
                in[static_cast<std::size_t>(e.to)] = e.cost;
                parent[static_cast<std::size_t>(e.to)] = e.from;
            }
        }
        in[static_cast<std::size_t>(root)] = T(0);
        for(int v = 0; v < n; v++){
            if(in[static_cast<std::size_t>(v)] == inf){
                result.exists = false;
                return result;
            }
        }

        int component_count = 0;
        std::vector<int> component(static_cast<std::size_t>(n), -1);
        std::vector<int> visited(static_cast<std::size_t>(n), -1);
        for(int v = 0; v < n; v++) result.cost += in[static_cast<std::size_t>(v)];

        for(int i = 0; i < n; i++){
            int v = i;
            while(visited[static_cast<std::size_t>(v)] != i &&
                  component[static_cast<std::size_t>(v)] == -1 &&
                  v != root){
                visited[static_cast<std::size_t>(v)] = i;
                v = parent[static_cast<std::size_t>(v)];
            }
            if(v != root && component[static_cast<std::size_t>(v)] == -1){
                for(int u = parent[static_cast<std::size_t>(v)]; u != v; u = parent[static_cast<std::size_t>(u)]){
                    component[static_cast<std::size_t>(u)] = component_count;
                }
                component[static_cast<std::size_t>(v)] = component_count++;
            }
        }
        if(component_count == 0) return result;
        for(int v = 0; v < n; v++){
            if(component[static_cast<std::size_t>(v)] == -1){
                component[static_cast<std::size_t>(v)] = component_count++;
            }
        }

        std::vector<DirectedMstEdge<T>> next_edges;
        next_edges.reserve(edges.size());
        for(const auto& e: edges){
            int from = component[static_cast<std::size_t>(e.from)];
            int to = component[static_cast<std::size_t>(e.to)];
            if(from != to){
                next_edges.push_back({from, to, e.cost - in[static_cast<std::size_t>(e.to)]});
            }
        }
        root = component[static_cast<std::size_t>(root)];
        n = component_count;
        edges.swap(next_edges);
    }
}
