#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_MAXIMUM_CLIQUE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_MAXIMUM_CLIQUE_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace maximum_clique_internal{

struct Solver{
    int n;
    std::vector<std::vector<char>> adjacent;
    std::vector<int> clique;
    std::vector<int> best;

    void color_sort(std::vector<int>& vertices, std::vector<int>& colors){
        std::vector<int> order;
        std::vector<int> color_bound;
        std::vector<int> remaining = vertices;
        int color = 0;
        while(!remaining.empty()){
            color++;
            std::vector<int> next_remaining;
            std::vector<int> independent;
            for(int v: remaining){
                bool ok = true;
                for(int u: independent){
                    if(adjacent[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)]){
                        ok = false;
                        break;
                    }
                }
                if(ok){
                    independent.push_back(v);
                    order.push_back(v);
                    color_bound.push_back(color);
                }else{
                    next_remaining.push_back(v);
                }
            }
            remaining.swap(next_remaining);
        }
        vertices.swap(order);
        colors.swap(color_bound);
    }

    void expand(std::vector<int> vertices, std::vector<int> colors){
        while(!vertices.empty()){
            if(static_cast<int>(clique.size()) + colors.back() <= static_cast<int>(best.size())) return;
            int v = vertices.back();
            vertices.pop_back();
            colors.pop_back();
            clique.push_back(v);

            std::vector<int> next_vertices;
            for(int u: vertices){
                if(adjacent[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)]){
                    next_vertices.push_back(u);
                }
            }
            if(next_vertices.empty()){
                if(best.size() < clique.size()) best = clique;
            }else{
                std::vector<int> next_colors;
                color_sort(next_vertices, next_colors);
                expand(next_vertices, next_colors);
            }
            clique.pop_back();
        }
    }
};

} // namespace maximum_clique_internal

std::vector<int> maximum_clique(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    maximum_clique_internal::Solver solver;
    solver.n = n;
    solver.adjacent.assign(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (maximum_clique).");
            }
            if(to != v){
                solver.adjacent[static_cast<std::size_t>(v)][static_cast<std::size_t>(to)] = 1;
            }
        }
    }
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(solver.adjacent[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] !=
               solver.adjacent[static_cast<std::size_t>(j)][static_cast<std::size_t>(i)])[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph must be undirected (maximum_clique).");
            }
        }
    }

    std::vector<int> vertices(static_cast<std::size_t>(n));
    for(int i = 0; i < n; i++) vertices[static_cast<std::size_t>(i)] = i;
    std::vector<int> colors;
    solver.color_sort(vertices, colors);
    solver.expand(vertices, colors);
    std::sort(solver.best.begin(), solver.best.end());
    return solver.best;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_MAXIMUM_CLIQUE_HPP_INCLUDED
