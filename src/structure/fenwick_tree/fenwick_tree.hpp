#ifndef CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_TREE_HPP_INCLUDED

#include <array>
#include <type_traits>
#include "../array/static_or_vector.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE = -1>
struct FenwickTree{
    static_assert(MAX_SIZE == -1 || MAX_SIZE > 0);

    int _n;
    cpplib_detail::StaticOrVector<T, (MAX_SIZE < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_SIZE) + 1)> data;

    explicit FenwickTree(int n = (MAX_SIZE < 0 ? 0 : MAX_SIZE)) : _n(n){
        if(_n < 0 || (MAX_SIZE >= 0 && MAX_SIZE < _n)) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.resize(static_cast<std::size_t>(_n) + 1);
        data.fill(T{});
    }

    explicit FenwickTree(const std::vector<T>& v) : FenwickTree(cpplib_detail::checked_int_size(v.size())){
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
        for(std::size_t i = static_cast<std::size_t>(k) + 1; i <= static_cast<std::size_t>(_n); i += i & -i){
            if constexpr(std::is_same_v<T, bool>) data[i] = static_cast<T>(data[i]) + x;
            else data[i] += x;
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

#endif  // CPPLIB_SRC_STRUCTURE_FENWICK_TREE_FENWICK_TREE_HPP_INCLUDED
