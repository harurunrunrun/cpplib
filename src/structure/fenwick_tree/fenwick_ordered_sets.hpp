#ifndef CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_ORDERED_SETS_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_ORDERED_SETS_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Value>
class FenwickOrderedSets{
    int n_;
    std::vector<std::vector<Value>> coordinates_;
    std::vector<std::vector<int>> bit_;

    static void add_bit(std::vector<int>& bit, int position, int delta){
        for(++position; position < static_cast<int>(bit.size()); position += position & -position) bit[position] += delta;
    }
    static int sum_bit(const std::vector<int>& bit, int right){
        int result = 0;
        for(; right > 0; right -= right & -right) result += bit[right];
        return result;
    }

public:
    FenwickOrderedSets(int n, const std::vector<std::pair<int, Value>>& possible_updates)
        : n_(n), coordinates_(n + 1), bit_(n + 1){
        if(n < 0) throw std::invalid_argument("FenwickOrderedSets: negative size");
        for(const auto& [position, value]: possible_updates){
            if(position < 0 || position >= n_) throw std::out_of_range("FenwickOrderedSets: invalid position");
            for(int p = position + 1; p <= n_; p += p & -p) coordinates_[p].push_back(value);
        }
        for(int p = 1; p <= n_; ++p){
            auto& values = coordinates_[p];
            std::sort(values.begin(), values.end());
            values.erase(std::unique(values.begin(), values.end()), values.end());
            bit_[p].assign(values.size() + 1, 0);
        }
    }
    int size() const noexcept { return n_; }
    void add(int position, const Value& value, int delta = 1){
        if(position < 0 || position >= n_) throw std::out_of_range("FenwickOrderedSets: invalid position");
        for(int p = position + 1; p <= n_; p += p & -p){
            const auto it = std::lower_bound(coordinates_[p].begin(), coordinates_[p].end(), value);
            if(it == coordinates_[p].end() || *it != value)
                throw std::invalid_argument("FenwickOrderedSets: value was not registered");
            add_bit(bit_[p], static_cast<int>(it - coordinates_[p].begin()), delta);
        }
    }
    int prefix_count_less(int right, const Value& value) const {
        if(right < 0 || right > n_) throw std::out_of_range("FenwickOrderedSets: invalid range");
        int result = 0;
        for(int p = right; p > 0; p -= p & -p){
            const int count = static_cast<int>(std::lower_bound(coordinates_[p].begin(), coordinates_[p].end(), value)
                                               - coordinates_[p].begin());
            result += sum_bit(bit_[p], count);
        }
        return result;
    }
    int count_less(int left, int right, const Value& value) const {
        if(left < 0 || left > right || right > n_) throw std::out_of_range("FenwickOrderedSets: invalid range");
        return prefix_count_less(right, value) - prefix_count_less(left, value);
    }
    int count(int left, int right, const Value& lower, const Value& upper) const {
        if(upper < lower) throw std::invalid_argument("FenwickOrderedSets: invalid value range");
        return count_less(left, right, upper) - count_less(left, right, lower);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_ORDERED_SETS_HPP_INCLUDED
