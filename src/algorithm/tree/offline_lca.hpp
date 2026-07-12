#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

namespace tree_algorithm_detail{

struct OfflineLcaDsu{
    std::vector<int> parent;
    std::vector<int> size;

    explicit OfflineLcaDsu(int n): parent(n), size(n, 1){
        for(int i = 0; i < n; i++){
            parent[i] = i;
        }
    }

    int root(int v){
        while(parent[v] != v){
            parent[v] = parent[parent[v]];
            v = parent[v];
        }
        return v;
    }

    void unite(int u, int v){
        u = root(u);
        v = root(v);
        if(u == v){
            return;
        }
        if(size[u] < size[v]){
            std::swap(u, v);
        }
        parent[v] = u;
        size[u] += size[v];
    }
};

}

std::vector<int> offline_lca(
    const std::vector<std::vector<int>>& graph,
    const std::vector<std::pair<int, int>>& queries,
    int root = 0
){
    const int n = (int)graph.size();
    if(n == 0){
        if(!queries.empty())[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (offline_lca).");
        }
        return {};
    }
    if(root < 0 || n <= root)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (offline_lca).");
    }

    long long edge_twice = 0;
    for(int v = 0; v < n; v++){
        edge_twice += (int)graph[v].size();
    }
    if(edge_twice != 2LL * (n - 1))[[unlikely]]{
        throw std::runtime_error("library assertion fault: graph is not a tree (offline_lca).");
    }

    std::vector<int> parent(n, -1);
    std::vector<int> order;
    order.reserve(n);
    std::vector<int> stack = {root};
    parent[root] = -2;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        order.push_back(v);
        for(int to: graph[v]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (offline_lca).");
            }
            if(to == parent[v]){
                continue;
            }
            if(parent[to] != -1)[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph is not a tree (offline_lca).");
            }
            parent[to] = v;
            stack.push_back(to);
        }
    }
    if((int)order.size() != n)[[unlikely]]{
        throw std::runtime_error("library assertion fault: graph is not connected (offline_lca).");
    }
    parent[root] = -1;

    std::vector<std::vector<int>> children(n);
    for(int v = 0; v < n; v++){
        if(parent[v] != -1){
            children[parent[v]].push_back(v);
        }
    }

    std::vector<std::vector<std::pair<int, int>>> query_at(n);
    for(int i = 0; i < (int)queries.size(); i++){
        auto [u, v] = queries[i];
        if(u < 0 || n <= u || v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (offline_lca).");
        }
        query_at[u].push_back({v, i});
        if(u != v){
            query_at[v].push_back({u, i});
        }
    }

    tree_algorithm_detail::OfflineLcaDsu dsu(n);
    std::vector<int> ancestor(n);
    std::vector<int> answer(queries.size(), -1);
    std::vector<char> visited(n, false), black(n, false);

    struct Frame{
        int v;
        int next;
    };
    std::vector<Frame> dfs = {{root, 0}};
    visited[root] = true;
    ancestor[dsu.root(root)] = root;
    while(!dfs.empty()){
        Frame& frame = dfs.back();
        int v = frame.v;
        if(frame.next < (int)children[v].size()){
            int to = children[v][frame.next++];
            visited[to] = true;
            ancestor[dsu.root(to)] = to;
            dfs.push_back({to, 0});
            continue;
        }

        black[v] = true;
        for(auto [to, id]: query_at[v]){
            if(black[to]){
                answer[id] = ancestor[dsu.root(to)];
            }
        }
        dfs.pop_back();
        if(parent[v] != -1){
            dsu.unite(parent[v], v);
            ancestor[dsu.root(parent[v])] = parent[v];
        }
    }

    (void)visited;
    return answer;
}
