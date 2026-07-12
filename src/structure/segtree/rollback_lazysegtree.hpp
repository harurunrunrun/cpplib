#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "../types/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_UPDATE>
struct RollbackLazySegtree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= 0);

    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;
    using Snapshot = int;

    static constexpr int sz = []{
        int res = 1;
        while(res < MAX_SIZE){
            res <<= 1;
        }
        return res;
    }();

    static constexpr int height = []{
        int res = 1;
        int h = 0;
        while(res < MAX_SIZE){
            res <<= 1;
            h++;
        }
        return h;
    }();

private:
    static constexpr std::size_t changes_per_update =
        static_cast<std::size_t>(8 * height) + 4;

    static_assert(
        static_cast<std::size_t>(MAX_UPDATE) <=
        std::numeric_limits<std::size_t>::max() / changes_per_update
    );

    static constexpr std::size_t history_capacity =
        static_cast<std::size_t>(MAX_UPDATE) * changes_per_update;

    struct DataHistory{
        int index;
        S value;
    };

    struct LazyHistory{
        int index;
        T value;
    };

    struct Operation{
        int data_size;
        int lazy_size;
    };

    int _n = 0;
    int operation_count = 0;
    int data_history_size = 0;
    int lazy_history_size = 0;

    std::array<S, 2 * sz> data;
    std::array<T, sz> lazy;
    std::array<std::optional<DataHistory>, history_capacity> data_history;
    std::array<std::optional<LazyHistory>, history_capacity> lazy_history;
    std::array<Operation, MAX_UPDATE> operation_history;

    void initialize(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        _n = n;
        data.fill(Monoid_act.e());
        lazy.fill(Monoid_act.id());
    }

    void build(){
        for(int k = sz - 1; k > 0; k--){
            data[k] = Monoid_act.op(data[2 * k], data[2 * k + 1]);
        }
    }

    void write_data(int k, const S& value){
        if(
            static_cast<std::size_t>(data_history_size) >= history_capacity
        )[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (update)."
            );
        }
        data_history[data_history_size].emplace(DataHistory{k, data[k]});
        data[k] = value;
        data_history_size++;
    }

    void write_lazy(int k, const T& value){
        if(
            static_cast<std::size_t>(lazy_history_size) >= history_capacity
        )[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (update)."
            );
        }
        lazy_history[lazy_history_size].emplace(LazyHistory{k, lazy[k]});
        lazy[k] = value;
        lazy_history_size++;
    }

    void restore(Operation snapshot){
        while(snapshot.data_size < data_history_size){
            --data_history_size;
            const auto& entry = *data_history[data_history_size];
            data[entry.index] = entry.value;
            data_history[data_history_size].reset();
        }
        while(snapshot.lazy_size < lazy_history_size){
            --lazy_history_size;
            const auto& entry = *lazy_history[lazy_history_size];
            lazy[entry.index] = entry.value;
            lazy_history[lazy_history_size].reset();
        }
    }

    void update(int k){
        S value = Monoid_act.op(data[2 * k], data[2 * k + 1]);
        write_data(k, value);
    }

    void all_apply(int k, const T& f){
        S value = Monoid_act.mapping(f, data[k]);
        if(k < sz){
            T action = Monoid_act.composition(f, lazy[k]);
            write_data(k, value);
            write_lazy(k, action);
        }else{
            write_data(k, value);
        }
    }

    void push(int k){
        T action = lazy[k];
        all_apply(2 * k, action);
        all_apply(2 * k + 1, action);
        write_lazy(k, Monoid_act.id());
    }

    void set_impl(int k, int l, int r, int p, const S& x){
        if(r - l == 1){
            write_data(k, x);
            return;
        }
        push(k);
        int mid = l + (r - l) / 2;
        if(p < mid){
            set_impl(2 * k, l, mid, p, x);
        }else{
            set_impl(2 * k + 1, mid, r, p, x);
        }
        update(k);
    }

    void apply_impl(
        int k,
        int l,
        int r,
        int ql,
        int qr,
        const T& f
    ){
        if(ql <= l && r <= qr){
            all_apply(k, f);
            return;
        }
        push(k);
        int mid = l + (r - l) / 2;
        if(ql < mid){
            apply_impl(2 * k, l, mid, ql, qr, f);
        }
        if(mid < qr){
            apply_impl(2 * k + 1, mid, r, ql, qr, f);
        }
        update(k);
    }

    S prod_impl(
        int k,
        int l,
        int r,
        int ql,
        int qr,
        const T& carry
    ) const{
        if(r <= ql || qr <= l){
            return Monoid_act.e();
        }
        if(ql <= l && r <= qr){
            return Monoid_act.mapping(carry, data[k]);
        }
        T next = k < sz
            ? Monoid_act.composition(carry, lazy[k])
            : carry;
        int mid = l + (r - l) / 2;
        return Monoid_act.op(
            prod_impl(2 * k, l, mid, ql, qr, next),
            prod_impl(2 * k + 1, mid, r, ql, qr, next)
        );
    }

    template<class F, class... Args>
    int max_right_impl(
        int k,
        int l,
        int r,
        int ql,
        const T& carry,
        S& sm,
        F& f,
        Args&... args
    ) const{
        if(r <= ql || _n <= l){
            return -1;
        }
        if(ql <= l && r <= _n){
            S value = Monoid_act.mapping(carry, data[k]);
            S next_value = Monoid_act.op(sm, value);
            if(std::invoke(f, next_value, args...)){
                sm = next_value;
                return -1;
            }
            if(r - l == 1){
                return l;
            }
        }
        T next = k < sz
            ? Monoid_act.composition(carry, lazy[k])
            : carry;
        int mid = l + (r - l) / 2;
        int res = max_right_impl(
            2 * k, l, mid, ql, next, sm, f, args...
        );
        if(res != -1){
            return res;
        }
        return max_right_impl(
            2 * k + 1, mid, r, ql, next, sm, f, args...
        );
    }

    template<class F, class... Args>
    int min_left_impl(
        int k,
        int l,
        int r,
        int qr,
        const T& carry,
        S& sm,
        F& f,
        Args&... args
    ) const{
        if(qr <= l || _n <= l){
            return -1;
        }
        if(r <= qr && r <= _n){
            S value = Monoid_act.mapping(carry, data[k]);
            S next_value = Monoid_act.op(value, sm);
            if(std::invoke(f, next_value, args...)){
                sm = next_value;
                return -1;
            }
            if(r - l == 1){
                return r;
            }
        }
        T next = k < sz
            ? Monoid_act.composition(carry, lazy[k])
            : carry;
        int mid = l + (r - l) / 2;
        int res = min_left_impl(
            2 * k + 1, mid, r, qr, next, sm, f, args...
        );
        if(res != -1){
            return res;
        }
        return min_left_impl(
            2 * k, l, mid, qr, next, sm, f, args...
        );
    }

    template<class F>
    void perform_update(F&& update_function){
        if(operation_count == MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (update)."
            );
        }
        Operation snapshot{data_history_size, lazy_history_size};
        try{
            std::forward<F>(update_function)();
        }catch(...){
            restore(snapshot);
            throw;
        }
        operation_history[operation_count++] = snapshot;
    }

