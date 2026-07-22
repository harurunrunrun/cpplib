#ifndef CPPLIB_SRC_STRUCTURE_OTHER_SPARSE_TABLE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_SPARSE_TABLE_HPP_INCLUDED

#include <array>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<auto Semilattice, int MAX_SIZE>
struct SparseTable{
    static_assert(MAX_SIZE > 0);

    using S = typename std::remove_cvref_t<decltype(Semilattice)>::S;

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

    explicit SparseTable(int n = MAX_SIZE):_n(n){
        if(_n < 0 || MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(Semilattice.e());
    }

    explicit SparseTable(const std::vector<S>& v):_n((int)v.size()){
        if(MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(Semilattice.e());
        for(int i = 0; i < _n; i++){
            table(0, i) = v[i];
        }
        for(int k = 1; k < LOG; k++){
            const int len = 1 << k;
            if(_n < len) break;
            const int half = len >> 1;
            for(int i = 0; i + len <= _n; i++){
                table(k, i) = Semilattice.op(table(k - 1, i), table(k - 1, i + half));
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
            return Semilattice.e();
        }
        const int k = floor_log2((unsigned int)(r - l));
        return Semilattice.op(table(k, l), table(k, r - (1 << k)));
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

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_SPARSE_TABLE_HPP_INCLUDED
