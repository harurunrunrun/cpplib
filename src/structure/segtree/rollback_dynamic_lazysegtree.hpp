#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "../types/monoid_act_len.hpp"

template<auto Monoid, long long MAX_SIZE, int MAX_NODES, int MAX_CHANGES>
struct RollbackDynamicLazySegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFFLL);
    static_assert(MAX_NODES >= 0 && MAX_CHANGES >= 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid)>::T;
    using Snapshot = int;

private:
    struct Node{
        int left = -1;
        int right = -1;
        S value;
        T lazy;
        Node(const S& value_, const T& lazy_): value(value_), lazy(lazy_){}
    };
    struct Requirements{ int nodes = 0; int states = 0; };
    std::array<std::optional<Node>, MAX_NODES> nodes;
    std::array<char, MAX_CHANGES> change_kind{};
    std::array<int, MAX_CHANGES> change_node{};
    std::array<int, MAX_CHANGES> change_old_child{};
    std::array<std::optional<S>, MAX_CHANGES> change_old_value;
    std::array<std::optional<T>, MAX_CHANGES> change_old_lazy;
    std::array<int, MAX_CHANGES> operation_node_snapshot{};
    std::array<int, MAX_CHANGES> operation_change_snapshot{};
    std::array<int, MAX_CHANGES> operation_old_root{};
    long long _n;
    int root = -1;
    int node_count = 0;
    int change_count = 0;
    int operation_count = 0;
    int node_snapshot = 0;

    int left_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->left; }
    int right_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->right; }
    S value_of(int node) const{ return node == -1 ? Monoid.e() : nodes[static_cast<std::size_t>(node)]->value; }
    T lazy_of(int node) const{ return node == -1 ? Monoid.id() : nodes[static_cast<std::size_t>(node)]->lazy; }
    int new_node(){
        const int result = node_count++;
        nodes[static_cast<std::size_t>(result)].emplace(Monoid.e(), Monoid.id());
        return result;
    }
    void write_state(int node, const S& value, const T& lazy){
        const int change = change_count++;
        change_kind[static_cast<std::size_t>(change)] = 0;
        change_node[static_cast<std::size_t>(change)] = node;
        change_old_value[static_cast<std::size_t>(change)].emplace(nodes[static_cast<std::size_t>(node)]->value);
        change_old_lazy[static_cast<std::size_t>(change)].emplace(nodes[static_cast<std::size_t>(node)]->lazy);
        nodes[static_cast<std::size_t>(node)]->value = value;
        nodes[static_cast<std::size_t>(node)]->lazy = lazy;
    }
    void attach(int parent, bool right, int child){
        if(parent < 0)[[unlikely]] throw std::runtime_error("library assertion fault: invalid parent (update).");
        if(parent < node_snapshot){
            const int change = change_count++;
            change_kind[static_cast<std::size_t>(change)] = static_cast<char>(right ? 2 : 1);
            change_node[static_cast<std::size_t>(change)] = parent;
            change_old_child[static_cast<std::size_t>(change)] = right ? right_of(parent) : left_of(parent);
        }
        if(right) nodes[static_cast<std::size_t>(parent)]->right = child;
        else nodes[static_cast<std::size_t>(parent)]->left = child;
    }
    void restore_changes(int snapshot){
        while(snapshot < change_count){
            const int change = --change_count;
            const int node = change_node[static_cast<std::size_t>(change)];
            if(change_kind[static_cast<std::size_t>(change)] == 0){
                nodes[static_cast<std::size_t>(node)]->value = *change_old_value[static_cast<std::size_t>(change)];
                nodes[static_cast<std::size_t>(node)]->lazy = *change_old_lazy[static_cast<std::size_t>(change)];
                change_old_value[static_cast<std::size_t>(change)].reset();
                change_old_lazy[static_cast<std::size_t>(change)].reset();
            }else if(change_kind[static_cast<std::size_t>(change)] == 1){
                nodes[static_cast<std::size_t>(node)]->left = change_old_child[static_cast<std::size_t>(change)];
            }else{
                nodes[static_cast<std::size_t>(node)]->right = change_old_child[static_cast<std::size_t>(change)];
            }
        }
    }
    void reset_nodes(int snapshot){ while(snapshot < node_count) nodes[static_cast<std::size_t>(--node_count)].reset(); }

    void all_apply(int node, const T& action, long long length){
        write_state(
            node,
            Monoid.mapping(action, value_of(node), length),
            Monoid.composition(action, lazy_of(node))
        );
    }
    void push(int node, long long left, long long right){
        const long long middle = left + (right - left) / 2;
        int left_child = left_of(node);
        int right_child = right_of(node);
        if(left_child == -1){ left_child = new_node(); attach(node, false, left_child); }
        if(right_child == -1){ right_child = new_node(); attach(node, true, right_child); }
        const T action = lazy_of(node);
        const S value = value_of(node);
        all_apply(left_child, action, middle - left);
        all_apply(right_child, action, right - middle);
        write_state(node, value, Monoid.id());
    }
    void update(int node, long long left, long long right){
        const long long middle = left + (right - left) / 2;
        write_state(
            node,
            Monoid.op(value_of(left_of(node)), middle - left, value_of(right_of(node)), right - middle),
            lazy_of(node)
        );
    }
    void set_impl(int node, long long left, long long right, long long index, const S& value){
        if(right - left == 1){ write_state(node, value, Monoid.id()); return; }
        push(node, left, right);
        const long long middle = left + (right - left) / 2;
        if(index < middle) set_impl(left_of(node), left, middle, index, value);
        else set_impl(right_of(node), middle, right, index, value);
        update(node, left, right);
    }
    void apply_impl(int node, long long left, long long right, long long query_left, long long query_right, const T& action){
        if(query_left <= left && right <= query_right){ all_apply(node, action, right - left); return; }
        push(node, left, right);
        const long long middle = left + (right - left) / 2;
        if(query_left < middle) apply_impl(left_of(node), left, middle, query_left, query_right, action);
        if(middle < query_right) apply_impl(right_of(node), middle, right, query_left, query_right, action);
        update(node, left, right);
    }

    Requirements set_requirements(int node, bool exists, long long left, long long right, long long index) const{
        Requirements result;
        if(!exists) ++result.nodes;
        if(right - left == 1){ result.states = 1; return result; }
        int left_child = exists && node != -1 ? left_of(node) : -1;
        int right_child = exists && node != -1 ? right_of(node) : -1;
        if(left_child == -1) ++result.nodes;
        if(right_child == -1) ++result.nodes;
        result.states = 4;
        const long long middle = left + (right - left) / 2;
        Requirements child = index < middle
            ? set_requirements(left_child, true, left, middle, index)
            : set_requirements(right_child, true, middle, right, index);
        result.nodes += child.nodes; result.states += child.states;
        return result;
    }
    Requirements apply_requirements(int node, bool exists, long long left, long long right, long long query_left, long long query_right) const{
        Requirements result;
        if(!exists) ++result.nodes;
        if(query_left <= left && right <= query_right){ result.states = 1; return result; }
        int left_child = exists && node != -1 ? left_of(node) : -1;
        int right_child = exists && node != -1 ? right_of(node) : -1;
        if(left_child == -1) ++result.nodes;
        if(right_child == -1) ++result.nodes;
        result.states = 4;
        const long long middle = left + (right - left) / 2;
        if(query_left < middle){
            Requirements child = apply_requirements(left_child, true, left, middle, query_left, query_right);
            result.nodes += child.nodes; result.states += child.states;
        }
        if(middle < query_right){
            Requirements child = apply_requirements(right_child, true, middle, right, query_left, query_right);
            result.nodes += child.nodes; result.states += child.states;
        }
        return result;
    }

    S eval(int node, const T& carry, long long length) const{ return Monoid.mapping(carry, value_of(node), length); }
    T next_carry(int node, const T& carry) const{ return Monoid.composition(carry, lazy_of(node)); }
    std::pair<S, long long> prod_impl(int node, long long left, long long right, long long query_left, long long query_right, const T& carry) const{
        if(right <= query_left || query_right <= left) return {Monoid.e(), 0};
        if(query_left <= left && right <= query_right) return {eval(node, carry, right - left), right - left};
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, carry);
        auto a = prod_impl(left_of(node), left, middle, query_left, query_right, next);
        auto b = prod_impl(right_of(node), middle, right, query_left, query_right, next);
        return {Monoid.op(a.first, a.second, b.first, b.second), a.second + b.second};
    }
    template<class F, class... Args>
    long long max_right_impl(int node, long long left, long long right, long long query_left, const T& carry, S& sum, long long& length, F& predicate, Args&... args) const{
        if(right <= query_left) return -1;
        if(query_left <= left){
            S value = eval(node, carry, right - left);
            S next = Monoid.op(sum, length, value, right - left);
            if(std::invoke(predicate, next, args...)){ sum = next; length += right - left; return -1; }
            if(right - left == 1) return left;
        }
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, carry);
        long long result = max_right_impl(left_of(node), left, middle, query_left, next, sum, length, predicate, args...);
        if(result != -1) return result;
        return max_right_impl(right_of(node), middle, right, query_left, next, sum, length, predicate, args...);
    }
    template<class F, class... Args>
    long long min_left_impl(int node, long long left, long long right, long long query_right, const T& carry, S& sum, long long& length, F& predicate, Args&... args) const{
        if(query_right <= left) return -1;
        if(right <= query_right){
            S value = eval(node, carry, right - left);
            S next = Monoid.op(value, right - left, sum, length);
            if(std::invoke(predicate, next, args...)){ sum = next; length += right - left; return -1; }
            if(right - left == 1) return right;
        }
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, carry);
        long long result = min_left_impl(right_of(node), middle, right, query_right, next, sum, length, predicate, args...);
        if(result != -1) return result;
        return min_left_impl(left_of(node), left, middle, query_right, next, sum, length, predicate, args...);
    }

    template<class F>
    void perform_update(const Requirements& required, F&& function){
        if(operation_count == MAX_CHANGES)[[unlikely]] throw std::runtime_error("library assertion fault: operation capacity exceeded (update).");
        if(static_cast<long long>(node_count) + required.nodes > MAX_NODES)[[unlikely]] throw std::runtime_error("library assertion fault: node capacity exceeded (update).");
        if(static_cast<long long>(change_count) + required.states + required.nodes > MAX_CHANGES)[[unlikely]] throw std::runtime_error("library assertion fault: history capacity exceeded (update).");
        node_snapshot = node_count;
        const int old_root = root;
        const int old_changes = change_count;
        try{
            if(root == -1 && required.nodes != 0) root = new_node();
            std::forward<F>(function)();
        }catch(...){
            restore_changes(old_changes);
            root = old_root;
            reset_nodes(node_snapshot);
            throw;
        }
        const int operation = operation_count++;
        operation_node_snapshot[static_cast<std::size_t>(operation)] = node_snapshot;
        operation_change_snapshot[static_cast<std::size_t>(operation)] = old_changes;
        operation_old_root[static_cast<std::size_t>(operation)] = old_root;
    }
    void undo_one(){
        const int operation = --operation_count;
        restore_changes(operation_change_snapshot[static_cast<std::size_t>(operation)]);
        root = operation_old_root[static_cast<std::size_t>(operation)];
        reset_nodes(operation_node_snapshot[static_cast<std::size_t>(operation)]);
    }

