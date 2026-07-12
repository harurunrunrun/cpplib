#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "../types/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_UPDATE>
struct RollbackDualSegtree{
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
    static constexpr std::size_t lazy_changes_per_update =
        static_cast<std::size_t>(8 * height) + 4;
    static_assert(
        static_cast<std::size_t>(MAX_UPDATE) <=
        std::numeric_limits<std::size_t>::max() /
            lazy_changes_per_update
    );
    static constexpr std::size_t lazy_history_capacity =
        static_cast<std::size_t>(MAX_UPDATE) *
        lazy_changes_per_update;

    struct LazyHistory{
        int index;
        T value;
    };

    struct DataHistory{
        int index;
        S value;
    };

    struct Operation{
        int lazy_size;
        int data_size;
    };

    int _n = 0;
    int operation_count = 0;
    int lazy_history_size = 0;
    int data_history_size = 0;
    std::array<S, sz> data;
    std::array<T, 2 * sz> lazy;
    std::array<
        std::optional<LazyHistory>,
        lazy_history_capacity
    > lazy_history;
    std::array<std::optional<DataHistory>, MAX_UPDATE> data_history;
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

    void write_lazy(int k, const T& value){
        if(
            static_cast<std::size_t>(lazy_history_size) >=
            lazy_history_capacity
        )[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (update)."
            );
        }
        lazy_history[lazy_history_size].emplace(LazyHistory{k, lazy[k]});
        lazy[k] = value;
        lazy_history_size++;
    }

    void write_data(int k, const S& value){
        if(data_history_size == MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (set)."
            );
        }
        data_history[data_history_size].emplace(DataHistory{k, data[k]});
        data[k] = value;
        data_history_size++;
    }

    void compose(int k, const T& f){
        T action = Monoid_act.composition(f, lazy[k]);
        write_lazy(k, action);
    }

    void push(int k){
        compose(2 * k, lazy[k]);
        compose(2 * k + 1, lazy[k]);
        write_lazy(k, Monoid_act.id());
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
            compose(k, f);
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
    }

    void set_impl(int k, int l, int r, int p, const S& x){
        if(r - l == 1){
            write_data(l, x);
            write_lazy(k, Monoid_act.id());
            return;
        }
        push(k);
        int mid = l + (r - l) / 2;
        if(p < mid){
            set_impl(2 * k, l, mid, p, x);
        }else{
            set_impl(2 * k + 1, mid, r, p, x);
        }
    }

    S get_impl(int k, int l, int r, int p) const{
        if(r - l == 1){
            return Monoid_act.mapping(lazy[k], data[l]);
        }
        int mid = l + (r - l) / 2;
        S value = p < mid
            ? get_impl(2 * k, l, mid, p)
            : get_impl(2 * k + 1, mid, r, p);
        return Monoid_act.mapping(lazy[k], value);
    }

    void restore(Operation snapshot){
        while(snapshot.lazy_size < lazy_history_size){
            --lazy_history_size;
            const auto& entry = *lazy_history[lazy_history_size];
            lazy[entry.index] = entry.value;
            lazy_history[lazy_history_size].reset();
        }
        while(snapshot.data_size < data_history_size){
            --data_history_size;
            const auto& entry = *data_history[data_history_size];
            data[entry.index] = entry.value;
            data_history[data_history_size].reset();
        }
    }

    template<class F>
    void perform_update(F&& function){
        if(operation_count == MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (update)."
            );
        }
        Operation snapshot{lazy_history_size, data_history_size};
        try{
            std::forward<F>(function)();
        }catch(...){
            restore(snapshot);
            throw;
        }
        operation_history[operation_count++] = snapshot;
    }

public:
    explicit RollbackDualSegtree(int n = MAX_SIZE){
        initialize(n);
    }

    explicit RollbackDualSegtree(const std::vector<S>& v){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initialize(static_cast<int>(v.size()));
        for(int k = 0; k < _n; k++){
            data[k] = v[static_cast<std::size_t>(k)];
        }
    }

    template<std::size_t N>
    explicit RollbackDualSegtree(const std::array<S, N>& v){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        initialize(static_cast<int>(N));
        for(int k = 0; k < _n; k++){
            data[k] = v[static_cast<std::size_t>(k)];
        }
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
        return get_impl(1, 0, sz, k);
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
