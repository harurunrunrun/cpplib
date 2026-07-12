#pragma once

#include <algorithm>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

struct CentroidDecomposition{
private:
    int _n;
    std::vector<std::vector<int>> graph;
    std::vector<std::vector<int>> centroid_graph;
    std::vector<int> centroid_parent;
    std::vector<int> centroid_depth;
    std::vector<int> subtree_size;
    std::vector<int> component_parent;
    std::vector<char> removed;
    int root_centroid;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_tree() const{
        long long edge_twice = 0;
        for(int v = 0; v < _n; v++){
            edge_twice += (int)graph[v].size();
            for(int to: graph[v]){
                if(to < 0 || _n <= to)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: range violation (build).");
                }
            }
        }
        if(_n > 0 && edge_twice != 2LL * (_n - 1))[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not a tree (build).");
        }
    }

    std::vector<int> collect_component(int start){
        std::vector<int> nodes;
        std::vector<int> stack = {start};
        component_parent[start] = -2;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            nodes.push_back(v);
            for(int to: graph[v]){
                if(removed[to] || to == component_parent[v]){
                    continue;
                }
                if(component_parent[to] != -1)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: graph is not a tree (build).");
                }
                component_parent[to] = v;
                stack.push_back(to);
            }
        }
        return nodes;
    }

    int find_centroid(const std::vector<int>& nodes){
        for(auto it = nodes.rbegin(); it != nodes.rend(); ++it){
            int v = *it;
            subtree_size[v] = 1;
            for(int to: graph[v]){
                if(!removed[to] && component_parent[to] == v){
                    subtree_size[v] += subtree_size[to];
                }
            }
        }

        const int component_size = (int)nodes.size();
        int centroid = nodes[0];
        for(int v: nodes){
            int largest = component_size - subtree_size[v];
            for(int to: graph[v]){
                if(!removed[to] && component_parent[to] == v){
                    largest = std::max(largest, subtree_size[to]);
                }
            }
            if(largest * 2 <= component_size){
                centroid = v;
                break;
            }
        }
        return centroid;
    }

public:
    explicit CentroidDecomposition(const std::vector<std::vector<int>>& tree = {}):
        _n((int)tree.size()),
        graph(tree),
        centroid_graph(_n),
        centroid_parent(_n, -1),
        centroid_depth(_n, -1),
        subtree_size(_n, 0),
        component_parent(_n, -1),
        removed(_n, false),
        root_centroid(-1){
        build();
    }

    int size() const{
        return _n;
    }

    void build(){
        check_tree();
        std::fill(centroid_graph.begin(), centroid_graph.end(), std::vector<int>{});
        std::fill(centroid_parent.begin(), centroid_parent.end(), -1);
        std::fill(centroid_depth.begin(), centroid_depth.end(), -1);
        std::fill(subtree_size.begin(), subtree_size.end(), 0);
        std::fill(component_parent.begin(), component_parent.end(), -1);
        std::fill(removed.begin(), removed.end(), false);
        root_centroid = -1;

        if(_n == 0){
            return;
        }

        std::vector<std::tuple<int, int, int>> stack = {{0, -1, 0}};
        int processed = 0;
        while(!stack.empty()){
            auto [start, parent, depth] = stack.back();
            stack.pop_back();
            if(removed[start]){
                continue;
            }

            auto nodes = collect_component(start);
            int centroid = find_centroid(nodes);
            removed[centroid] = true;
            processed++;

            centroid_parent[centroid] = parent;
            centroid_depth[centroid] = depth;
            if(parent == -1){
                root_centroid = centroid;
            }else{
                centroid_graph[parent].push_back(centroid);
                centroid_graph[centroid].push_back(parent);
            }

            for(int to: graph[centroid]){
                if(!removed[to]){
                    stack.push_back({to, centroid, depth + 1});
                }
            }

            for(int v: nodes){
                component_parent[v] = -1;
            }
        }
        if(processed != _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not connected (build).");
        }
    }

    int root() const{
        return root_centroid;
    }

    int parent(int v) const{
        check_vertex(v, "library assertion fault: range violation (parent).");
        return centroid_parent[v];
    }

    int depth(int v) const{
        check_vertex(v, "library assertion fault: range violation (depth).");
        return centroid_depth[v];
    }

    const std::vector<std::vector<int>>& tree() const{
        return centroid_graph;
    }
};
