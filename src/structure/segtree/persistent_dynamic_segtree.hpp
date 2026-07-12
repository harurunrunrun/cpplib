#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include "../types/monoid.hpp"

template<auto Monoid, long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentDynamicSegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFFLL);
    static_assert(MAX_NODES >= 0);
    static_assert(MAX_VERSIONS >= 0);
    static_assert(MAX_VERSIONS < std::numeric_limits<int>::max());
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    static constexpr int height = []{
        long long span = MAX_SIZE;
        int result = 0;
        while(span > 1){ span = (span + 1) / 2; ++result; }
        return result;
    }();

    static int path_length(long long index){
        int result = 1;
        long long left = 0, right = MAX_SIZE;
        while(right - left > 1){
            const long long middle = left + (right - left) / 2;
            if(index < middle) right = middle;
            else left = middle;
            ++result;
        }
        return result;
    }
    struct Node{
        int left = -1;
        int right = -1;
        S value;
        explicit Node(const S& value_): value(value_){}
    };

    std::array<std::optional<Node>, MAX_NODES> nodes;
    std::array<int, MAX_VERSIONS + 1> roots{};
    int node_count = 0;
    int version_count = 1;

    const S& value_ref(int node) const{
        return nodes[static_cast<std::size_t>(node)]->value;
    }
    S value_of(int node) const{
        return node == -1 ? Monoid.e() : value_ref(node);
    }
    int left_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->left; }
    int right_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->right; }

    int clone_node(int source){
        const int result = node_count++;
        nodes[static_cast<std::size_t>(result)].emplace(source == -1 ? Monoid.e() : value_ref(source));
        if(source != -1){
            nodes[static_cast<std::size_t>(result)]->left = left_of(source);
            nodes[static_cast<std::size_t>(result)]->right = right_of(source);
        }
        return result;
    }

    int set_impl(int source, long long left, long long right, long long index, const S& value){
        const int current = clone_node(source);
        if(right - left == 1){
            nodes[static_cast<std::size_t>(current)]->value = value;
            return current;
        }
        const long long middle = left + (right - left) / 2;
        if(index < middle){
            nodes[static_cast<std::size_t>(current)]->left = set_impl(left_of(source), left, middle, index, value);
        }else{
            nodes[static_cast<std::size_t>(current)]->right = set_impl(right_of(source), middle, right, index, value);
        }
        nodes[static_cast<std::size_t>(current)]->value = Monoid.op(
            value_of(nodes[static_cast<std::size_t>(current)]->left),
            value_of(nodes[static_cast<std::size_t>(current)]->right)
        );
        return current;
    }

    S prod_impl(int node, long long left, long long right, long long query_left, long long query_right) const{
        if(node == -1 || right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right) return value_ref(node);
        const long long middle = left + (right - left) / 2;
        return Monoid.op(
            prod_impl(left_of(node), left, middle, query_left, query_right),
            prod_impl(right_of(node), middle, right, query_left, query_right)
        );
    }

    template<class F, class... Args>
    long long max_right_impl(int node, long long left, long long right, long long query_left, S& sum, F& predicate, Args&... args) const{
        if(right <= query_left) return -1;
        if(query_left <= left){
            S next = Monoid.op(sum, value_of(node));
            if(std::invoke(predicate, next, args...)){ sum = next; return -1; }
            if(right - left == 1) return left;
        }
        const long long middle = left + (right - left) / 2;
        long long result = max_right_impl(left_of(node), left, middle, query_left, sum, predicate, args...);
        if(result != -1) return result;
        return max_right_impl(right_of(node), middle, right, query_left, sum, predicate, args...);
    }

    template<class F, class... Args>
    long long min_left_impl(int node, long long left, long long right, long long query_right, S& sum, F& predicate, Args&... args) const{
        if(query_right <= left) return -1;
        if(right <= query_right){
            S next = Monoid.op(value_of(node), sum);
            if(std::invoke(predicate, next, args...)){ sum = next; return -1; }
            if(right - left == 1) return right;
        }
        const long long middle = left + (right - left) / 2;
        long long result = min_left_impl(right_of(node), middle, right, query_right, sum, predicate, args...);
        if(result != -1) return result;
        return min_left_impl(left_of(node), left, middle, query_right, sum, predicate, args...);
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]] throw std::runtime_error(message);
    }
    void reset_nodes(int snapshot){
        while(snapshot < node_count) nodes[static_cast<std::size_t>(--node_count)].reset();
    }

