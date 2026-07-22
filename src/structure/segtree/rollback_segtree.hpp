#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_SEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_SEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../types/monoid.hpp"

template<auto Monoid, int MAX_SIZE, int MAX_UPDATE>
struct RollbackSegtree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= 0);

    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
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
        static_cast<std::size_t>(height) + 1;

    static_assert(
        static_cast<std::size_t>(MAX_UPDATE) <=
        std::numeric_limits<std::size_t>::max() / changes_per_update
    );

    static constexpr std::size_t history_capacity =
        static_cast<std::size_t>(MAX_UPDATE) * changes_per_update;

    static_assert(
        history_capacity <=
        static_cast<std::size_t>(std::numeric_limits<int>::max())
    );

    struct History{
        int tree_index;
        int old_value_ref;
    };

    int _n = 0;
    int operation_count = 0;
    int node_history_size = 0;

    std::array<std::optional<S>, 2 * sz> initial;
    std::array<int, 2 * sz> current_value_ref;
    std::array<std::optional<S>, history_capacity> updated_values;
    std::array<History, history_capacity> history;
    std::array<int, MAX_UPDATE> operation_history;

    static int initial_value_ref(int tree_index){
        return -tree_index - 1;
    }

    void initialize(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        _n = n;
        for(int k = 0; k < 2 * sz; k++){
            initial[k].emplace(Monoid.e());
            current_value_ref[k] = initial_value_ref(k);
        }
    }

    void build(){
        for(int k = sz - 1; k > 0; k--){
            S value = Monoid.op(*initial[2 * k], *initial[2 * k + 1]);
            initial[k].reset();
            initial[k].emplace(value);
        }
    }

    const S& current_value(int tree_index) const{
        int value_ref = current_value_ref[tree_index];
        if(value_ref < 0){
            return *initial[-value_ref - 1];
        }
        return *updated_values[value_ref];
    }

    void write(int tree_index, const S& value){
        if(
            static_cast<std::size_t>(node_history_size) >= history_capacity
        )[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (set)."
            );
        }

        updated_values[node_history_size].emplace(value);
        history[node_history_size] = {
            tree_index,
            current_value_ref[tree_index]
        };
        current_value_ref[tree_index] = node_history_size;
        node_history_size++;
    }

    void restore_nodes(int snapshot){
        while(snapshot < node_history_size){
            --node_history_size;
            const History& entry = history[node_history_size];
            current_value_ref[entry.tree_index] = entry.old_value_ref;
            updated_values[node_history_size].reset();
        }
    }

    void undo_one(){
        --operation_count;
        restore_nodes(operation_history[operation_count]);
    }

public:
    RollbackSegtree(const RollbackSegtree&) = delete;
    RollbackSegtree& operator=(const RollbackSegtree&) = delete;
    RollbackSegtree(RollbackSegtree&&) = delete;
    RollbackSegtree& operator=(RollbackSegtree&&) = delete;

    explicit RollbackSegtree(int n = MAX_SIZE){
        initialize(n);
        build();
    }

    explicit RollbackSegtree(const std::vector<S>& v){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        initialize(static_cast<int>(v.size()));
        for(int k = 0; k < _n; k++){
            initial[k + sz].reset();
            initial[k + sz].emplace(v[static_cast<std::size_t>(k)]);
        }
        build();
    }

    template<std::size_t N>
    explicit RollbackSegtree(const std::array<S, N>& v){
        if(N > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        initialize(static_cast<int>(N));
        for(int k = 0; k < _n; k++){
            initial[k + sz].reset();
            initial[k + sz].emplace(v[static_cast<std::size_t>(k)]);
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
        if(operation_count == MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (set)."
            );
        }

        int snapshot = node_history_size;
        try{
            k += sz;
            write(k, x);
            while(k >>= 1){
                S value = Monoid.op(
                    current_value(2 * k),
                    current_value(2 * k + 1)
                );
                write(k, value);
            }
        }catch(...){
            restore_nodes(snapshot);
            throw;
        }

        operation_history[operation_count++] = snapshot;
    }

    S get(int k) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (get)."
            );
        }
        return current_value(k + sz);
    }

    S prod(int l, int r) const{
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (prod)."
            );
        }

        S sml = Monoid.e();
        S smr = Monoid.e();
        l += sz;
        r += sz;
        while(l < r){
            if(l & 1){
                sml = Monoid.op(sml, current_value(l++));
            }
            if(r & 1){
                smr = Monoid.op(current_value(--r), smr);
            }
            l >>= 1;
            r >>= 1;
        }
        return Monoid.op(sml, smr);
    }

    S all_prod() const{
        return current_value(1);
    }

    int max_right(int l, auto f, auto&&... args) const{
        if(l < 0 || _n < l)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (max_right)."
            );
        }
        if(!std::invoke(f, Monoid.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (max_right)."
            );
        }
        if(l == _n){
            return _n;
        }

        S sm = Monoid.e();
        l += sz;
        do{
            while((l & 1) == 0){
                l >>= 1;
            }
            if(!std::invoke(f, Monoid.op(sm, current_value(l)), args...)){
                while(l < sz){
                    l <<= 1;
                    if(std::invoke(
                        f, Monoid.op(sm, current_value(l)), args...
                    )){
                        sm = Monoid.op(sm, current_value(l));
                        l++;
                    }
                }
                return l - sz;
            }
            sm = Monoid.op(sm, current_value(l));
            l++;
        }while((l & -l) != l);
        return _n;
    }

    int min_left(int r, auto f, auto&&... args) const{
        if(r < 0 || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (min_left)."
            );
        }
        if(!std::invoke(f, Monoid.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (min_left)."
            );
        }
        if(r == 0){
            return 0;
        }

        S sm = Monoid.e();
        r += sz;
        do{
            r--;
            while(r > 1 && (r & 1)){
                r >>= 1;
            }
            if(!std::invoke(f, Monoid.op(current_value(r), sm), args...)){
                while(r < sz){
                    r = 2 * r + 1;
                    if(std::invoke(
                        f, Monoid.op(current_value(r), sm), args...
                    )){
                        sm = Monoid.op(current_value(r), sm);
                        r--;
                    }
                }
                return r + 1 - sz;
            }
            sm = Monoid.op(current_value(r), sm);
        }while((r & -r) != r);
        return 0;
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
        undo_one();
    }

    void rollback(Snapshot snapshot){
        if(snapshot < 0 || operation_count < snapshot)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rollback)."
            );
        }
        while(snapshot < operation_count){
            undo_one();
        }
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_SEGTREE_HPP_INCLUDED
