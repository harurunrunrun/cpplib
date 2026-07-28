#ifndef CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_LADDER_DECOMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_LADDER_DECOMPOSITION_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

class LadderDecomposition{
    int n_ = 0;
    std::vector<int> parent_, depth_, heavy_, ladder_, position_;
    std::vector<std::vector<int>> vertices_;

public:
    LadderDecomposition() = default;
    explicit LadderDecomposition(const std::vector<std::vector<int>>& graph, int root = 0)
        : n_(static_cast<int>(graph.size())), parent_(n_, -1), depth_(n_), heavy_(n_, -1),
          ladder_(n_, -1), position_(n_, -1){
        if(n_ == 0) return;
        if(root < 0 || root >= n_) throw std::out_of_range("LadderDecomposition: invalid root");
        std::vector<int> order{root};
        parent_[root] = root;
        for(std::size_t i = 0; i < order.size(); ++i){
            const int v = order[i];
            for(const int to: graph[v]){
                if(to < 0 || to >= n_) throw std::out_of_range("LadderDecomposition: invalid edge");
                if(parent_[to] != -1) continue;
                parent_[to] = v;
                depth_[to] = depth_[v] + 1;
                order.push_back(to);
            }
        }
        if(static_cast<int>(order.size()) != n_) throw std::invalid_argument("LadderDecomposition: disconnected graph");
        std::vector<int> subtree(n_, 1);
        for(int i = n_ - 1; i > 0; --i){
            const int v = order[i], p = parent_[v];
            subtree[p] += subtree[v];
            if(heavy_[p] == -1 || subtree[v] > subtree[heavy_[p]]) heavy_[p] = v;
        }
        for(const int start: order){
            if(start != root && heavy_[parent_[start]] == start) continue;
            const int id = static_cast<int>(vertices_.size());
            vertices_.emplace_back();
            for(int v = start; v != -1; v = heavy_[v]){
                ladder_[v] = id;
                position_[v] = static_cast<int>(vertices_[id].size());
                vertices_[id].push_back(v);
            }
        }
    }

    int size() const noexcept { return n_; }
    int ladder_count() const noexcept { return static_cast<int>(vertices_.size()); }
    int parent(int v) const { check(v); return parent_[v]; }
    int depth(int v) const { check(v); return depth_[v]; }
    int ladder_id(int v) const { check(v); return ladder_[v]; }
    int position(int v) const { check(v); return position_[v]; }
    int head(int v) const { check(v); return vertices_[ladder_[v]].front(); }
    const std::vector<int>& ladder_vertices(int id) const {
        if(id < 0 || id >= ladder_count()) throw std::out_of_range("LadderDecomposition: invalid ladder");
        return vertices_[id];
    }
    std::vector<std::pair<int, int>> path_to_root(int v) const {
        check(v);
        std::vector<std::pair<int, int>> result;
        while(true){
            const int id = ladder_[v];
            result.emplace_back(id, position_[v] + 1);
            const int h = vertices_[id].front();
            if(parent_[h] == h) break;
            v = parent_[h];
        }
        return result;
    }

private:
    void check(int v) const {
        if(v < 0 || v >= n_) throw std::out_of_range("LadderDecomposition: invalid vertex");
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_LADDER_DECOMPOSITION_HPP_INCLUDED
