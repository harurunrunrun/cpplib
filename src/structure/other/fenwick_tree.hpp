#ifndef CPPLIB_SRC_STRUCTURE_OTHER_FENWICK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_FENWICK_TREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE>
struct FenwickTree{
    static_assert(MAX_SIZE > 0);

    int _n;
    std::array<T, MAX_SIZE + 1> data;

    explicit FenwickTree(int n = MAX_SIZE) : _n(n){
        if(_n < 0 || MAX_SIZE < _n) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(T{});
    }

    explicit FenwickTree(const std::vector<T>& v) : FenwickTree(static_cast<int>(v.size())){
        for(std::size_t i = 0; i < v.size(); i++){
            add(static_cast<int>(i), v[i]);
        }
    }

    int size()const{
        return _n;
    }

    bool empty()const{
        return _n == 0;
    }

    void add(int k, const T& x){
        if(k < 0 || _n <= k) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (add).");
        }
        for(k++; k <= _n; k += k & -k){
            data[k] += x;
        }
    }

    T prefix_sum(int r)const{
        if(r < 0 || _n < r) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (prefix_sum).");
        }
        T res{};
        for(; r > 0; r -= r & -r){
            res += data[r];
        }
        return res;
    }

    T sum(int l, int r)const{
        if(l < 0 || l > r || _n < r) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (sum).");
        }
        return prefix_sum(r) - prefix_sum(l);
    }

    T get(int k)const{
        if(k < 0 || _n <= k) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return sum(k, k + 1);
    }

    void set(int k, const T& x){
        if(k < 0 || _n <= k) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (set).");
        }
        add(k, x - get(k));
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_FENWICK_TREE_HPP_INCLUDED