public:
    explicit RollbackDynamicLazySegtree(long long n = MAX_SIZE): _n(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
    }
    RollbackDynamicLazySegtree(const RollbackDynamicLazySegtree&) = delete;
    RollbackDynamicLazySegtree& operator=(const RollbackDynamicLazySegtree&) = delete;
    long long size() const{ return _n; }
    int history_size() const{ return operation_count; }
    int nodes_used() const{ return node_count; }
    int changes_used() const{ return change_count; }
    bool can_undo() const{ return operation_count != 0; }
    void set(long long index, const S& value){
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        Requirements required = set_requirements(root, root != -1, 0, _n, index);
        perform_update(required, [&]{ set_impl(root, 0, _n, index, value); });
    }
    void apply(long long index, const T& action){
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (apply).");
        apply(index, index + 1, action);
    }
    void apply(long long left, long long right, const T& action){
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (apply).");
        if(left == right){ perform_update({}, []{}); return; }
        Requirements required = apply_requirements(root, root != -1, 0, _n, left, right);
        perform_update(required, [&]{ apply_impl(root, 0, _n, left, right, action); });
    }
    S get(long long index) const{
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        return prod(index, index + 1);
    }
    S prod(long long left, long long right) const{
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (prod).");
        if(left == right) return Monoid.e();
        return prod_impl(root, 0, _n, left, right, Monoid.id()).first;
    }
    S all_prod() const{ return value_of(root); }
    long long max_right(long long left, auto predicate, auto&&... args) const{
        if(left < 0 || _n < left)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (max_right).");
        S sum = Monoid.e(); long long length = 0;
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        if(left == _n) return _n;
        long long result = max_right_impl(root, 0, _n, left, Monoid.id(), sum, length, predicate, args...);
        return result == -1 ? _n : result;
    }
    long long min_left(long long right, auto predicate, auto&&... args) const{
        if(right < 0 || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (min_left).");
        S sum = Monoid.e(); long long length = 0;
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        if(right == 0) return 0;
        long long result = min_left_impl(root, 0, _n, right, Monoid.id(), sum, length, predicate, args...);
        return result == -1 ? 0 : result;
    }
    Snapshot snapshot() const{ return operation_count; }
    void undo(){ if(!can_undo())[[unlikely]] throw std::runtime_error("library assertion fault: undo history is empty (undo)."); undo_one(); }
    void rollback(Snapshot target){
        if(target < 0 || operation_count < target)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (rollback).");
        while(target < operation_count) undo_one();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED
