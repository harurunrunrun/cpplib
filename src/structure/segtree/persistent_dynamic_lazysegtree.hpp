#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include "../types/monoid_act_len.hpp"

template<auto Monoid, long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentDynamicLazySegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFFLL);
    static_assert(MAX_NODES >= 0 && MAX_VERSIONS >= 0);
    static_assert(MAX_VERSIONS < std::numeric_limits<int>::max());
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid)>::T;

private:
    static constexpr int height = []{
        long long span = MAX_SIZE;
        int result = 0;
        while(span > 1){ span = (span + 1) / 2; ++result; }
        return result;
    }();
    static constexpr int nodes_per_update = 4 * height + 1;

    struct Node{
        int left = -1;
        int right = -1;
        S value;
        T lazy;
        Node(const S& value_, const T& lazy_): value(value_), lazy(lazy_){}
    };
    std::array<std::optional<Node>, MAX_NODES> nodes;
    std::array<int, MAX_VERSIONS + 1> roots{};
    long long _n;
    int node_count = 0;
    int version_count = 1;

    int left_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->left; }
    int right_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->right; }
    S value_of(int node) const{ return node == -1 ? Monoid.e() : nodes[static_cast<std::size_t>(node)]->value; }
    T lazy_of(int node) const{ return node == -1 ? Monoid.id() : nodes[static_cast<std::size_t>(node)]->lazy; }

    int clone_node(int source){
        const int result = node_count++;
        nodes[static_cast<std::size_t>(result)].emplace(value_of(source), lazy_of(source));
        if(source != -1){
            nodes[static_cast<std::size_t>(result)]->left = left_of(source);
            nodes[static_cast<std::size_t>(result)]->right = right_of(source);
        }
        return result;
    }
    void all_apply(int node, const T& action, long long length){
        auto& current = *nodes[static_cast<std::size_t>(node)];
        current.value = Monoid.mapping(action, current.value, length);
        current.lazy = Monoid.composition(action, current.lazy);
    }
    void push(int node, long long left, long long right){
        const long long middle = left + (right - left) / 2;
        auto& current = *nodes[static_cast<std::size_t>(node)];
        const int new_left = clone_node(current.left);
        const int new_right = clone_node(current.right);
        all_apply(new_left, current.lazy, middle - left);
        all_apply(new_right, current.lazy, right - middle);
        current.left = new_left;
        current.right = new_right;
        current.lazy = Monoid.id();
    }
    void update(int node, long long left, long long right){
        const long long middle = left + (right - left) / 2;
        auto& current = *nodes[static_cast<std::size_t>(node)];
        current.value = Monoid.op(value_of(current.left), middle - left, value_of(current.right), right - middle);
    }
    void set_impl(int node, long long left, long long right, long long index, const S& value){
        if(right - left == 1){
            nodes[static_cast<std::size_t>(node)]->value = value;
            nodes[static_cast<std::size_t>(node)]->lazy = Monoid.id();
            return;
        }
        push(node, left, right);
        const long long middle = left + (right - left) / 2;
        if(index < middle) set_impl(nodes[static_cast<std::size_t>(node)]->left, left, middle, index, value);
        else set_impl(nodes[static_cast<std::size_t>(node)]->right, middle, right, index, value);
        update(node, left, right);
    }
    void apply_impl(int node, long long left, long long right, long long query_left, long long query_right, const T& action){
        if(query_left <= left && right <= query_right){ all_apply(node, action, right - left); return; }
        push(node, left, right);
        const long long middle = left + (right - left) / 2;
        if(query_left < middle) apply_impl(nodes[static_cast<std::size_t>(node)]->left, left, middle, query_left, query_right, action);
        if(middle < query_right) apply_impl(nodes[static_cast<std::size_t>(node)]->right, middle, right, query_left, query_right, action);
        update(node, left, right);
    }

    S eval(int node, const T& carry, long long length) const{ return Monoid.mapping(carry, value_of(node), length); }
    T next_carry(int node, const T& carry) const{ return Monoid.composition(carry, lazy_of(node)); }
    std::pair<S, long long> prod_impl(int node, long long left, long long right, long long query_left, long long query_right, const T& carry) const{
        if(right <= query_left || query_right <= left) return {Monoid.e(), 0};
        if(query_left <= left && right <= query_right) return {eval(node, carry, right - left), right - left};
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, carry);
        auto left_result = prod_impl(left_of(node), left, middle, query_left, query_right, next);
        auto right_result = prod_impl(right_of(node), middle, right, query_left, query_right, next);
        return {
            Monoid.op(left_result.first, left_result.second, right_result.first, right_result.second),
            left_result.second + right_result.second
        };
    }
    template<class F, class... Args>
    long long max_right_impl(int node, long long left, long long right, long long query_left, const T& carry, S& sum, long long& sum_length, F& predicate, Args&... args) const{
        if(right <= query_left) return -1;
        if(query_left <= left){
            S current = eval(node, carry, right - left);
            S next_sum = Monoid.op(sum, sum_length, current, right - left);
            if(std::invoke(predicate, next_sum, args...)){ sum = next_sum; sum_length += right - left; return -1; }
            if(right - left == 1) return left;
        }
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, carry);
        long long result = max_right_impl(left_of(node), left, middle, query_left, next, sum, sum_length, predicate, args...);
        if(result != -1) return result;
        return max_right_impl(right_of(node), middle, right, query_left, next, sum, sum_length, predicate, args...);
    }
    template<class F, class... Args>
    long long min_left_impl(int node, long long left, long long right, long long query_right, const T& carry, S& sum, long long& sum_length, F& predicate, Args&... args) const{
        if(query_right <= left) return -1;
        if(right <= query_right){
            S current = eval(node, carry, right - left);
            S next_sum = Monoid.op(current, right - left, sum, sum_length);
            if(std::invoke(predicate, next_sum, args...)){ sum = next_sum; sum_length += right - left; return -1; }
            if(right - left == 1) return right;
        }
        const long long middle = left + (right - left) / 2;
        const T next = next_carry(node, carry);
        long long result = min_left_impl(right_of(node), middle, right, query_right, next, sum, sum_length, predicate, args...);
        if(result != -1) return result;
        return min_left_impl(left_of(node), left, middle, query_right, next, sum, sum_length, predicate, args...);
    }
    void reset_nodes(int snapshot){ while(snapshot < node_count) nodes[static_cast<std::size_t>(--node_count)].reset(); }
    void check_version(int version, const char* message) const{ if(version < 0 || version_count <= version)[[unlikely]] throw std::runtime_error(message); }

    template<class F>
    int update_version(int source_version, F&& function){
        check_version(source_version, "library assertion fault: range violation (update).");
        if(version_count > MAX_VERSIONS)[[unlikely]] throw std::runtime_error("library assertion fault: version capacity exceeded (update).");
        if(static_cast<long long>(node_count) + nodes_per_update > MAX_NODES)[[unlikely]] throw std::runtime_error("library assertion fault: node capacity exceeded (update).");
        const int snapshot = node_count;
        try{
            const int new_root = clone_node(roots[static_cast<std::size_t>(source_version)]);
            function(new_root);
            roots[static_cast<std::size_t>(version_count)] = new_root;
        }catch(...){ reset_nodes(snapshot); throw; }
        return version_count++;
    }

