#ifndef CPPLIB_SRC_ALGORITHM_TREE_TREE_ISOMORPHISM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_TREE_ISOMORPHISM_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace tree_isomorphism_detail{

inline void validate_tree(const std::vector<std::vector<int>>& graph, const char* message){
    const int n = (int)graph.size();
    if(n == 0){
        return;
    }

    long long edge_twice = 0;
    for(int v = 0; v < n; v++){
        edge_twice += (int)graph[v].size();
        for(int to: graph[v]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error(message);
            }
        }
    }
    if(edge_twice != 2LL * (n - 1))[[unlikely]]{
        throw std::runtime_error(message);
    }

    std::vector<int> parent(n, -1);
    std::vector<int> stack = {0};
    parent[0] = -2;
    int count = 0;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        count++;
        for(int to: graph[v]){
            if(to == parent[v]){
                continue;
            }
            if(parent[to] != -1)[[unlikely]]{
                throw std::runtime_error(message);
            }
            parent[to] = v;
            stack.push_back(to);
        }
    }
    if(count != n)[[unlikely]]{
        throw std::runtime_error(message);
    }
}

struct RootedTreeInfo{
    std::vector<std::vector<int>> children;
    std::vector<int> depth;
    std::vector<int> order;
    int max_depth = 0;
};

inline RootedTreeInfo build_rooted_tree(
    const std::vector<std::vector<int>>& graph,
    int root,
    const char* message
){
    const int n = (int)graph.size();
    if(root < 0 || n <= root)[[unlikely]]{
        throw std::runtime_error(message);
    }
    validate_tree(graph, message);

    RootedTreeInfo info;
    info.children.assign(n, {});
    info.depth.assign(n, 0);
    info.order.reserve(n);

    std::vector<int> parent(n, -1);
    std::vector<int> stack = {root};
    parent[root] = -2;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        info.order.push_back(v);
        info.max_depth = std::max(info.max_depth, info.depth[v]);
        for(int to: graph[v]){
            if(to == parent[v]){
                continue;
            }
            parent[to] = v;
            info.depth[to] = info.depth[v] + 1;
            info.children[v].push_back(to);
            stack.push_back(to);
        }
    }
    return info;
}

inline int find_label(std::map<std::vector<int>, int>& dictionary, const std::vector<int>& signature){
    auto it = dictionary.find(signature);
    if(it != dictionary.end()){
        return it->second;
    }
    const int id = (int)dictionary.size() + 1;
    dictionary.emplace(signature, id);
    return id;
}

inline std::vector<int> rooted_labels_from_info(
    const RootedTreeInfo& info,
    std::map<std::vector<int>, int>& dictionary,
    int max_depth
){
    const int n = (int)info.children.size();
    std::vector<std::vector<int>> by_depth(max_depth + 1);
    for(int v: info.order){
        by_depth[info.depth[v]].push_back(v);
    }

    std::vector<int> label(n, 0);
    for(int d = max_depth; d >= 0; d--){
        for(int v: by_depth[d]){
            std::vector<int> signature;
            signature.reserve(info.children[v].size());
            for(int to: info.children[v]){
                signature.push_back(label[to]);
            }
            std::sort(signature.begin(), signature.end());
            label[v] = find_label(dictionary, signature);
        }
    }
    return label;
}

inline std::pair<std::vector<int>, std::vector<int>> rooted_labels_pair(
    const RootedTreeInfo& a,
    const RootedTreeInfo& b
){
    const int max_depth = std::max(a.max_depth, b.max_depth);
    std::map<std::vector<int>, int> dictionary;
    return {
        rooted_labels_from_info(a, dictionary, max_depth),
        rooted_labels_from_info(b, dictionary, max_depth)
    };
}

}

inline std::vector<int> tree_centers(const std::vector<std::vector<int>>& graph){
    const int n = (int)graph.size();
    if(n == 0){
        return {};
    }
    tree_isomorphism_detail::validate_tree(
        graph,
        "library assertion fault: graph is not a tree (tree_centers)."
    );
    if(n <= 2){
        std::vector<int> res(n);
        for(int i = 0; i < n; i++){
            res[i] = i;
        }
        return res;
    }

    std::vector<int> degree(n), leaves;
    leaves.reserve(n);
    for(int v = 0; v < n; v++){
        degree[v] = (int)graph[v].size();
        if(degree[v] <= 1){
            leaves.push_back(v);
        }
    }

    int remain = n;
    while(remain > 2){
        remain -= (int)leaves.size();
        std::vector<int> next;
        for(int v: leaves){
            for(int to: graph[v]){
                if(--degree[to] == 1){
                    next.push_back(to);
                }
            }
        }
        leaves.swap(next);
    }
    std::sort(leaves.begin(), leaves.end());
    return leaves;
}

inline std::vector<int> rooted_tree_isomorphism_labels(
    const std::vector<std::vector<int>>& graph,
    int root = 0
){
    auto info = tree_isomorphism_detail::build_rooted_tree(
        graph,
        root,
        "library assertion fault: graph is not a tree (rooted_tree_isomorphism_labels)."
    );
    std::map<std::vector<int>, int> dictionary;
    return tree_isomorphism_detail::rooted_labels_from_info(info, dictionary, info.max_depth);
}

inline bool rooted_tree_isomorphic(
    const std::vector<std::vector<int>>& a,
    int root_a,
    const std::vector<std::vector<int>>& b,
    int root_b
){
    if(a.size() != b.size()){
        return false;
    }
    if(a.empty()){
        return true;
    }
    auto info_a = tree_isomorphism_detail::build_rooted_tree(
        a,
        root_a,
        "library assertion fault: graph is not a tree (rooted_tree_isomorphic)."
    );
    auto info_b = tree_isomorphism_detail::build_rooted_tree(
        b,
        root_b,
        "library assertion fault: graph is not a tree (rooted_tree_isomorphic)."
    );
    auto [label_a, label_b] = tree_isomorphism_detail::rooted_labels_pair(info_a, info_b);
    return label_a[root_a] == label_b[root_b];
}

inline bool tree_isomorphic(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b
){
    if(a.size() != b.size()){
        return false;
    }
    if(a.empty()){
        return true;
    }

    const auto center_a = tree_centers(a);
    const auto center_b = tree_centers(b);
    if(center_a.size() != center_b.size()){
        return false;
    }
    if(center_a.size() == 1){
        return rooted_tree_isomorphic(a, center_a[0], b, center_b[0]);
    }
    return rooted_tree_isomorphic(a, center_a[0], b, center_b[0])
        || rooted_tree_isomorphic(a, center_a[0], b, center_b[1]);
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_TREE_ISOMORPHISM_HPP_INCLUDED
