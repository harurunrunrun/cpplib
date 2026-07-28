#ifndef CPPLIB_SRC_ALGORITHM_TREE_QUERY_LEVEL_ANCESTOR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_QUERY_LEVEL_ANCESTOR_HPP_INCLUDED

#include <bit>
#include <stdexcept>
#include <vector>

class LevelAncestor{
    int n_ = 0;
    int log_ = 0;
    std::vector<int> depth_;
    std::vector<std::vector<int>> up_;

public:
    LevelAncestor() = default;

    explicit LevelAncestor(const std::vector<std::vector<int>>& graph, int root = 0)
        : n_(static_cast<int>(graph.size())){
        if(n_ == 0) return;
        if(root < 0 || root >= n_) throw std::out_of_range("LevelAncestor: invalid root");
        log_ = static_cast<int>(std::bit_width(static_cast<unsigned int>(n_)));
        depth_.assign(n_, -1);
        up_.assign(log_, std::vector<int>(n_, root));
        std::vector<int> stack{root};
        depth_[root] = 0;
        for(std::size_t i = 0; i < stack.size(); ++i){
            const int v = stack[i];
            for(const int to: graph[v]){
                if(to < 0 || to >= n_) throw std::out_of_range("LevelAncestor: invalid edge");
                if(depth_[to] != -1) continue;
                depth_[to] = depth_[v] + 1;
                up_[0][to] = v;
                stack.push_back(to);
            }
        }
        if(static_cast<int>(stack.size()) != n_) throw std::invalid_argument("LevelAncestor: graph is disconnected");
        for(int k = 1; k < log_; ++k){
            for(int v = 0; v < n_; ++v) up_[k][v] = up_[k - 1][up_[k - 1][v]];
        }
    }

    int size() const noexcept { return n_; }
    int depth(int v) const {
        check_vertex(v);
        return depth_[v];
    }
    int kth_ancestor(int v, int distance) const {
        check_vertex(v);
        if(distance < 0 || distance > depth_[v]) return -1;
        for(int k = 0; distance != 0; ++k, distance >>= 1){
            if((distance & 1) != 0) v = up_[k][v];
        }
        return v;
    }
    int ancestor_at_depth(int v, int target_depth) const {
        check_vertex(v);
        if(target_depth < 0 || target_depth > depth_[v]) return -1;
        return kth_ancestor(v, depth_[v] - target_depth);
    }

private:
    void check_vertex(int v) const {
        if(v < 0 || v >= n_) throw std::out_of_range("LevelAncestor: invalid vertex");
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_QUERY_LEVEL_ANCESTOR_HPP_INCLUDED