public:
    RollbackLazySegtree(const RollbackLazySegtree&) = delete;
    RollbackLazySegtree& operator=(const RollbackLazySegtree&) = delete;
    RollbackLazySegtree(RollbackLazySegtree&&) = delete;
    RollbackLazySegtree& operator=(RollbackLazySegtree&&) = delete;

    explicit RollbackLazySegtree(int n = MAX_SIZE){
        initialize(n);
        build();
    }

    explicit RollbackLazySegtree(const std::vector<S>& v){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initialize(static_cast<int>(v.size()));
        for(int k = 0; k < _n; k++){
            data[k + sz] = v[static_cast<std::size_t>(k)];
        }
        build();
    }

    template<std::size_t N>
    explicit RollbackLazySegtree(const std::array<S, N>& v){
        if(N > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initialize(static_cast<int>(N));
        for(int k = 0; k < _n; k++){
            data[k + sz] = v[static_cast<std::size_t>(k)];
        }
        build();
    }

    int size() const{
        return _n;
    }

    int history_size() const{
        return operation_count;
    }

    bool can_undo() const{
        return operation_count != 0;
    }

    void set(int k, const S& x){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (set)."
            );
        }
        perform_update([&]{
            set_impl(1, 0, sz, k, x);
        });
    }

    void apply(int k, const T& f){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (apply)."
            );
        }
        perform_update([&]{
            apply_impl(1, 0, sz, k, k + 1, f);
        });
    }

    void apply(int l, int r, const T& f){
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (apply)."
            );
        }
        perform_update([&]{
            if(l != r){
                apply_impl(1, 0, sz, l, r, f);
            }
        });
    }

    S get(int k) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (get)."
            );
        }
        return prod_impl(1, 0, sz, k, k + 1, Monoid_act.id());
    }

    S prod(int l, int r) const{
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (prod)."
            );
        }
        return prod_impl(1, 0, sz, l, r, Monoid_act.id());
    }

    S all_prod() const{
        return data[1];
    }

    int max_right(int l, auto f, auto&&... args) const{
        if(l < 0 || _n < l)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (max_right)."
            );
        }
        if(!std::invoke(f, Monoid_act.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (max_right)."
            );
        }
        if(l == _n){
            return _n;
        }
        S sm = Monoid_act.e();
        int res = max_right_impl(
            1, 0, sz, l, Monoid_act.id(), sm, f, args...
        );
        return res == -1 ? _n : res;
    }

    int min_left(int r, auto f, auto&&... args) const{
        if(r < 0 || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (min_left)."
            );
        }
        if(!std::invoke(f, Monoid_act.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (min_left)."
            );
        }
        if(r == 0){
            return 0;
        }
        S sm = Monoid_act.e();
        int res = min_left_impl(
            1, 0, sz, r, Monoid_act.id(), sm, f, args...
        );
        return res == -1 ? 0 : res;
    }

    Snapshot snapshot() const{
        return operation_count;
    }

    void undo(){
        if(!can_undo())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: undo history is empty (undo)."
            );
        }
        --operation_count;
        restore(operation_history[operation_count]);
    }

    void rollback(Snapshot snapshot){
        if(snapshot < 0 || operation_count < snapshot)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rollback)."
            );
        }
        while(snapshot < operation_count){
            undo();
        }
    }
};
