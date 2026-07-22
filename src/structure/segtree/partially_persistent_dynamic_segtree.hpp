#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_SEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_SEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include "../types/monoid.hpp"

template<auto Monoid, long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentDynamicSegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFFLL);
    static_assert(MAX_NODES >= 0 && MAX_VERSIONS >= 0 && MAX_CHANGES >= 0);
    static_assert(MAX_VERSIONS < std::numeric_limits<int>::max());
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    static constexpr int height = []{
        long long span = MAX_SIZE;
        int result = 0;
        while(span > 1){ span = (span + 1) / 2; ++result; }
        return result;
    }();
    static constexpr int history_log = []{
        int value = MAX_CHANGES;
        int result = 1;
        while(value > 1){ value >>= 1; ++result; }
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
    struct Node{ int left = -1; int right = -1; };
    std::array<std::optional<Node>, MAX_NODES> nodes;
    std::array<int, MAX_NODES> head{};
    std::array<std::optional<S>, MAX_CHANGES> history_value;
    std::array<int, MAX_CHANGES> history_node{};
    std::array<int, MAX_CHANGES> history_version{};
    std::array<int, MAX_CHANGES> history_previous{};
    std::array<std::array<int, MAX_CHANGES>, history_log> jump{};
    int root = -1;
    int node_count = 0;
    int history_count = 0;
    int version_count = 1;

    int new_node(){
        const int result = node_count++;
        nodes[static_cast<std::size_t>(result)].emplace();
        head[static_cast<std::size_t>(result)] = -1;
        return result;
    }
    int left_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->left; }
    int right_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->right; }

    S value_at(int node, int version) const{
        if(node == -1) return Monoid.e();
        int change = head[static_cast<std::size_t>(node)];
        if(change == -1) return Monoid.e();
        if(history_version[static_cast<std::size_t>(change)] <= version) return *history_value[static_cast<std::size_t>(change)];
        for(int level = history_log - 1; level >= 0; --level){
            const int ancestor = jump[static_cast<std::size_t>(level)][static_cast<std::size_t>(change)];
            if(ancestor != -1 && history_version[static_cast<std::size_t>(ancestor)] > version) change = ancestor;
        }
        change = history_previous[static_cast<std::size_t>(change)];
        return change == -1 ? Monoid.e() : *history_value[static_cast<std::size_t>(change)];
    }
    S current_value(int node) const{
        if(node == -1) return Monoid.e();
        const int change = head[static_cast<std::size_t>(node)];
        return change == -1 ? Monoid.e() : *history_value[static_cast<std::size_t>(change)];
    }

    void write(int node, int version, const S& value){
        const int change = history_count;
        history_value[static_cast<std::size_t>(change)].emplace(value);
        history_node[static_cast<std::size_t>(change)] = node;
        history_version[static_cast<std::size_t>(change)] = version;
        history_previous[static_cast<std::size_t>(change)] = head[static_cast<std::size_t>(node)];
        jump[0][static_cast<std::size_t>(change)] = history_previous[static_cast<std::size_t>(change)];
        for(int level = 1; level < history_log; ++level){
            const int ancestor = jump[static_cast<std::size_t>(level - 1)][static_cast<std::size_t>(change)];
            jump[static_cast<std::size_t>(level)][static_cast<std::size_t>(change)] =
                ancestor == -1 ? -1 : jump[static_cast<std::size_t>(level - 1)][static_cast<std::size_t>(ancestor)];
        }
        head[static_cast<std::size_t>(node)] = change;
        ++history_count;
    }

    void rollback_history(int snapshot){
        while(snapshot < history_count){
            const int change = --history_count;
            head[static_cast<std::size_t>(history_node[static_cast<std::size_t>(change)])] = history_previous[static_cast<std::size_t>(change)];
            history_value[static_cast<std::size_t>(change)].reset();
        }
    }
    void reset_nodes(int snapshot){
        while(snapshot < node_count) nodes[static_cast<std::size_t>(--node_count)].reset();
    }

    int required_nodes(long long index) const{
        int required = 0;
        int node = root;
        long long left = 0, right = MAX_SIZE;
        while(true){
            if(node == -1) ++required;
            if(right - left == 1) break;
            const long long middle = left + (right - left) / 2;
            if(index < middle){ node = left_of(node); right = middle; }
            else{ node = right_of(node); left = middle; }
        }
        return required;
    }

    S prod_impl(int node, int version, long long left, long long right, long long query_left, long long query_right) const{
        if(node == -1 || right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right) return value_at(node, version);
        const long long middle = left + (right - left) / 2;
        return Monoid.op(
            prod_impl(left_of(node), version, left, middle, query_left, query_right),
            prod_impl(right_of(node), version, middle, right, query_left, query_right)
        );
    }

    template<class F, class... Args>
    long long max_right_impl(int node, int version, long long left, long long right, long long query_left, S& sum, F& predicate, Args&... args) const{
        if(right <= query_left) return -1;
        if(query_left <= left){
            S next = Monoid.op(sum, value_at(node, version));
            if(std::invoke(predicate, next, args...)){ sum = next; return -1; }
            if(right - left == 1) return left;
        }
        const long long middle = left + (right - left) / 2;
        long long result = max_right_impl(left_of(node), version, left, middle, query_left, sum, predicate, args...);
        if(result != -1) return result;
        return max_right_impl(right_of(node), version, middle, right, query_left, sum, predicate, args...);
    }

    template<class F, class... Args>
    long long min_left_impl(int node, int version, long long left, long long right, long long query_right, S& sum, F& predicate, Args&... args) const{
        if(query_right <= left) return -1;
        if(right <= query_right){
            S next = Monoid.op(value_at(node, version), sum);
            if(std::invoke(predicate, next, args...)){ sum = next; return -1; }
            if(right - left == 1) return right;
        }
        const long long middle = left + (right - left) / 2;
        long long result = min_left_impl(right_of(node), version, middle, right, query_right, sum, predicate, args...);
        if(result != -1) return result;
        return min_left_impl(left_of(node), version, left, middle, query_right, sum, predicate, args...);
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]] throw std::runtime_error(message);
    }

