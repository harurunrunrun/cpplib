#ifndef CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_TREE_AUTOMORPHISM_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_TREE_AUTOMORPHISM_COUNT_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
T rooted_tree_automorphism_count(const std::vector<std::vector<int>>& graph, int root){
    const int n = static_cast<int>(graph.size());
    if(root < 0 || root >= n) throw std::out_of_range("rooted_tree_automorphism_count: invalid root");
    std::vector<int> parent(n, -1), order{root};
    parent[root] = root;
    for(std::size_t i = 0; i < order.size(); ++i){
        const int v = order[i];
        for(const int to: graph[v]) if(parent[to] == -1){
            parent[to] = v;
            order.push_back(to);
        }
    }
    if(static_cast<int>(order.size()) != n) throw std::invalid_argument("rooted_tree_automorphism_count: disconnected graph");
    std::map<std::vector<int>, int> ids;
    std::vector<int> type(n);
    std::vector<T> count(n, T{1});
    for(int i = n - 1; i >= 0; --i){
        const int v = order[i];
        std::vector<int> children;
        for(const int to: graph[v]) if(parent[to] == v){
            children.push_back(type[to]);
            count[v] *= count[to];
        }
        std::sort(children.begin(), children.end());
        for(int l = 0; l < static_cast<int>(children.size()); ){
            int r = l + 1;
            while(r < static_cast<int>(children.size()) && children[r] == children[l]) ++r;
            for(int k = 2; k <= r - l; ++k) count[v] *= T{k};
            l = r;
        }
        const auto [it, inserted] = ids.emplace(children, static_cast<int>(ids.size()));
        (void)inserted;
        type[v] = it->second;
    }
    return count[root];
}

template<class T>
T tree_automorphism_count(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    if(n == 0) return T{1};
    std::vector<int> degree(n);
    std::vector<char> removed(n, false);
    std::vector<int> leaves;
    for(int v = 0; v < n; ++v){
        degree[v] = static_cast<int>(graph[v].size());
        if(degree[v] <= 1) leaves.push_back(v);
    }
    int remaining = n;
    while(remaining > 2){
        remaining -= static_cast<int>(leaves.size());
        std::vector<int> next;
        for(const int v: leaves){
            removed[v] = true;
            for(const int to: graph[v]) if(!removed[to] && --degree[to] == 1) next.push_back(to);
        }
        leaves = std::move(next);
    }
    if(remaining == 1) return rooted_tree_automorphism_count<T>(graph, leaves.front());
    const int a = leaves[0], b = leaves[1];
    std::vector<std::vector<int>> augmented = graph;
    augmented.push_back({a, b});
    augmented[a].push_back(n);
    augmented[b].push_back(n);
    augmented[a].erase(std::find(augmented[a].begin(), augmented[a].end(), b));
    augmented[b].erase(std::find(augmented[b].begin(), augmented[b].end(), a));
    return rooted_tree_automorphism_count<T>(augmented, n);
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_TREE_AUTOMORPHISM_COUNT_HPP_INCLUDED
