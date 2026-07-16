#pragma once

#include <algorithm>
#include <cstddef>
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
    std::vector<int> first_occurrence;
    std::vector<int> range_minimum_log;
    std::vector<int> range_minimum_vertex;
    int euler_size = 0;
    bool built = false;

    static std::size_t checked_size(int n){
        if(n < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        return static_cast<std::size_t>(n);
    }

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_built(const char* message) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int table(int k, int v) const{
        return up[
            static_cast<std::size_t>(k) * static_cast<std::size_t>(_n)
            + static_cast<std::size_t>(v)
        ];
    }

    int shallower(int u, int v) const{
        return depth_vertex[u] <= depth_vertex[v] ? u : v;
    }

public:
    explicit LowestCommonAncestor(int n = 0):
        _n(n),
        _log(1),
        graph(checked_size(n)),
        parent_vertex(checked_size(n), -1),
        depth_vertex(checked_size(n), 0),
        first_occurrence(checked_size(n), -1){
        while((1LL << _log) <= std::max(1, n)){
            _log++;
        }
        up.assign(
            static_cast<std::size_t>(_log)
                * static_cast<std::size_t>(std::max(1, n)),
            0
        );
    }

    int size() const{
        return _n;
    }

    void add_edge(int u, int v){
        check_vertex(u, "library assertion fault: range violation (add_edge).");
        check_vertex(v, "library assertion fault: range violation (add_edge).");
        graph[u].push_back(v);
        graph[v].push_back(u);
        built = false;
    }

    void build(int root = 0){
        if(_n == 0){
            euler_size = 0;
            range_minimum_log.clear();
            range_minimum_vertex.clear();
            built = true;
            return;
        }
        check_vertex(root, "library assertion fault: range violation (build).");
        built = false;
        std::size_t edge_twice = 0;
        for(const auto& adjacency: graph){
            edge_twice += adjacency.size();
        }
        if(edge_twice != 2ULL * static_cast<std::size_t>(_n - 1))[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not a tree (build).");
        }
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
            const std::size_t base =
                static_cast<std::size_t>(k) * static_cast<std::size_t>(_n);
            const std::size_t prev =
                static_cast<std::size_t>(k - 1) * static_cast<std::size_t>(_n);
            for(int v = 0; v < _n; v++){
                const std::size_t vertex = static_cast<std::size_t>(v);
                up[base + vertex] = up[prev + static_cast<std::size_t>(
                    up[prev + vertex]
                )];
            }
        }

        std::fill(first_occurrence.begin(), first_occurrence.end(), -1);
        std::vector<int> euler;
        euler.reserve(static_cast<std::size_t>(2 * _n - 1));
        std::vector<std::size_t> next_edge(static_cast<std::size_t>(_n), 0);
        stack = {root};
        first_occurrence[root] = 0;
        euler.push_back(root);
        while(!stack.empty()){
            const int v = stack.back();
            if(next_edge[v] == graph[v].size()){
                stack.pop_back();
                if(!stack.empty()){
                    euler.push_back(stack.back());
                }
                continue;
            }
            const int to = graph[v][next_edge[v]++];
            if(parent_vertex[to] != v){
                continue;
            }
            first_occurrence[to] = static_cast<int>(euler.size());
            euler.push_back(to);
            stack.push_back(to);
        }

        euler_size = static_cast<int>(euler.size());
        range_minimum_log.assign(static_cast<std::size_t>(euler_size + 1), 0);
        for(int length = 2; length <= euler_size; length++){
            range_minimum_log[length] = range_minimum_log[length / 2] + 1;
        }
        const int levels = range_minimum_log[euler_size] + 1;
        range_minimum_vertex.assign(
            static_cast<std::size_t>(levels) * static_cast<std::size_t>(euler_size),
            root
        );
        std::copy(euler.begin(), euler.end(), range_minimum_vertex.begin());
        for(int level = 1; level < levels; level++){
            const int half = 1 << (level - 1);
            const int length = half << 1;
            const int previous = (level - 1) * euler_size;
            const int current = level * euler_size;
            for(int left = 0; left + length <= euler_size; left++){
                range_minimum_vertex[current + left] = shallower(
                    range_minimum_vertex[previous + left],
                    range_minimum_vertex[previous + left + half]
                );
            }
        }
        built = true;
    }

    int parent(int v) const{
        check_vertex(v, "library assertion fault: range violation (parent).");
        check_built("library assertion fault: build is required (parent).");
        return parent_vertex[v];
    }

    int depth(int v) const{
        check_vertex(v, "library assertion fault: range violation (depth).");
        check_built("library assertion fault: build is required (depth).");
        return depth_vertex[v];
    }

    int jump(int v, int k) const{
        check_vertex(v, "library assertion fault: range violation (jump).");
        check_built("library assertion fault: build is required (jump).");
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
        check_built("library assertion fault: build is required (lca).");
        int left = first_occurrence[u];
        int right = first_occurrence[v];
        if(left > right){
            std::swap(left, right);
        }
        const int length = right - left + 1;
        const int level = range_minimum_log[length];
        const int block = 1 << level;
        return shallower(
            range_minimum_vertex[level * euler_size + left],
            range_minimum_vertex[level * euler_size + right - block + 1]
        );
    }

    int dist(int u, int v) const{
        int w = lca(u, v);
        return depth_vertex[u] + depth_vertex[v] - 2 * depth_vertex[w];
    }

    int path_vertex_count(int u, int v) const{
        return dist(u, v) + 1;
    }
};