public:
    PartiallyPersistentDynamicSegtree(){
        head.fill(-1);
        for(auto& level: jump) level.fill(-1);
    }
    PartiallyPersistentDynamicSegtree(const PartiallyPersistentDynamicSegtree&) = delete;
    PartiallyPersistentDynamicSegtree& operator=(const PartiallyPersistentDynamicSegtree&) = delete;

    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }
    int changes_used() const{ return history_count; }

    int set(long long index, const S& value){
        if(index < 0 || MAX_SIZE <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        if(version_count > MAX_VERSIONS)[[unlikely]] throw std::runtime_error("library assertion fault: version capacity exceeded (set).");
        const int missing = required_nodes(index);
        if(static_cast<long long>(node_count) + missing > MAX_NODES)[[unlikely]] throw std::runtime_error("library assertion fault: node capacity exceeded (set).");
        if(static_cast<long long>(history_count) + path_length(index) > MAX_CHANGES)[[unlikely]] throw std::runtime_error("library assertion fault: history capacity exceeded (set).");

        const int node_snapshot = node_count;
        const int history_snapshot = history_count;
        const int old_root = root;
        int linked_parent = -1;
        bool linked_right = false;
        try{
            if(root == -1) root = new_node();
            std::array<int, height + 1> path{};
            int path_size = 0;
            int node = root;
            long long left = 0, right = MAX_SIZE;
            while(true){
                path[static_cast<std::size_t>(path_size++)] = node;
                if(right - left == 1) break;
                const long long middle = left + (right - left) / 2;
                const bool go_right = middle <= index;
                int child = go_right ? right_of(node) : left_of(node);
                if(child == -1){
                    child = new_node();
                    if(node < node_snapshot && linked_parent == -1){ linked_parent = node; linked_right = go_right; }
                    if(go_right) nodes[static_cast<std::size_t>(node)]->right = child;
                    else nodes[static_cast<std::size_t>(node)]->left = child;
                }
                node = child;
                if(go_right) left = middle;
                else right = middle;
            }
            write(path[static_cast<std::size_t>(path_size - 1)], version_count, value);
            for(int position = path_size - 2; position >= 0; --position){
                node = path[static_cast<std::size_t>(position)];
                write(node, version_count, Monoid.op(current_value(left_of(node)), current_value(right_of(node))));
            }
        }catch(...){
            rollback_history(history_snapshot);
            if(linked_parent != -1){
                if(linked_right) nodes[static_cast<std::size_t>(linked_parent)]->right = -1;
                else nodes[static_cast<std::size_t>(linked_parent)]->left = -1;
            }
            root = old_root;
            reset_nodes(node_snapshot);
            throw;
        }
        return version_count++;
    }

    S get(int version, long long index) const{
        check_version(version, "library assertion fault: range violation (get).");
        if(index < 0 || MAX_SIZE <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        int node = root;
        long long left = 0, right = MAX_SIZE;
        while(node != -1 && right - left > 1){
            const long long middle = left + (right - left) / 2;
            if(index < middle){ node = left_of(node); right = middle; }
            else{ node = right_of(node); left = middle; }
        }
        return value_at(node, version);
    }
    S get(long long index) const{ return get(latest_version(), index); }

    S prod(int version, long long left, long long right) const{
        check_version(version, "library assertion fault: range violation (prod).");
        if(left < 0 || right < left || MAX_SIZE < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (prod).");
        return prod_impl(root, version, 0, MAX_SIZE, left, right);
    }
    S prod(long long left, long long right) const{ return prod(latest_version(), left, right); }

    S all_prod(int version) const{
        check_version(version, "library assertion fault: range violation (all_prod).");
        return value_at(root, version);
    }
    S all_prod() const{ return all_prod(latest_version()); }

    long long max_right(int version, long long left, auto predicate, auto&&... args) const{
        check_version(version, "library assertion fault: range violation (max_right).");
        if(left < 0 || MAX_SIZE < left)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (max_right).");
        S sum = Monoid.e();
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        if(left == MAX_SIZE) return MAX_SIZE;
        long long result = max_right_impl(root, version, 0, MAX_SIZE, left, sum, predicate, args...);
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
        long long result = min_left_impl(root, version, 0, MAX_SIZE, right, sum, predicate, args...);
        return result == -1 ? 0 : result;
    }
    template<class F, class... Args>
    requires std::is_invocable_v<F, S, Args...>
    long long min_left(long long right, F predicate, Args&&... args) const{ return min_left(latest_version(), right, predicate, args...); }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_DYNAMIC_SEGTREE_HPP_INCLUDED
