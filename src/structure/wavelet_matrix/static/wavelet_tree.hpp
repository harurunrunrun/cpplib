#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_STATIC_WAVELET_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_STATIC_WAVELET_TREE_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <vector>

template<class T>
class WaveletTree{
    struct Node{
        T lower{}, upper{};
        std::vector<int> left_prefix;
        int left = -1, right = -1;
    };
    int n_ = 0;
    std::vector<Node> nodes_;
    std::vector<T> values_;

    int build(std::vector<int> indices, int lower, int upper){
        const int id = static_cast<int>(nodes_.size());
        nodes_.push_back({values_[lower], values_[upper - 1], std::vector<int>(indices.size() + 1), -1, -1});
        if(upper - lower == 1) return id;
        const int middle = lower + (upper - lower) / 2;
        std::vector<int> lhs, rhs;
        lhs.reserve(indices.size()); rhs.reserve(indices.size());
        for(std::size_t i = 0; i < indices.size(); ++i){
            if(indices[i] < middle) lhs.push_back(indices[i]); else rhs.push_back(indices[i]);
            nodes_[id].left_prefix[i + 1] = static_cast<int>(lhs.size());
        }
        if(!lhs.empty()) nodes_[id].left = build(std::move(lhs), lower, middle);
        if(!rhs.empty()) nodes_[id].right = build(std::move(rhs), middle, upper);
        return id;
    }
    int count_less_impl(int node, int left, int right, const T& value) const {
        if(node == -1 || left == right || value <= nodes_[node].lower) return 0;
        if(nodes_[node].upper < value) return right - left;
        const int ll = nodes_[node].left_prefix[left], lr = nodes_[node].left_prefix[right];
        return count_less_impl(nodes_[node].left, ll, lr, value)
             + count_less_impl(nodes_[node].right, left - ll, right - lr, value);
    }

public:
    WaveletTree() = default;
    explicit WaveletTree(const std::vector<T>& sequence): n_(static_cast<int>(sequence.size())), values_(sequence){
        std::sort(values_.begin(), values_.end());
        values_.erase(std::unique(values_.begin(), values_.end()), values_.end());
        if(n_ == 0) return;
        std::vector<int> indices;
        indices.reserve(n_);
        for(const T& value: sequence)
            indices.push_back(static_cast<int>(std::lower_bound(values_.begin(), values_.end(), value) - values_.begin()));
        build(std::move(indices), 0, static_cast<int>(values_.size()));
    }
    int size() const noexcept { return n_; }
    int count_less(int left, int right, const T& value) const {
        check_range(left, right);
        return n_ == 0 ? 0 : count_less_impl(0, left, right, value);
    }
    int count(int left, int right, const T& lower, const T& upper) const {
        check_range(left, right);
        return count_less(left, right, upper) - count_less(left, right, lower);
    }
    T kth_smallest(int left, int right, int k) const {
        check_range(left, right);
        if(k < 0 || k >= right - left) throw std::out_of_range("WaveletTree: invalid rank");
        int node = 0;
        while(nodes_[node].left != -1 || nodes_[node].right != -1){
            const int ll = nodes_[node].left_prefix[left], lr = nodes_[node].left_prefix[right];
            const int count_left = lr - ll;
            if(k < count_left){ node = nodes_[node].left; left = ll; right = lr; }
            else { k -= count_left; node = nodes_[node].right; left -= ll; right -= lr; }
        }
        return nodes_[node].lower;
    }
private:
    void check_range(int left, int right) const {
        if(left < 0 || left > right || right > n_) throw std::out_of_range("WaveletTree: invalid range");
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_STATIC_WAVELET_TREE_HPP_INCLUDED
