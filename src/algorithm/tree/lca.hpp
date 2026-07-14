#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

struct LowestCommonAncestor{
private:
    int _n;
    int _log;
    std::vector<std::vector<int>> graph;
    std::vector<int> parent_vertex;
    std::vector<int> depth_vertex;
    std::vector<int> up;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int table(int k, int v) const{
        return up[k * _n + v];
    }

public:
    explicit LowestCommonAncestor(int n = 0):
        _n(n),
        _log(1),
        graph(n),
        parent_vertex(n, -1),
        depth_vertex(n, 0){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        while((1 << _log) <= std::max(1, n)){
            _log++;
        }
        up.assign(_log * std::max(1, n), 0);
    }

    int size() const{
        return _n;
    }

    void add_edge(int u, int v){
        check_vertex(u, "library assertion fault: range violation (add_edge).");
        check_vertex(v, "library assertion fault: range violation (add_edge).");
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    void build(int root = 0){
        if(_n == 0){
            return;
        }
        check_vertex(root, "library assertion fault: range violation (build).");
        std::fill(parent_vertex.begin(), parent_vertex.end(), -1);
        std::fill(depth_vertex.begin(), depth_vertex.end(), 0);

        std::vector<int> order;
        order.reserve(_n);
        std::vector<int> stack = {root};
        parent_vertex[root] = -2;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            order.push_back(v);
            for(int to: graph[v]){
                if(to == parent_vertex[v]){
                    continue;
                }
                if(parent_vertex[to] != -1)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: graph is not a tree (build).");
                }
                parent_vertex[to] = v;
                depth_vertex[to] = depth_vertex[v] + 1;
                stack.push_back(to);
            }
        }
        if((int)order.size() != _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not connected (build).");
        }
        parent_vertex[root] = -1;

        for(int v: order){
            up[v] = parent_vertex[v] == -1 ? v : parent_vertex[v];
        }
        for(int k = 1; k < _log; k++){
            int base = k * _n;
            int prev = (k - 1) * _n;
            for(int v = 0; v < _n; v++){
                up[base + v] = up[prev + up[prev + v]];
            }
        }
    }

    int parent(int v) const{
        check_vertex(v, "library assertion fault: range violation (parent).");
        return parent_vertex[v];
    }

    int depth(int v) const{
        check_vertex(v, "library assertion fault: range violation (depth).");
        return depth_vertex[v];
    }

    int jump(int v, int k) const{
        check_vertex(v, "library assertion fault: range violation (jump).");
        if(k < 0 || depth_vertex[v] < k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (jump).");
        }
        for(int i = 0; k > 0; i++){
            if(k & 1){
                v = table(i, v);
            }
            k >>= 1;
        }
        return v;
    }

    int lca(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (lca).");
        check_vertex(v, "library assertion fault: range violation (lca).");
        if(depth_vertex[u] < depth_vertex[v]){
            std::swap(u, v);
        }
        u = jump(u, depth_vertex[u] - depth_vertex[v]);
        if(u == v){
            return u;
        }
        for(int k = _log - 1; k >= 0; k--){
            if(table(k, u) != table(k, v)){
                u = table(k, u);
                v = table(k, v);
            }
        }
        return parent_vertex[u];
    }

    int dist(int u, int v) const{
        int w = lca(u, v);
        return depth_vertex[u] + depth_vertex[v] - 2 * depth_vertex[w];
    }

    int path_vertex_count(int u, int v) const{
        return dist(u, v) + 1;
    }
};
