#pragma once

#include <stdexcept>
#include <vector>
#include "dinic.hpp"

template<class T>
struct BurnBuryResult{
    T cost;
    std::vector<int> assignment;
};

template<class T>
struct BurnBury{
    int n;
    int source;
    int sink;
    T constant;
    Dinic<T> graph;

    explicit BurnBury(int n_)
        : n(n_),
          source(n_),
          sink(n_ + 1),
          constant(T(0)),
          graph(n_ + 2){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BurnBury).");
        }
    }

    void add_one_cost(int v, T zero_cost, T one_cost){
        if(v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BurnBury::add_one_cost).");
        }
        constant += zero_cost;
        add_one_delta(v, one_cost - zero_cost);
    }

    void add_one_delta(int v, T delta){
        if(v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BurnBury::add_one_delta).");
        }
        if(delta >= T(0)){
            graph.add_edge(source, v, delta);
        }else{
            constant += delta;
            graph.add_edge(v, sink, -delta);
        }
    }

    void add_pair_cost(int i, int j, T cost00, T cost01, T cost10, T cost11){
        if(i < 0 || n <= i || j < 0 || n <= j)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BurnBury::add_pair_cost).");
        }
        T cut_cost = cost01 + cost10 - cost00 - cost11;
        if(cut_cost < T(0))[[unlikely]]{
            throw std::runtime_error("library assertion fault: pair cost must be submodular (BurnBury::add_pair_cost).");
        }
        constant += cost00;
        add_one_delta(i, cost10 - cost00);
        add_one_delta(j, cost11 - cost10);
        graph.add_edge(i, j, cut_cost);
    }

    BurnBuryResult<T> solve(){
        T flow = graph.max_flow(source, sink);
        auto reachable = graph.min_cut(source);
        BurnBuryResult<T> result;
        result.cost = constant + flow;
        result.assignment.assign(static_cast<std::size_t>(n), 0);
        for(int i = 0; i < n; i++){
            result.assignment[static_cast<std::size_t>(i)] = reachable[static_cast<std::size_t>(i)] ? 0 : 1;
        }
        return result;
    }
};
