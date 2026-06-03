#pragma once

#include <array>
#include <vector>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include "../types/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE>
struct UndoLazySegtree{
    static_assert(MAX_SIZE > 0);

    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;
    using Snapshot = std::pair<int, int>;

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

    struct DataHistory{
        int k;
        S old_value;
    };

    struct LazyHistory{
        int k;
        T old_value;
    };

    std::vector<DataHistory> data_history;
    std::vector<LazyHistory> lazy_history;

    // set/apply 1 回分の開始時点
    std::vector<Snapshot> operation_history;

    Snapshot current_snapshot() const{
        return Snapshot((int)data_history.size(), (int)lazy_history.size());
    }

    void restore(Snapshot s){
        while((int)data_history.size() > s.first){
            auto h = data_history.back();
            data_history.pop_back();
            data[h.k] = h.old_value;
        }

        while((int)lazy_history.size() > s.second){
            auto h = lazy_history.back();
            lazy_history.pop_back();
            lazy[h.k] = h.old_value;
        }
    }

    void save_operation(){
        operation_history.push_back(current_snapshot());
    }

    void set_data(int k, const S& x){
        data_history.push_back({k, data[k]});
        data[k] = x;
    }

    void set_lazy(int k, const T& x){
        lazy_history.push_back({k, lazy[k]});
        lazy[k] = x;
    }

    void update(int k){
        set_data(k, Monoid_act.op(data[2 * k], data[2 * k + 1]));
    }

    void all_apply(int k, const T& f){
        set_data(k, Monoid_act.mapping(f, data[k]));

        if(k < sz){
            set_lazy(k, Monoid_act.composition(f, lazy[k]));
        }
    }

    void push(int k){
        all_apply(2 * k, lazy[k]);
        all_apply(2 * k + 1, lazy[k]);
        set_lazy(k, Monoid_act.id());
    }

public:
    explicit UndoLazySegtree(int n = MAX_SIZE): _n(n){
        if(_n < 0 || _n > MAX_SIZE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }

        data.fill(Monoid_act.e());
        lazy.fill(Monoid_act.id());
    }

    explicit UndoLazySegtree(const std::vector<S>& v): _n((int)v.size()){
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

    bool can_undo() const{
        return !operation_history.empty();
    }

    void undo(){
        if(operation_history.empty())[[unlikely]]{
            throw std::runtime_error("library assertion fault: undo history is empty (undo).");
        }

        Snapshot s = operation_history.back();
        operation_history.pop_back();
        restore(s);
    }

    Snapshot snapshot() const{
        return current_snapshot();
    }

    void rollback(Snapshot s){
        restore(s);
    }

    void set(int k, const S& x){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (set).");
        }

        save_operation();

        k += sz;

        for(int i = height; i > 0; i--){
            push(k >> i);
        }

        set_data(k, x);

        while(k >>= 1){
            update(k);
        }
    }

    S get(int k){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (get).");
        }

        Snapshot s = current_snapshot();

        S res = [&]{
            k += sz;

            for(int i = height; i > 0; i--){
                push(k >> i);
            }

            return data[k];
        }();

        restore(s);
        return res;
    }

    S prod(int l, int r){
        if(l < 0 || l > r || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prod).");
        }

        Snapshot s = current_snapshot();

        S res = [&]{
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
        }();

        restore(s);
        return res;
    }

    S all_prod() const{
        return data[1];
    }

    void apply(int k, const T& f){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (apply).");
        }

        save_operation();

        k += sz;

        for(int i = height; i > 0; i--){
            push(k >> i);
        }

        set_data(k, Monoid_act.mapping(f, data[k]));

        while(k >>= 1){
            update(k);
        }
    }

    void apply(int l, int r, const T& f){
        if(l < 0 || l > r || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (apply).");
        }

        save_operation();

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

        [this, &f](int l, int r){
            while(l < r){
                if(l & 1){
                    all_apply(l++, f);
                }
                if(r & 1){
                    all_apply(--r, f);
                }
                l >>= 1;
                r >>= 1;
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

        Snapshot s = current_snapshot();

        int res = [&]() -> int {
            l += sz;

            for(int i = height; i > 0; i--){
                push(l >> i);
            }

            S sm = Monoid_act.e();

            do {
                while((l & 1) == 0){
                    l >>= 1;
                }

                if(!std::invoke(g, Monoid_act.op(sm, data[l]), args...)){
                    while(l < sz){
                        push(l);
                        l <<= 1;

                        if(std::invoke(g, Monoid_act.op(sm, data[l]), args...)){
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
        }();

        restore(s);
        return res;
    }

    int min_left(int r, auto g, auto&&... args){
        if(r < 0 || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (min_left).");
        }

        if(!std::invoke(g, Monoid_act.e(), args...))[[unlikely]]{
            throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        }

        if(r == 0) return 0;

        Snapshot s = current_snapshot();

        int res = [&]() -> int {
            r += sz;

            for(int i = height; i > 0; i--){
                push((r - 1) >> i);
            }

            S sm = Monoid_act.e();

            do {
                r--;

                while(r > 1 && (r & 1)){
                    r >>= 1;
                }

                if(!std::invoke(g, Monoid_act.op(data[r], sm), args...)){
                    while(r < sz){
                        push(r);
                        r = 2 * r + 1;

                        if(std::invoke(g, Monoid_act.op(data[r], sm), args...)){
                            sm = Monoid_act.op(data[r], sm);
                            r--;
                        }
                    }

                    return r + 1 - sz;
                }

                sm = Monoid_act.op(data[r], sm);

            } while((r & -r) != r);

            return 0;
        }();

        restore(s);
        return res;
    }
};
