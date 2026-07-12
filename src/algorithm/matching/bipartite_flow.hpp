#pragma once

#include <stdexcept>
#include <vector>
#include "dinic.hpp"

template<class T>
struct BipartiteFlowEdge{
    int left;
    int right;
    T cap;
};

template<class T>
struct BipartiteFlowResult{
    T flow;
    std::vector<DinicEdge<T>> edges;
};

template<class T>
struct BipartiteFlow{
    int left_size;
    int right_size;
    std::vector<T> left_cap;
    std::vector<T> right_cap;
    std::vector<BipartiteFlowEdge<T>> edges;

    BipartiteFlow(int left_size_, int right_size_)
        : left_size(left_size_),
          right_size(right_size_),
          left_cap(static_cast<std::size_t>(left_size_), T(0)),
          right_cap(static_cast<std::size_t>(right_size_), T(0)){
        if(left_size < 0 || right_size < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BipartiteFlow).");
        }
    }

    void add_left_capacity(int left, T cap){
        if(left < 0 || left_size <= left || cap < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BipartiteFlow::add_left_capacity).");
        }
        left_cap[static_cast<std::size_t>(left)] += cap;
    }

    void add_right_capacity(int right, T cap){
        if(right < 0 || right_size <= right || cap < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BipartiteFlow::add_right_capacity).");
        }
        right_cap[static_cast<std::size_t>(right)] += cap;
    }

    void add_edge(int left, int right, T cap){
        if(left < 0 || left_size <= left || right < 0 || right_size <= right || cap < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BipartiteFlow::add_edge).");
        }
        edges.push_back({left, right, cap});
    }

    BipartiteFlowResult<T> max_flow(){
        int source = left_size + right_size;
        int sink = source + 1;
        Dinic<T> graph(sink + 1);
        for(int i = 0; i < left_size; i++){
            graph.add_edge(source, i, left_cap[static_cast<std::size_t>(i)]);
        }
        for(const auto& e: edges){
            graph.add_edge(e.left, left_size + e.right, e.cap);
        }
        for(int i = 0; i < right_size; i++){
            graph.add_edge(left_size + i, sink, right_cap[static_cast<std::size_t>(i)]);
        }
        T flow = graph.max_flow(source, sink);
        return {flow, graph.edges};
    }
};