public:
    PersistentDynamicSegtree(){ roots.fill(-1); }
    PersistentDynamicSegtree(const PersistentDynamicSegtree&) = delete;
    PersistentDynamicSegtree& operator=(const PersistentDynamicSegtree&) = delete;

    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }

    int set(int version, long long index, const S& value){
        check_version(version, "library assertion fault: range violation (set).");
        if(index < 0 || MAX_SIZE <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        if(version_count > MAX_VERSIONS)[[unlikely]] throw std::runtime_error("library assertion fault: version capacity exceeded (set).");
        if(static_cast<long long>(node_count) + path_length(index) > MAX_NODES)[[unlikely]] throw std::runtime_error("library assertion fault: node capacity exceeded (set).");
        const int snapshot = node_count;
        try{
            const int root = set_impl(roots[static_cast<std::size_t>(version)], 0, MAX_SIZE, index, value);
            roots[static_cast<std::size_t>(version_count)] = root;
        }catch(...){ reset_nodes(snapshot); throw; }
        return version_count++;
    }

    int set(long long index, const S& value){ return set(latest_version(), index, value); }

    int fork(int version){
        check_version(version, "library assertion fault: range violation (fork).");
        if(version_count > MAX_VERSIONS)[[unlikely]] throw std::runtime_error("library assertion fault: version capacity exceeded (fork).");
        roots[static_cast<std::size_t>(version_count)] = roots[static_cast<std::size_t>(version)];
        return version_count++;
    }

    S get(int version, long long index) const{
        check_version(version, "library assertion fault: range violation (get).");
        if(index < 0 || MAX_SIZE <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        int node = roots[static_cast<std::size_t>(version)];
        long long left = 0, right = MAX_SIZE;
        while(node != -1 && right - left > 1){
            const long long middle = left + (right - left) / 2;
            if(index < middle){ node = left_of(node); right = middle; }
            else{ node = right_of(node); left = middle; }
        }
        return value_of(node);
    }
    S get(long long index) const{ return get(latest_version(), index); }

    S prod(int version, long long left, long long right) const{
        check_version(version, "library assertion fault: range violation (prod).");
        if(left < 0 || right < left || MAX_SIZE < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (prod).");
        return prod_impl(roots[static_cast<std::size_t>(version)], 0, MAX_SIZE, left, right);
    }
    S prod(long long left, long long right) const{ return prod(latest_version(), left, right); }

    S all_prod(int version) const{
        check_version(version, "library assertion fault: range violation (all_prod).");
        return value_of(roots[static_cast<std::size_t>(version)]);
    }
    S all_prod() const{ return all_prod(latest_version()); }

    long long max_right(int version, long long left, auto predicate, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (max_right).");
        if(left < 0 || MAX_SIZE < left)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (max_right).");
        S sum = Monoid.e();
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        if(left == MAX_SIZE) return MAX_SIZE;
        long long result = max_right_impl(roots[static_cast<std::size_t>(version)], 0, MAX_SIZE, left, sum, predicate, args...);
        return result == -1 ? MAX_SIZE : result;
    }
    template<class F, class... Args>
    requires std::is_invocable_v<F, S, Args...>
    long long max_right(long long left, F predicate, Args&&... args) const{ return max_right(latest_version(), left, predicate, args...); }

    long long min_left(int version, long long right, auto predicate, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (min_left).");
        if(right < 0 || MAX_SIZE < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (min_left).");
        S sum = Monoid.e();
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        if(right == 0) return 0;
        long long result = min_left_impl(roots[static_cast<std::size_t>(version)], 0, MAX_SIZE, right, sum, predicate, args...);
        return result == -1 ? 0 : result;
    }
    template<class F, class... Args>
    requires std::is_invocable_v<F, S, Args...>
    long long min_left(long long right, F predicate, Args&&... args) const{ return min_left(latest_version(), right, predicate, args...); }
};
