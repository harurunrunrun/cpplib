#pragma once
#include <array>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include "../types/monoid.hpp"


template<auto Monoid, int MAX_SIZE>
struct Segtree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    static constexpr int sz = []{
        int _sz = 1;
        while(_sz < MAX_SIZE){
            _sz <<= 1;
        }
        return _sz;
    }();

    int _n;
    
    std::array<S, 2 * sz> data;
    
    explicit constexpr Segtree(int n = MAX_SIZE):_n(n){
        if(_n < 0 ||  _n > MAX_SIZE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(Monoid.e());
    }

    explicit constexpr Segtree(const std::vector<S>& v):_n((int)v.size()){
        if(v.size() > MAX_SIZE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        data.fill(Monoid.e());
        for(int k = 0; k < _n; k++){
            data[k + sz] = v[k];
        }
        for(int k = sz - 1; k > 0; k--){
            data[k] = Monoid.op(data[2 * k], data[2 * k + 1]);
        }
    }

    void set(int k, const S& x){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (set).");
        }
        k += sz;
        data[k] = x;
        while(k >>= 1){
            data[k] = Monoid.op(data[2 * k], data[2 * k + 1]);
        }
    }

    S get(int k)const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }
        return data[k + sz];
    }

    S prod(int l, int r)const{
        if(l < 0 || l > r || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prod).");
        }
        S sml = Monoid.e();
        S smr = Monoid.e();
        l += sz;
        r += sz;
        while(l < r){
            if(l & 1){
                sml = Monoid.op(sml, data[l++]);
            }
            if(r & 1){
                smr = Monoid.op(data[--r], smr);
            }
            l >>= 1;
            r >>= 1;
        }
        return Monoid.op(sml, smr);
    }

    S all_prod()const{
        return data[1];
    }

    int max_right(int l, auto f, auto&&... args)const{
        if(l < 0 || _n < l)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (max_right).");
        }
        if(!std::invoke(f, Monoid.e(), args...))[[unlikely]]{
            throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        }
        if(l == _n) return _n;
        S sm = Monoid.e();
        l += sz;
        do {
            while((l & 1) == 0) {
                l >>= 1;
            }
            if(!std::invoke(f, Monoid.op(sm, data[l]), args...)) {
                while(l < sz) {
                    l <<= 1;
                    if(std::invoke(f, Monoid.op(sm, data[l]), args...)) {
                        sm = Monoid.op(sm, data[l]);
                        l++;
                    }
                }
                return l - sz;
            }
            sm = Monoid.op(sm, data[l]);
            l++;
        } while((l & -l) != l);
        return _n;
    }

    int min_left(int r, auto f, auto&&... args) const {
        if(r < 0 || _n < r) [[unlikely]] {
            throw std::runtime_error("library assertion fault: range violation (min_left).");
        }
        if(!std::invoke(f, Monoid.e(), args...)) [[unlikely]] {
            throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        }
        if(r == 0) return 0;
        S sm = Monoid.e();
        r += sz;
        do {
            r--;
            while(r > 1 && (r & 1)) {
                r >>= 1;
            }
            if(!std::invoke(f,Monoid.op(data[r], sm), args...)) {
                while(r < sz) {
                    r = 2 * r + 1;
                    if(std::invoke(f,Monoid.op(data[r], sm), args...)) {
                        sm = Monoid.op(data[r], sm);
                        r--;
                    }
                }
                return r + 1 - sz;
            }
            sm = Monoid.op(data[r], sm);
        } while((r & -r) != r);
        return 0;
    }
};