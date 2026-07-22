#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "../other/partially_persistent_storage.hpp"
#include "../types/monoid_act_len.hpp"

template<auto Monoid, long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentDynamicLazySegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFFLL);
    static_assert(MAX_NODES > 0 && MAX_VERSIONS >= 0 && MAX_CHANGES >= 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid)>::T;

private:
    static constexpr int height = []{
        long long span = MAX_SIZE;
        int result = 0;
        while(span > 1){ span = (span + 1) / 2; ++result; }
        return result;
    }();
    static constexpr int max_links = 4 * height + 1;
    struct State{ S value; T lazy; };
    struct Node{ int left = -1; int right = -1; };
    struct Link{ int node; bool right; };
    struct Requirements{ int nodes = 0; int changes = 0; };

    std::array<Node, MAX_NODES> nodes{};
    PartiallyPersistentStorage<State, MAX_NODES, MAX_CHANGES> state;
    long long _n;
    int root = -1;
    int node_count = 0;
    int version_count = 1;
    std::array<Link, max_links> links{};
    int link_count = 0;
    int node_snapshot = 0;

    int left_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)].left; }
    int right_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)].right; }
    State state_at(int node, int version) const{ return node == -1 ? State{Monoid.e(), Monoid.id()} : state.get(node, version); }
    State current_state(int node) const{ return node == -1 ? State{Monoid.e(), Monoid.id()} : state.current(node); }

    int new_node(){
        const int result = node_count++;
        nodes[static_cast<std::size_t>(result)] = {};
        state.initialize(result, State{Monoid.e(), Monoid.id()});
        return result;
    }
    void attach(int parent, bool right, int child){
        if(parent < 0)[[unlikely]] throw std::runtime_error("library assertion fault: invalid parent (update).");
        if(parent < node_snapshot) links[static_cast<std::size_t>(link_count++)] = {parent, right};
        if(right) nodes[static_cast<std::size_t>(parent)].right = child;
        else nodes[static_cast<std::size_t>(parent)].left = child;
    }
    void write_state(int node, const State& value){ state.write(node, version_count, value); }
    void all_apply(int node, const T& action, long long length){
        State current = current_state(node);
        write_state(node, State{
            Monoid.mapping(action, current.value, length),
            Monoid.composition(action, current.lazy)
        });
    }
    void push(int node, long long left, long long right){
        const long long middle = left + (right - left) / 2;
        int left_child = left_of(node);
        int right_child = right_of(node);
        if(left_child == -1){ left_child = new_node(); attach(node, false, left_child); }
        if(right_child == -1){ right_child = new_node(); attach(node, true, right_child); }
        State current = current_state(node);
        all_apply(left_child, current.lazy, middle - left);
        all_apply(right_child, current.lazy, right - middle);
        write_state(node, State{current.value, Monoid.id()});
    }
    void update(int node, long long left, long long right){
        const long long middle = left + (right - left) / 2;
        State current = current_state(node);
        write_state(node, State{
            Monoid.op(current_state(left_of(node)).value, middle - left, current_state(right_of(node)).value, right - middle),
            current.lazy
        });
    }
    void set_impl(int node, long long left, long long right, long long index, const S& value){
        if(right - left == 1){ write_state(node, State{value, Monoid.id()}); return; }
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
        if(right - left == 1){ result.changes = 1; return result; }
        int left_child = exists && node != -1 ? left_of(node) : -1;
        int right_child = exists && node != -1 ? right_of(node) : -1;
        if(left_child == -1) ++result.nodes;
        if(right_child == -1) ++result.nodes;
        result.changes = 4;
        const long long middle = left + (right - left) / 2;
        Requirements child = index < middle
            ? set_requirements(left_child, true, left, middle, index)
            : set_requirements(right_child, true, middle, right, index);
        result.nodes += child.nodes;
        result.changes += child.changes;
        return result;
    }
    Requirements apply_requirements(int node, bool exists, long long left, long long right, long long query_left, long long query_right) const{
        Requirements result;
        if(!exists) ++result.nodes;
        if(query_left <= left && right <= query_right){ result.changes = 1; return result; }
        int left_child = exists && node != -1 ? left_of(node) : -1;
        int right_child = exists && node != -1 ? right_of(node) : -1;
        if(left_child == -1) ++result.nodes;
        if(right_child == -1) ++result.nodes;
        result.changes = 4;
        const long long middle = left + (right - left) / 2;
        if(query_left < middle){
            Requirements child = apply_requirements(left_child, true, left, middle, query_left, query_right);
            result.nodes += child.nodes; result.changes += child.changes;
        }
        if(middle < query_right){
            Requirements child = apply_requirements(right_child, true, middle, right, query_left, query_right);
            result.nodes += child.nodes; result.changes += child.changes;
        }
        return result;
    }

    S eval(int node, int version, const T& carry, long long length) const{ return Monoid.mapping(carry, state_at(node, version).value, length); }
    T next_carry(int node, int version, const T& carry) const{ return Monoid.composition(carry, state_at(node, version).lazy); }
    std::pair<S, long long> prod_impl(int node, int version, long long left, long long right, long long query_left, long long query_right, const T& carry) const{
        if(right <= query_left || query_right <= left) return {Monoid.e(), 0};
        if(query_left <= left && right <= query_right) return {eval(node, version, carry, right - left), right - left};
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, version, carry);
        auto a = prod_impl(left_of(node), version, left, middle, query_left, query_right, next);
        auto b = prod_impl(right_of(node), version, middle, right, query_left, query_right, next);
        return {Monoid.op(a.first, a.second, b.first, b.second), a.second + b.second};
    }
    template<class F, class... Args>
    long long max_right_impl(int node, int version, long long left, long long right, long long query_left, const T& carry, S& sum, long long& length, F& predicate, Args&... args) const{
        if(right <= query_left) return -1;
        if(query_left <= left){
            S value = eval(node, version, carry, right - left);
            S next = Monoid.op(sum, length, value, right - left);
            if(std::invoke(predicate, next, args...)){ sum = next; length += right - left; return -1; }
            if(right - left == 1) return left;
        }
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, version, carry);
        long long result = max_right_impl(left_of(node), version, left, middle, query_left, next, sum, length, predicate, args...);
        if(result != -1) return result;
        return max_right_impl(right_of(node), version, middle, right, query_left, next, sum, length, predicate, args...);
    }
    template<class F, class... Args>
    long long min_left_impl(int node, int version, long long left, long long right, long long query_right, const T& carry, S& sum, long long& length, F& predicate, Args&... args) const{
        if(query_right <= left) return -1;
        if(right <= query_right){
            S value = eval(node, version, carry, right - left);
            S next = Monoid.op(value, right - left, sum, length);
            if(std::invoke(predicate, next, args...)){ sum = next; length += right - left; return -1; }
            if(right - left == 1) return right;
        }
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, version, carry);
        long long result = min_left_impl(right_of(node), version, middle, right, query_right, next, sum, length, predicate, args...);
        if(result != -1) return result;
        return min_left_impl(left_of(node), version, left, middle, query_right, next, sum, length, predicate, args...);
    }

    void check_version(int version, const char* message) const{ if(version < 0 || version_count <= version)[[unlikely]] throw std::runtime_error(message); }
    template<class F>
    int perform_update(const Requirements& required, F&& function){
        if(version_count > MAX_VERSIONS)[[unlikely]] throw std::runtime_error("library assertion fault: version capacity exceeded (update).");
        if(static_cast<long long>(node_count) + required.nodes > MAX_NODES)[[unlikely]] throw std::runtime_error("library assertion fault: node capacity exceeded (update).");
        if(static_cast<long long>(state.changes()) + required.changes > MAX_CHANGES)[[unlikely]] throw std::runtime_error("library assertion fault: history capacity exceeded (update).");
        node_snapshot = node_count;
        const int state_snapshot = state.changes();
        const int old_root = root;
        link_count = 0;
        try{
            if(root == -1) root = new_node();
            std::forward<F>(function)();
        }catch(...){
            state.rollback(state_snapshot);
            while(link_count > 0){
                const Link link = links[static_cast<std::size_t>(--link_count)];
                if(link.right) nodes[static_cast<std::size_t>(link.node)].right = -1;
                else nodes[static_cast<std::size_t>(link.node)].left = -1;
            }
            root = old_root;
            node_count = node_snapshot;
            throw;
        }
        return version_count++;
    }

