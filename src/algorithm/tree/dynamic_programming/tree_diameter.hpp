#ifndef CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DIAMETER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DIAMETER_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

struct TreeDiameterResult{
    int from;
    int to;
    long long length;
    std::vector<int> path;
};

template<class T>
struct WeightedTreeDiameterResult{
    int from;
    int to;
    T length;
    std::vector<int> path;
};

namespace tree_algorithm_detail{

inline void check_unweighted_tree(const std::vector<std::vector<int>>& graph, const char* name){
    const int n = (int)graph.size();
    long long edge_twice = 0;
    for(int v = 0; v < n; v++){
        edge_twice += (int)graph[v].size();
        for(int to: graph[v]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error(name);
            }
        }
    }
    if(n > 0 && edge_twice != 2LL * (n - 1))[[unlikely]]{
        throw std::runtime_error(name);
    }
}

template<class T>
void check_weighted_tree(const std::vector<std::vector<std::pair<int, T>>>& graph, const char* name){
    const int n = (int)graph.size();
    long long edge_twice = 0;
    for(int v = 0; v < n; v++){
        edge_twice += (int)graph[v].size();
        for(auto [to, weight]: graph[v]){
            (void)weight;
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error(name);
            }
        }
    }
    if(n > 0 && edge_twice != 2LL * (n - 1))[[unlikely]]{
        throw std::runtime_error(name);
    }
}

inline std::pair<int, long long> farthest_unweighted(
    const std::vector<std::vector<int>>& graph,
    int start,
    std::vector<int>* parent
){
    const int n = (int)graph.size();
    std::vector<long long> dist(n, -1);
    if(parent != nullptr){
        parent->assign(n, -1);
    }
    std::vector<int> stack = {start};
    dist[start] = 0;
    int farthest = start;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        if(dist[farthest] < dist[v]){
            farthest = v;
        }
        for(int to: graph[v]){
            if(dist[to] != -1){
                continue;
            }
            dist[to] = dist[v] + 1;
            if(parent != nullptr){
                (*parent)[to] = v;
            }
            stack.push_back(to);
        }
    }
    for(long long d: dist){
        if(d == -1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not connected (tree_diameter).");
        }
    }
    return {farthest, dist[farthest]};
}

template<class T>
std::pair<int, T> farthest_weighted(
    const std::vector<std::vector<std::pair<int, T>>>& graph,
    int start,
    std::vector<int>* parent
){
    const int n = (int)graph.size();
    std::vector<char> seen(n, false);
    std::vector<T> dist(n, T{});
    if(parent != nullptr){
        parent->assign(n, -1);
    }
    std::vector<int> stack = {start};
    seen[start] = true;
    int farthest = start;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        if(dist[farthest] < dist[v]){
            farthest = v;
        }
        for(auto [to, weight]: graph[v]){
            if(seen[to]){
                continue;
            }
            seen[to] = true;
            dist[to] = dist[v] + weight;
            if(parent != nullptr){
                (*parent)[to] = v;
            }
            stack.push_back(to);
        }
    }
    for(char ok: seen){
        if(!ok)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not connected (weighted_tree_diameter).");
        }
    }
    return {farthest, dist[farthest]};
}

inline std::vector<int> restore_path(int from, int to, const std::vector<int>& parent){
    std::vector<int> path;
    for(int v = to; v != -1; v = parent[v]){
        path.push_back(v);
        if(v == from){
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    return path;
}

}

inline TreeDiameterResult tree_diameter(const std::vector<std::vector<int>>& graph){
    const int n = (int)graph.size();
    if(n == 0){
        return {-1, -1, 0, {}};
    }
    tree_algorithm_detail::check_unweighted_tree(
        graph,
        "library assertion fault: graph is not a tree (tree_diameter)."
    );
    int s = tree_algorithm_detail::farthest_unweighted(graph, 0, nullptr).first;
    std::vector<int> parent;
    auto [t, length] = tree_algorithm_detail::farthest_unweighted(graph, s, &parent);
    return {s, t, length, tree_algorithm_detail::restore_path(s, t, parent)};
}

template<class T>
WeightedTreeDiameterResult<T> weighted_tree_diameter(
    const std::vector<std::vector<std::pair<int, T>>>& graph
){
    const int n = (int)graph.size();
    if(n == 0){
        return {-1, -1, T{}, {}};
    }
    tree_algorithm_detail::check_weighted_tree(
        graph,
        "library assertion fault: graph is not a tree (weighted_tree_diameter)."
    );
    int s = tree_algorithm_detail::farthest_weighted(graph, 0, nullptr).first;
    std::vector<int> parent;
    auto [t, length] = tree_algorithm_detail::farthest_weighted(graph, s, &parent);
    return {s, t, length, tree_algorithm_detail::restore_path(s, t, parent)};
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DIAMETER_HPP_INCLUDED
