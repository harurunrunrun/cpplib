#ifndef CPPLIB_SRC_STRUCTURE_FENWICK_TREE_PERSISTENT_FENWICK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_FENWICK_TREE_PERSISTENT_FENWICK_TREE_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
class PersistentFenwickTree{
    int n_ = 0;
    int versions_ = 1;
    std::vector<std::vector<std::pair<int, T>>> history_;

public:
    explicit PersistentFenwickTree(int n): n_(n), history_(n + 1) {
        if(n < 0) throw std::invalid_argument("PersistentFenwickTree: negative size");
        for(auto& history: history_) history.emplace_back(0, T{});
    }
    int size() const noexcept { return n_; }
    int version_count() const noexcept { return versions_; }
    int add(int position, const T& delta){
        if(position < 0 || position >= n_) throw std::out_of_range("PersistentFenwickTree: invalid position");
        const int version = versions_++;
        for(int p = position + 1; p <= n_; p += p & -p){
            history_[p].emplace_back(version, history_[p].back().second + delta);
        }
        return version;
    }
    T prefix_sum(int version, int right) const {
        check_version(version);
        if(right < 0 || right > n_) throw std::out_of_range("PersistentFenwickTree: invalid range");
        T result{};
        for(int p = right; p > 0; p -= p & -p){
            const auto& history = history_[p];
            const auto it = std::upper_bound(history.begin(), history.end(), version,
                [](int value, const auto& item){ return value < item.first; });
            result += std::prev(it)->second;
        }
        return result;
    }
    T sum(int version, int left, int right) const {
        if(left < 0 || left > right || right > n_) throw std::out_of_range("PersistentFenwickTree: invalid range");
        return prefix_sum(version, right) - prefix_sum(version, left);
    }

private:
    void check_version(int version) const {
        if(version < 0 || version >= versions_) throw std::out_of_range("PersistentFenwickTree: invalid version");
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_FENWICK_TREE_PERSISTENT_FENWICK_TREE_HPP_INCLUDED
