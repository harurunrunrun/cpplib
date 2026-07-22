#ifndef CPPLIB_SRC_ALGORITHM_TREE_TREE_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_TREE_CENTROID_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <vector>

inline std::vector<int> tree_centroids(const std::vector<std::vector<int>>& graph){
    const int n = (int)graph.size();
    if(n == 0){
        return {};
    }
    long long edge_twice = 0;
    for(int v = 0; v < n; v++){
        edge_twice += (int)graph[v].size();
        for(int to: graph[v]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (tree_centroids).");
            }
        }
    }
    if(edge_twice != 2LL * (n - 1))[[unlikely]]{
        throw std::runtime_error("library assertion fault: graph is not a tree (tree_centroids).");
    }

    std::vector<int> parent(n, -1);
    std::vector<int> order;
    order.reserve(n);
    std::vector<int> stack = {0};
    parent[0] = -2;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        order.push_back(v);
        for(int to: graph[v]){
            if(to == parent[v]){
                continue;
            }
            if(parent[to] != -1)[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph is not a tree (tree_centroids).");
            }
            parent[to] = v;
            stack.push_back(to);
        }
    }
    if((int)order.size() != n)[[unlikely]]{
        throw std::runtime_error("library assertion fault: graph is not connected (tree_centroids).");
    }
    parent[0] = -1;

    std::vector<int> subtree(n, 1);
    for(auto it = order.rbegin(); it != order.rend(); ++it){
        int v = *it;
        for(int to: graph[v]){
            if(parent[to] == v){
                subtree[v] += subtree[to];
            }
        }
    }

    std::vector<int> res;
    for(int v = 0; v < n; v++){
        int largest = n - subtree[v];
        for(int to: graph[v]){
            if(parent[to] == v){
                largest = std::max(largest, subtree[to]);
            }
        }
        if(largest * 2 <= n){
            res.push_back(v);
        }
    }
    return res;
}

inline int tree_centroid(const std::vector<std::vector<int>>& graph){
    auto centroids = tree_centroids(graph);
    if(centroids.empty()){
        return -1;
    }
    return centroids[0];
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_TREE_CENTROID_HPP_INCLUDED
