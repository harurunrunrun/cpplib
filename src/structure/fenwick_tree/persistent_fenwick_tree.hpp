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
        int root = roots_[static_cast<std::size_t>(base_version)];
        for(int fenwick_index = position + 1;
            fenwick_index <= n_;
            fenwick_index += fenwick_index & -fenwick_index){
            const T value = read(
                root, 1, n_ + 1, fenwick_index
            ) + delta;
            root = assign(
                root, 1, n_ + 1, fenwick_index, value
            );
        }
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
        T result{};
        const int root = roots_[static_cast<std::size_t>(version)];
        for(int fenwick_index = right;
            fenwick_index > 0;
            fenwick_index -= fenwick_index & -fenwick_index){
            result += read(root, 1, n_ + 1, fenwick_index);
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

    T read(int node, int left, int right, int position) const {
        if(node == -1) return T{};
        if(right - left == 1){
            return nodes_[static_cast<std::size_t>(node)].value;
        }
        const int middle = left + (right - left) / 2;
        if(position < middle){
            return read(
                nodes_[static_cast<std::size_t>(node)].left,
                left, middle, position
            );
        }
        return read(
            nodes_[static_cast<std::size_t>(node)].right,
            middle, right, position
        );
    }

    int assign(
        int node,
        int left,
        int right,
        int position,
        const T& value
    ){
        const int copied = static_cast<int>(nodes_.size());
        nodes_.push_back(node == -1
            ? Node{}
            : nodes_[static_cast<std::size_t>(node)]);
        if(right - left == 1){
            nodes_[static_cast<std::size_t>(copied)].value = value;
            return copied;
        }
        const int middle = left + (right - left) / 2;
        if(position < middle){
            nodes_[static_cast<std::size_t>(copied)].left = assign(
                nodes_[static_cast<std::size_t>(copied)].left,
                left, middle, position, value
            );
        }else{
            nodes_[static_cast<std::size_t>(copied)].right = assign(
                nodes_[static_cast<std::size_t>(copied)].right,
                middle, right, position, value
            );
        }
        return copied;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_FENWICK_TREE_PERSISTENT_FENWICK_TREE_HPP_INCLUDED
