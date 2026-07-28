#ifndef CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_RAKE_COMPRESS_TREE_CONTRACTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_RAKE_COMPRESS_TREE_CONTRACTION_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

struct TreeContractionStep{
    enum class Kind { rake, compress };
    Kind kind;
    int vertex;
    int left;
    int right;
};

inline std::vector<std::vector<TreeContractionStep>>
rake_compress_tree_contraction(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<int>> adjacent = graph;
    std::vector<char> alive(n, true);
    int remaining = n;
    std::vector<std::vector<TreeContractionStep>> rounds;
    while(remaining > 1){
        std::vector<int> degree(n);
        for(int v = 0; v < n; ++v) if(alive[v]){
            for(const int to: adjacent[v]){
                if(to < 0 || to >= n) throw std::out_of_range("rake_compress_tree_contraction: invalid edge");
                if(alive[to]) ++degree[v];
            }
        }
        std::vector<char> selected(n, false);
        std::vector<TreeContractionStep> steps;
        for(int v = 0; v < n; ++v){
            if(alive[v] && degree[v] <= 1 && remaining - static_cast<int>(steps.size()) > 1){
                int p = -1;
                for(const int to: adjacent[v]) if(alive[to]) p = to;
                selected[v] = true;
                steps.push_back({TreeContractionStep::Kind::rake, v, p, -1});
            }
        }
        for(int v = 0; v < n; ++v){
            if(!alive[v] || degree[v] != 2 || selected[v] || remaining - static_cast<int>(steps.size()) <= 1) continue;
            int a = -1, b = -1;
            for(const int to: adjacent[v]) if(alive[to]){
                if(a == -1) a = to;
                else b = to;
            }
            if(selected[a] || selected[b]) continue;
            selected[v] = true;
            steps.push_back({TreeContractionStep::Kind::compress, v, a, b});
        }
        if(steps.empty()) throw std::invalid_argument("rake_compress_tree_contraction: graph is not a tree");
        for(const auto& step: steps){
            alive[step.vertex] = false;
            --remaining;
            if(step.kind == TreeContractionStep::Kind::compress){
                adjacent[step.left].push_back(step.right);
                adjacent[step.right].push_back(step.left);
            }
        }
        rounds.push_back(std::move(steps));
    }
    return rounds;
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_RAKE_COMPRESS_TREE_CONTRACTION_HPP_INCLUDED
