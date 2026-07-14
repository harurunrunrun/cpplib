#pragma once

#include <stdexcept>
#include <vector>
#include "strongly_connected_components.hpp"

struct TwoSat{
    int n;
    std::vector<std::vector<int>> graph;
    std::vector<int> answer;

    explicit TwoSat(int n_): n(n_), graph(static_cast<std::size_t>(2 * n_)), answer(static_cast<std::size_t>(n_), 0){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (TwoSat).");
        }
    }

    int id(int variable, bool value) const{
        if(variable < 0 || n <= variable)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (TwoSat::id).");
        }
        return 2 * variable + static_cast<int>(value);
    }

    void add_implication(int from_variable, bool from_value, int to_variable, bool to_value){
        const int from = id(from_variable, from_value);
        const int to = id(to_variable, to_value);
        graph[static_cast<std::size_t>(from)].push_back(to);
        graph[static_cast<std::size_t>(to ^ 1)].push_back(from ^ 1);
    }

    void add_clause(int i, bool f, int j, bool g){
        int a = id(i, f);
        int b = id(j, g);
        graph[static_cast<std::size_t>(a ^ 1)].push_back(b);
        graph[static_cast<std::size_t>(b ^ 1)].push_back(a);
    }

    bool satisfiable(){
        auto scc = strongly_connected_components(graph);
        for(int i = 0; i < n; i++){
            if(scc.id[static_cast<std::size_t>(2 * i)] == scc.id[static_cast<std::size_t>(2 * i + 1)]){
                return false;
            }
            answer[static_cast<std::size_t>(i)] =
                scc.id[static_cast<std::size_t>(2 * i)] < scc.id[static_cast<std::size_t>(2 * i + 1)];
        }
        return true;
    }
};
