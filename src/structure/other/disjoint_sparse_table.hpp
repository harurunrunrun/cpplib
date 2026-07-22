#ifndef CPPLIB_SRC_STRUCTURE_OTHER_DISJOINT_SPARSE_TABLE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_DISJOINT_SPARSE_TABLE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<auto Monoid, int MAX_SIZE>
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

    explicit DisjointSparseTable(const std::vector<S>& v):_n((int)v.size()){
        if(MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(Monoid.e());
        for(int i = 0; i < _n; i++){
            table(0, i) = v[i];
        }
        for(int k = 1, half = 1; k < LOG; k++, half <<= 1){
            const int block = half << 1;
            for(int left = 0; left < _n; left += block){
                const int mid = std::min(left + half, _n);
                const int right = std::min(left + block, _n);

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

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_DISJOINT_SPARSE_TABLE_HPP_INCLUDED