public:
    explicit PersistentDynamicLazySegtree(long long n = MAX_SIZE): _n(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
        roots.fill(-1);
    }
    PersistentDynamicLazySegtree(const PersistentDynamicLazySegtree&) = delete;
    PersistentDynamicLazySegtree& operator=(const PersistentDynamicLazySegtree&) = delete;

    long long size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }
    int fork(int version){
        check_version(version, "library assertion fault: range violation (fork).");
        if(version_count > MAX_VERSIONS)[[unlikely]] throw std::runtime_error("library assertion fault: version capacity exceeded (fork).");
        roots[static_cast<std::size_t>(version_count)] = roots[static_cast<std::size_t>(version)];
        return version_count++;
    }
    int set(int version, long long index, const S& value){
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        return update_version(version, [&](int root){ set_impl(root, 0, _n, index, value); });
    }
    int apply(int version, long long index, const T& action){
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (apply).");
        return update_version(version, [&](int root){ apply_impl(root, 0, _n, index, index + 1, action); });
    }
    int apply(int version, long long left, long long right, const T& action){
        check_version(version, "library assertion fault: range violation (apply).");
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (apply).");
        if(left == right) return fork(version);
        return update_version(version, [&](int root){ apply_impl(root, 0, _n, left, right, action); });
    }
    S get(int version, long long index) const{
        check_version(version, "library assertion fault: range violation (get).");
        if(index < 0 || _n <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        return prod(version, index, index + 1);
    }
    S prod(int version, long long left, long long right) const{
        check_version(version, "library assertion fault: range violation (prod).");
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (prod).");
        if(left == right) return Monoid.e();
        return prod_impl(roots[static_cast<std::size_t>(version)], 0, _n, left, right, Monoid.id()).first;
    }
    S all_prod(int version) const{
        check_version(version, "library assertion fault: range violation (all_prod).");
        return value_of(roots[static_cast<std::size_t>(version)]);
    }
    long long max_right(int version, long long left, auto predicate, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (max_right).");
        if(left < 0 || _n < left)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (max_right).");
        S sum = Monoid.e(); long long sum_length = 0;
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        if(left == _n) return _n;
        long long result = max_right_impl(roots[static_cast<std::size_t>(version)], 0, _n, left, Monoid.id(), sum, sum_length, predicate, args...);
        return result == -1 ? _n : result;
    }
    long long min_left(int version, long long right, auto predicate, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (min_left).");
        if(right < 0 || _n < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (min_left).");
        S sum = Monoid.e(); long long sum_length = 0;
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        if(right == 0) return 0;
        long long result = min_left_impl(roots[static_cast<std::size_t>(version)], 0, _n, right, Monoid.id(), sum, sum_length, predicate, args...);
        return result == -1 ? 0 : result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_DYNAMIC_LAZYSEGTREE_HPP_INCLUDED
