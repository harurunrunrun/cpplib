#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_DISJOINT_SPARSE_TABLE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_DISJOINT_SPARSE_TABLE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <algorithm>
#include <bit>
#include "../../array/static_or_vector.hpp"
#include <stdexcept>
#include <type_traits>
#include <vector>

template<auto Monoid, int MAX_SIZE = -1>
struct DisjointSparseTable{
    static_assert(MAX_SIZE > 0);

    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

    static constexpr int LOG = []{
        int lg = 1;
        int n = 1;
        while(n < MAX_SIZE){
            n <<= 1;
            lg++;
        }
        return lg;
    }();

    int _n;
    std::array<S, LOG * MAX_SIZE> data;

    explicit DisjointSparseTable(int n = MAX_SIZE):_n(n){
        if(_n < 0 || MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(Monoid.e());
    }

    explicit DisjointSparseTable(const std::vector<S>& v):_n(cpplib_detail::checked_int_size(v.size())){
        if(MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(Monoid.e());
        for(int i = 0; i < _n; i++){
            table(0, i) = v[i];
        }
        for(int k = 1; k < LOG; k++){
            const std::size_t half = std::size_t(1) << (k - 1);
            const std::size_t block = half * 2;
            for(std::size_t offset = 0; offset < static_cast<std::size_t>(_n); offset += block){
                const int left = static_cast<int>(offset);
                const int mid = static_cast<int>(std::min(offset + half, static_cast<std::size_t>(_n)));
                const int right = static_cast<int>(std::min(offset + block, static_cast<std::size_t>(_n)));

                table(k, mid - 1) = table(0, mid - 1);
                for(int i = mid - 2; i >= left; i--){
                    table(k, i) = Monoid.op(table(0, i), table(k, i + 1));
                }
                if(mid < right){
                    table(k, mid) = table(0, mid);
                    for(int i = mid + 1; i < right; i++){
                        table(k, i) = Monoid.op(table(k, i - 1), table(0, i));
                    }
                }
            }
        }
    }

    int size()const{
        return _n;
    }

    bool empty()const{
        return _n == 0;
    }

    S get(int k)const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return table(0, k);
    }

    S prod(int l, int r)const{
        if(l < 0 || l > r || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prod).");
        }
        if(l == r){
            return Monoid.e();
        }
        r--;
        if(l == r){
            return table(0, l);
        }
        const int k = floor_log2((unsigned int)(l ^ r)) + 1;
        return Monoid.op(table(k, l), table(k, r));
    }

    S all_prod()const{
        return prod(0, _n);
    }

private:
    S& table(int k, int i){
        return data[k * MAX_SIZE + i];
    }

    const S& table(int k, int i)const{
        return data[k * MAX_SIZE + i];
    }

    static int floor_log2(unsigned int x){
        return 31 - __builtin_clz(x);
    }
};


template<auto Monoid>
struct DisjointSparseTable<Monoid, -1>{

    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

    int LOG = 1;

    int _n;
    std::vector<S> data;

    explicit DisjointSparseTable(int n = 0):_n(n){
        if(_n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        LOG = 1 + static_cast<int>(std::bit_width(static_cast<unsigned>(_n > 0 ? _n - 1 : 0)));
        data.assign(static_cast<std::size_t>(LOG) * _n, Monoid.e());
    }

    explicit DisjointSparseTable(const std::vector<S>& v):_n(cpplib_detail::checked_int_size(v.size())){
        if(_n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        LOG = 1 + static_cast<int>(std::bit_width(static_cast<unsigned>(_n > 0 ? _n - 1 : 0)));
        data.assign(static_cast<std::size_t>(LOG) * _n, Monoid.e());
        for(int i = 0; i < _n; i++){
            table(0, i) = v[i];
        }
        for(int k = 1; k < LOG; k++){
            const std::size_t half = std::size_t(1) << (k - 1);
            const std::size_t block = half * 2;
            for(std::size_t offset = 0; offset < static_cast<std::size_t>(_n); offset += block){
                const int left = static_cast<int>(offset);
                const int mid = static_cast<int>(std::min(offset + half, static_cast<std::size_t>(_n)));
                const int right = static_cast<int>(std::min(offset + block, static_cast<std::size_t>(_n)));

                table(k, mid - 1) = table(0, mid - 1);
                for(int i = mid - 2; i >= left; i--){
                    table(k, i) = Monoid.op(table(0, i), table(k, i + 1));
                }
                if(mid < right){
                    table(k, mid) = table(0, mid);
                    for(int i = mid + 1; i < right; i++){
                        table(k, i) = Monoid.op(table(k, i - 1), table(0, i));
                    }
                }
            }
        }
    }

    int size()const{
        return _n;
    }

    bool empty()const{
        return _n == 0;
    }

    S get(int k)const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return table(0, k);
    }

    S prod(int l, int r)const{
        if(l < 0 || l > r || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prod).");
        }
        if(l == r){
            return Monoid.e();
        }
        r--;
        if(l == r){
            return table(0, l);
        }
        const int k = floor_log2((unsigned int)(l ^ r)) + 1;
        return Monoid.op(table(k, l), table(k, r));
    }

    S all_prod()const{
        return prod(0, _n);
    }

private:
    typename std::vector<S>::reference table(int k, int i){
        return data[static_cast<std::size_t>(k) * _n + i];
    }

    typename std::vector<S>::const_reference table(int k, int i)const{
        return data[static_cast<std::size_t>(k) * _n + i];
    }

    static int floor_log2(unsigned int x){
        return 31 - __builtin_clz(x);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_DISJOINT_SPARSE_TABLE_HPP_INCLUDED
