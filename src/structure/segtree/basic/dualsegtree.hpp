#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_BASIC_DUALSEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_BASIC_DUALSEGTREE_HPP_INCLUDED
#include "../../array/static_or_vector.hpp"
#include <array>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include "../../types/monoid/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE = -1>
struct DualSegtree{
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

        std::array<S, sz> data;
        std::array<T, 2 * sz> lazy;

        void push(int k){
            for(int i = height; i > 0; i--){
                lazy[(k >> i) << 1] = Monoid_act.composition(lazy[k >> i], lazy[(k >> i) << 1]);
                lazy[((k >> i) << 1) + 1] = Monoid_act.composition(lazy[k >> i], lazy[((k >> i) << 1) + 1]);
                lazy[k >> i] = Monoid_act.id();
            }
        }

    public:

        explicit constexpr DualSegtree(int n = MAX_SIZE):_n(n){
            if(_n < 0 ||  _n > MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (constructor).");
            }
            data.fill(Monoid_act.e());
            lazy.fill(Monoid_act.id());
        }

        explicit constexpr DualSegtree(const std::vector<S>& v):_n((int)v.size()){
            if(v.size() > MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (constructor).");
            }
            data.fill(Monoid_act.e());
            lazy.fill(Monoid_act.id());
            for(int k = 0; k < _n; k++){
                data[k] = v[k];
            }
        }

        void set(int k, const S& x){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (set).");
            }
            push(k + sz);
            lazy[k + sz] = Monoid_act.id();
            data[k] = x;
        }

        S get(int k){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (get).");
            }
            push(k + sz);
            data[k] = Monoid_act.mapping(lazy[k + sz], data[k]);
            lazy[k + sz] = Monoid_act.id();
            return data[k];
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
            push(l);
            push(r - 1);
            while(l < r){
                if(l & 1){
                    lazy[l] = Monoid_act.composition(f, lazy[l]);
                    l++;
                }
                if(r & 1){
                    --r;
                    lazy[r] = Monoid_act.composition(f, lazy[r]);
                }
                l >>= 1;
                r >>= 1;
            }
        }
};

// Capacity omitted: storage follows the runtime shape and actual updates.
template<auto Monoid_act>
struct DualSegtree<Monoid_act, -1>{
    void initialize_storage(int n){
        if(n < 0 || n > (1 << 29)) throw std::length_error("segment tree size");
        sz = 1;
        height = 0;
        while(sz < n){ sz <<= 1; ++height; }
        data.resize(sz);
        lazy.resize(2 * sz);
    }

    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;
    int sz = 1;

    int height = 0;

    private:

        int _n;

        cpplib_detail::StaticOrVector<S, -1> data;
        cpplib_detail::StaticOrVector<T, -1> lazy;

        void push(int k){
            for(int i = height; i > 0; i--){
                lazy[(k >> i) << 1] = Monoid_act.composition(lazy[k >> i], lazy[(k >> i) << 1]);
                lazy[((k >> i) << 1) + 1] = Monoid_act.composition(lazy[k >> i], lazy[((k >> i) << 1) + 1]);
                lazy[k >> i] = Monoid_act.id();
            }
        }

    public:

        explicit DualSegtree(int n = 0):_n(n){
        initialize_storage(_n);
            if(_n < 0 )[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (constructor).");
            }
            data.fill(Monoid_act.e());
            lazy.fill(Monoid_act.id());
        }

        explicit DualSegtree(const std::vector<S>& v):_n(cpplib_detail::checked_int_size(v.size())){
        initialize_storage(_n);
            if(false)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (constructor).");
            }
            data.fill(Monoid_act.e());
            lazy.fill(Monoid_act.id());
            for(int k = 0; k < _n; k++){
                data[k] = v[k];
            }
        }

        void set(int k, const S& x){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (set).");
            }
            push(k + sz);
            lazy[k + sz] = Monoid_act.id();
            data[k] = x;
        }

        S get(int k){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (get).");
            }
            push(k + sz);
            data[k] = Monoid_act.mapping(lazy[k + sz], data[k]);
            lazy[k + sz] = Monoid_act.id();
            return data[k];
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
            push(l);
            push(r - 1);
            while(l < r){
                if(l & 1){
                    lazy[l] = Monoid_act.composition(f, lazy[l]);
                    l++;
                }
                if(r & 1){
                    --r;
                    lazy[r] = Monoid_act.composition(f, lazy[r]);
                }
                l >>= 1;
                r >>= 1;
            }
        }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_BASIC_DUALSEGTREE_HPP_INCLUDED
