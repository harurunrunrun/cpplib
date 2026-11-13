#ifndef CPPLIB_SRC_STRUCTURE_FENWICK_TREE_PERSISTENT_FENWICK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_FENWICK_TREE_PERSISTENT_FENWICK_TREE_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
class PersistentFenwickTree{
    struct Node{
        T value{};
        int left = -1;
        int right = -1;
    };

    int n_ = 0;
    std::vector<Node> nodes_;
    std::vector<int> roots_{-1};

public:
    explicit PersistentFenwickTree(int n): n_(n) {
        if(n < 0){
            throw std::invalid_argument(
                "PersistentFenwickTree: negative size"
            );
        }
        if(n == std::numeric_limits<int>::max()){
            throw std::length_error(
                "PersistentFenwickTree: size is too large"
            );
        }
    }

    int size() const noexcept { return n_; }

    int version_count() const noexcept {
        return static_cast<int>(roots_.size());
    }

    int add(int base_version, int position, const T& delta){
        check_version(base_version);
        if(position < 0 || position >= n_){
            throw std::out_of_range(
                "PersistentFenwickTree: invalid position"
            );
        }
        const int root = add_rec(
            roots_[static_cast<std::size_t>(base_version)],
            0, n_, position, delta
        );
        roots_.push_back(root);
        return static_cast<int>(roots_.size()) - 1;
    }

    int add(int position, const T& delta){
        return add(version_count() - 1, position, delta);
    }

    T prefix_sum(int version, int right) const {
        check_version(version);
        if(right < 0 || right > n_){
            throw std::out_of_range(
                "PersistentFenwickTree: invalid range"
            );
        }
        // 区間 [0, right) を永続セグメント木の 1 本の経路で集計する
        T result{};
        int node = roots_[static_cast<std::size_t>(version)];
        int left = 0, right_end = n_;
        while(node != -1 && left < right){
            const Node& current = nodes_[static_cast<std::size_t>(node)];
            if(right_end <= right){
                result += current.value;
                break;
            }
            const int middle = left + (right_end - left) / 2;
            if(right <= middle){
                node = current.left;
                right_end = middle;
            }else{
                if(current.left != -1){
                    result += nodes_[static_cast<std::size_t>(current.left)].value;
                }
                node = current.right;
                left = middle;
            }
        }
        return result;
    }

    T sum(int version, int left, int right) const {
        if(left < 0 || left > right || right > n_){
            throw std::out_of_range(
                "PersistentFenwickTree: invalid range"
            );
        }
        return prefix_sum(version, right) - prefix_sum(version, left);
    }

private:
    void check_version(int version) const {
        if(version < 0 || version >= version_count()){
            throw std::out_of_range(
                "PersistentFenwickTree: invalid version"
            );
        }
    }

    // node を根とする区間 [left, right) の部分木を複製し、position に delta を加える
    int add_rec(
        int node,
        int left,
        int right,
        int position,
        const T& delta
    ){
        const int copied = static_cast<int>(nodes_.size());
        nodes_.push_back(node == -1
            ? Node{}
            : nodes_[static_cast<std::size_t>(node)]);
        nodes_[static_cast<std::size_t>(copied)].value =
            nodes_[static_cast<std::size_t>(copied)].value + delta;
        if(right - left == 1) return copied;
        const int middle = left + (right - left) / 2;
        if(position < middle){
            const int child = add_rec(
                nodes_[static_cast<std::size_t>(copied)].left,
                left, middle, position, delta
            );
            nodes_[static_cast<std::size_t>(copied)].left = child;
        }else{
            const int child = add_rec(
                nodes_[static_cast<std::size_t>(copied)].right,
                middle, right, position, delta
            );
            nodes_[static_cast<std::size_t>(copied)].right = child;
        }
        return copied;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_FENWICK_TREE_PERSISTENT_FENWICK_TREE_HPP_INCLUDED
