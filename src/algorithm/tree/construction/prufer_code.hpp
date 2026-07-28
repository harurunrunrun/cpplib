#ifndef CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_PRUFER_CODE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_PRUFER_CODE_HPP_INCLUDED

#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<int> prufer_encode(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    if(n <= 2) return {};
    std::vector<int> degree(n);
    for(int v = 0; v < n; ++v) degree[v] = static_cast<int>(graph[v].size());
    std::priority_queue<int, std::vector<int>, std::greater<int>> leaves;
    for(int v = 0; v < n; ++v) if(degree[v] == 1) leaves.push(v);
    std::vector<int> code;
    code.reserve(n - 2);
    for(int step = 0; step < n - 2; ++step){
        if(leaves.empty()) throw std::invalid_argument("prufer_encode: graph is not a tree");
        const int leaf = leaves.top();
        leaves.pop();
        int parent = -1;
        for(const int to: graph[leaf]) if(degree[to] != 0){
            parent = to;
            break;
        }
        if(parent == -1) throw std::invalid_argument("prufer_encode: graph is not a tree");
        code.push_back(parent);
        degree[leaf] = 0;
        if(--degree[parent] == 1) leaves.push(parent);
    }
    return code;
}

inline std::vector<std::pair<int, int>> prufer_decode(const std::vector<int>& code){
    const int n = static_cast<int>(code.size()) + 2;
    std::vector<int> degree(n, 1);
    for(const int v: code){
        if(v < 0 || v >= n) throw std::out_of_range("prufer_decode: invalid vertex");
        ++degree[v];
    }
    std::priority_queue<int, std::vector<int>, std::greater<int>> leaves;
    for(int v = 0; v < n; ++v) if(degree[v] == 1) leaves.push(v);
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n - 1);
    for(const int v: code){
        const int leaf = leaves.top();
        leaves.pop();
        edges.emplace_back(leaf, v);
        --degree[leaf];
        if(--degree[v] == 1) leaves.push(v);
    }
    const int a = leaves.top();
    leaves.pop();
    edges.emplace_back(a, leaves.top());
    return edges;
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_PRUFER_CODE_HPP_INCLUDED
