#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_LAZYSEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_LAZYSEGTREE_HPP_INCLUDED
#include <array>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include "../types/monoid_act.hpp"


template<auto Monoid_act, int MAX_SIZE>
struct LazySegtree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;
    static constexpr int sz = []{
        int _sz = 1;
        while(_sz < MAX_SIZE){
            _sz <<= 1;
        }
        return _sz;
    }();

    static constexpr int height = []{
        int _sz = 1;
        int _height = 0;
        while(_sz < MAX_SIZE){
            _sz <<= 1;
            _height++;
        }
        return _height;
    }();

    private:

        int _n;
        
        std::array<S, 2 * sz> data;
        std::array<T, sz> lazy;

        void update(int k){
            data[k] = Monoid_act.op(data[2 * k], data[2 * k + 1]);
        }
        void all_apply(int k, T f){
            data[k] = Monoid_act.mapping(f, data[k]);
            if(k < sz){
                lazy[k] = Monoid_act.composition(f, lazy[k]);
            }
        }
        void push(int k){
            all_apply(2 * k, lazy[k]);
            all_apply(2 * k + 1, lazy[k]);
            lazy[k] = Monoid_act.id();
        }

    public:
    
        explicit constexpr LazySegtree(int n = MAX_SIZE):_n(n){
            if(_n < 0 ||  _n > MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (constructor).");
            }
            data.fill(Monoid_act.e());
            lazy.fill(Monoid_act.id());
        }

        explicit constexpr LazySegtree(const std::vector<S>& v):_n((int)v.size()){
            if(v.size() > MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (constructor).");
            }
            data.fill(Monoid_act.e());
            lazy.fill(Monoid_act.id());
            for(int k = 0; k < _n; k++){
                data[k + sz] = v[k];
            }
            for(int k = sz - 1; k > 0; k--){
                data[k] = Monoid_act.op(data[2 * k], data[2 * k + 1]);
            }
        }

        void set(int k, const S& x){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (set).");
            }
            k += sz;
            for(int i = height; i > 0; i--){
                push(k >> i);
            }
            data[k] = x;
            while(k >>= 1){
                update(k);
            }
        }

        S get(int k){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (get).");
            }
            k += sz;
            for(int i = height; i > 0; i--){
                push(k >> i);
            }
            return data[k];
        }

        S prod(int l, int r){
            if(l < 0 || l > r || _n < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (prod).");
            }
            S sml = Monoid_act.e();
            S smr = Monoid_act.e();
            l += sz;
            r += sz;
            for(int i = height; i > 0; i--){
                if(((l >> i) << i) != l){
                    push(l >> i);
                }
                if(((r >> i) << i) != r){
                    push((r - 1) >> i);
                }
            }
            while(l < r){
                if(l & 1){
                    sml = Monoid_act.op(sml, data[l++]);
                }
                if(r & 1){
                    smr = Monoid_act.op(data[--r], smr);
                }
                l >>= 1;
                r >>= 1;
            }
            return Monoid_act.op(sml, smr);
        }

        S all_prod()const{
            return data[1];
        }

        void apply(int k, const T& f){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (apply).");
            }
            k += sz;
            for(int i = height; i > 0; i--){
                push(k >> i);
            }
            data[k] = Monoid_act.mapping(f, data[k]);
            while(k >>= 1){
                update(k);
            }
        }

        void apply(int l, int r, const T& f){
            if(l < 0 || l > r || _n < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (apply).");
            }
            if(l == r){
                return;
            }
            l += sz;
            r += sz;
            for(int i = height; i > 0; i--){
                if(((l >> i) << i) != l){
                    push(l >> i);
                }
                if(((r >> i) << i) != r){
                    push((r - 1) >> i);
                }
            }
            [this, &f](int left, int right){
                while(left < right){
                    if(left & 1){
                        all_apply(left++, f);
                    }
                    if(right & 1){
                        all_apply(--right, f);
                    }
                    left >>= 1;
                    right >>= 1;
                }
            }(l, r);
            for(int i = 1; i <= height; i++){
                if(((l >> i) << i) != l){
                    update(l >> i);
                }
                if(((r >> i) << i) != r){
                    update((r - 1) >> i);
                }
            }
        }

        int max_right(int l, auto g, auto&&... args){
            if(l < 0 || _n < l)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (max_right).");
            }
            if(!std::invoke(g, Monoid_act.e(), args...))[[unlikely]]{
                throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
            }
            if(l == _n) return _n;
            l += sz;
            for(int i = height; i > 0; i--){
                push(l >> i);
            }
            S sm = Monoid_act.e();
            do {
                while((l & 1) == 0) {
                    l >>= 1;
                }
                if(!std::invoke(g, Monoid_act.op(sm, data[l]), args...)) {
                    while(l < sz) {
                        push(l);
                        l <<= 1;
                        if(std::invoke(g, Monoid_act.op(sm, data[l]), args...)) {
                            sm = Monoid_act.op(sm, data[l]);
                            l++;
                        }
                    }
                    return l - sz;
                }
                sm = Monoid_act.op(sm, data[l]);
                l++;
            } while((l & -l) != l);
            return _n;
        }

        int min_left(int r, auto g, auto&&... args){
            if(r < 0 || _n < r) [[unlikely]] {
                throw std::runtime_error("library assertion fault: range violation (min_left).");
            }
            if(!std::invoke(g, Monoid_act.e(), args...)) [[unlikely]] {
                throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
            }
            if(r == 0) return 0;
            r += sz;
            for(int i = height; i > 0; i--){
                push((r - 1) >> i);
            }
            S sm = Monoid_act.e();
            do {
                r--;
                while(r > 1 && (r & 1)) {
                    r >>= 1;
                }
                if(!std::invoke(g, Monoid_act.op(data[r], sm), args...)) {
                    while(r < sz) {
                        push(r);
                        r = 2 * r + 1;
                        if(std::invoke(g, Monoid_act.op(data[r], sm), args...)) {
                            sm = Monoid_act.op(data[r], sm);
                            r--;
                        }
                    }
                    return r + 1 - sz;
                }
                sm = Monoid_act.op(data[r], sm);
            } while((r & -r) != r);
            return 0;
        }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_LAZYSEGTREE_HPP_INCLUDED