public:
    explicit PartiallyPersistentDynamicLazySegtree(long long n = MAX_SIZE): _n(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
    }
    PartiallyPersistentDynamicLazySegtree(const PartiallyPersistentDynamicLazySegtree&) = delete;
    PartiallyPersistentDynamicLazySegtree& operator=(const PartiallyPersistentDynamicLazySegtree&) = delete;
    long long size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }
    int changes_used() const{ return state.changes(); }
    int set(long long index, const S& value){
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        Requirements required = set_requirements(root, root != -1, 0, _n, index);
        return perform_update(required, [&]{ set_impl(root, 0, _n, index, value); });
    }
    int apply(long long index, const T& action){
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (apply).");
        return apply(index, index + 1, action);
    }
    int apply(long long left, long long right, const T& action){
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (apply).");
        if(left == right){
            if(version_count > MAX_VERSIONS)[[unlikely]] throw std::runtime_error("library assertion fault: version capacity exceeded (apply).");
            return version_count++;
        }
        Requirements required = apply_requirements(root, root != -1, 0, _n, left, right);
        return perform_update(required, [&]{ apply_impl(root, 0, _n, left, right, action); });
    }
    S get(int version, long long index) const{
        check_version(version, "library assertion fault: range violation (get).");
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        return prod(version, index, index + 1);
    }
    S get(long long index) const{ return get(latest_version(), index); }
    S prod(int version, long long left, long long right) const{
        check_version(version, "library assertion fault: range violation (prod).");
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (prod).");
        if(left == right) return Monoid.e();
        return prod_impl(root, version, 0, _n, left, right, Monoid.id()).first;
    }
    S prod(long long left, long long right) const{ return prod(latest_version(), left, right); }
    S all_prod(int version) const{ check_version(version, "library assertion fault: range violation (all_prod)."); return state_at(root, version).value; }
    S all_prod() const{ return all_prod(latest_version()); }
    long long max_right(int version, long long left, auto predicate, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (max_right).");
        if(left < 0 || _n < left)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (max_right).");
        S sum = Monoid.e(); long long length = 0;
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        if(left == _n) return _n;
        long long result = max_right_impl(root, version, 0, _n, left, Monoid.id(), sum, length, predicate, args...);
        return result == -1 ? _n : result;
    }
    template<class F, class... Args>
    requires std::is_invocable_v<F, S, Args...>
    long long max_right(long long left, F predicate, Args&&... args) const{
        return max_right(latest_version(), left, predicate, args...);
    }
    long long min_left(int version, long long right, auto predicate, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (min_left).");
        if(right < 0 || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (min_left).");
        S sum = Monoid.e(); long long length = 0;
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        if(right == 0) return 0;
        long long result = min_left_impl(root, version, 0, _n, right, Monoid.id(), sum, length, predicate, args...);
        return result == -1 ? 0 : result;
    }
    template<class F, class... Args>
    requires std::is_invocable_v<F, S, Args...>
    long long min_left(long long right, F predicate, Args&&... args) const{
        return min_left(latest_version(), right, predicate, args...);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED
