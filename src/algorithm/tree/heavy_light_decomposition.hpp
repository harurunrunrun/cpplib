#ifndef CPPLIB_SRC_ALGORITHM_TREE_HEAVY_LIGHT_DECOMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_HEAVY_LIGHT_DECOMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

struct HeavyLightDecomposition{
private:
    int _n;
    int root_vertex;
    std::vector<std::vector<int>> graph;
    std::vector<int> parent_vertex;
    std::vector<int> depth_vertex;
    std::vector<int> subtree_size;
    std::vector<int> heavy_child;
    std::vector<int> head_vertex;
    std::vector<int> in_vertex;
    std::vector<int> out_vertex;
    std::vector<int> vertex_at;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    explicit HeavyLightDecomposition(int n = 0):
        _n(n),
        root_vertex(0),
        graph(n),
        parent_vertex(n, -1),
        depth_vertex(n, 0),
        subtree_size(n, 0),
        heavy_child(n, -1),
        head_vertex(n, -1),
        in_vertex(n, -1),
        out_vertex(n, -1),
        vertex_at(n, -1){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
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
        root_vertex = root;
        std::fill(parent_vertex.begin(), parent_vertex.end(), -1);
        std::fill(depth_vertex.begin(), depth_vertex.end(), 0);
        std::fill(subtree_size.begin(), subtree_size.end(), 0);
        std::fill(heavy_child.begin(), heavy_child.end(), -1);
        std::fill(head_vertex.begin(), head_vertex.end(), -1);
        std::fill(in_vertex.begin(), in_vertex.end(), -1);
        std::fill(out_vertex.begin(), out_vertex.end(), -1);
        std::fill(vertex_at.begin(), vertex_at.end(), -1);

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

        for(auto it = order.rbegin(); it != order.rend(); ++it){
            int v = *it;
            subtree_size[v] = 1;
            int best_size = 0;
            for(int to: graph[v]){
                if(parent_vertex[to] == v){
                    subtree_size[v] += subtree_size[to];
                    if(best_size < subtree_size[to]){
                        best_size = subtree_size[to];
                        heavy_child[v] = to;
                    }
                }
            }
        }
        parent_vertex[root] = -1;

        int timer = 0;
        std::vector<std::pair<int, int>> heads = {{root, root}};
        while(!heads.empty()){
            auto [start, head] = heads.back();
            heads.pop_back();
            for(int v = start; v != -1; v = heavy_child[v]){
                head_vertex[v] = head;
                in_vertex[v] = timer;
                vertex_at[timer] = v;
                timer++;
                for(int to: graph[v]){
                    if(parent_vertex[to] == v && to != heavy_child[v]){
                        heads.push_back({to, to});
                    }
                }
            }
        }
        for(auto it = order.rbegin(); it != order.rend(); ++it){
            int v = *it;
            int out = in_vertex[v] + 1;
            for(int to: graph[v]){
                if(parent_vertex[to] == v){
                    out = std::max(out, out_vertex[to]);
                }
            }
            out_vertex[v] = out;
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

    int subtree(int v) const{
        check_vertex(v, "library assertion fault: range violation (subtree).");
        return subtree_size[v];
    }

    int head(int v) const{
        check_vertex(v, "library assertion fault: range violation (head).");
        return head_vertex[v];
    }

    int in(int v) const{
        check_vertex(v, "library assertion fault: range violation (in).");
        return in_vertex[v];
    }

    int out(int v) const{
        check_vertex(v, "library assertion fault: range violation (out).");
        return out_vertex[v];
    }

    int operator[](int k) const{
        check_vertex(k, "library assertion fault: range violation (operator[]).");
        return vertex_at[k];
    }

    int lca(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (lca).");
        check_vertex(v, "library assertion fault: range violation (lca).");
        while(head_vertex[u] != head_vertex[v]){
            if(depth_vertex[head_vertex[u]] > depth_vertex[head_vertex[v]]){
                u = parent_vertex[head_vertex[u]];
            }else{
                v = parent_vertex[head_vertex[v]];
            }
        }
        return depth_vertex[u] < depth_vertex[v] ? u : v;
    }

    int dist(int u, int v) const{
        int w = lca(u, v);
        return depth_vertex[u] + depth_vertex[v] - 2 * depth_vertex[w];
    }

    bool is_ancestor(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (is_ancestor).");
        check_vertex(v, "library assertion fault: range violation (is_ancestor).");
        return in_vertex[u] <= in_vertex[v] && in_vertex[v] < out_vertex[u];
    }

    int jump(int v, int k) const{
        check_vertex(v, "library assertion fault: range violation (jump).");
        if(k < 0 || depth_vertex[v] < k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (jump).");
        }
        while(k > 0){
            int h = head_vertex[v];
            int d = depth_vertex[v] - depth_vertex[h];
            if(k <= d){
                return vertex_at[in_vertex[v] - k];
            }
            k -= d + 1;
            v = parent_vertex[h];
        }
        return v;
    }

    std::vector<std::pair<int, int>> path_query(int u, int v, bool vertex = true) const{
        check_vertex(u, "library assertion fault: range violation (path_query).");
        check_vertex(v, "library assertion fault: range violation (path_query).");
        std::vector<std::pair<int, int>> left, right;
        auto push = [](std::vector<std::pair<int, int>>& ranges, int l, int r){
            if(l < r){
                ranges.push_back({l, r});
            }
        };
        while(head_vertex[u] != head_vertex[v]){
            if(depth_vertex[head_vertex[u]] >= depth_vertex[head_vertex[v]]){
                push(left, in_vertex[head_vertex[u]], in_vertex[u] + 1);
                u = parent_vertex[head_vertex[u]];
            }else{
                push(right, in_vertex[head_vertex[v]], in_vertex[v] + 1);
                v = parent_vertex[head_vertex[v]];
            }
        }
        if(depth_vertex[u] > depth_vertex[v]){
            push(left, in_vertex[v] + (vertex ? 0 : 1), in_vertex[u] + 1);
        }else{
            push(right, in_vertex[u] + (vertex ? 0 : 1), in_vertex[v] + 1);
        }
        std::reverse(right.begin(), right.end());
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }

    std::pair<int, int> subtree_query(int v, bool vertex = true) const{
        check_vertex(v, "library assertion fault: range violation (subtree_query).");
        return {in_vertex[v] + (vertex ? 0 : 1), out_vertex[v]};
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_HEAVY_LIGHT_DECOMPOSITION_HPP_INCLUDED
