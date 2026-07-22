#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_SEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_SEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include "../types/monoid.hpp"

template<auto Monoid, long long MAX_SIZE, int MAX_NODES, int MAX_CHANGES>
struct RollbackDynamicSegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFFLL);
    static_assert(MAX_NODES >= 0 && MAX_CHANGES >= 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    using Snapshot = int;

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
    std::array<int, MAX_CHANGES> changed_node{};
    std::array<std::optional<S>, MAX_CHANGES> old_value;
    std::array<int, MAX_CHANGES> operation_change_snapshot{};
    std::array<int, MAX_CHANGES> operation_node_snapshot{};
    std::array<int, MAX_CHANGES> operation_old_root{};
    std::array<int, MAX_CHANGES> operation_link_parent{};
    std::array<char, MAX_CHANGES> operation_link_right{};
    int root = -1;
    int node_count = 0;
    int change_count = 0;
    int operation_count = 0;

    int new_node(){
        const int result = node_count++;
        nodes[static_cast<std::size_t>(result)].emplace(Monoid.e());
        return result;
    }
    int left_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->left; }
    int right_of(int node) const{ return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)]->right; }
    S value_of(int node) const{ return node == -1 ? Monoid.e() : nodes[static_cast<std::size_t>(node)]->value; }

    void write(int node, const S& value){
        changed_node[static_cast<std::size_t>(change_count)] = node;
        old_value[static_cast<std::size_t>(change_count)].emplace(nodes[static_cast<std::size_t>(node)]->value);
        ++change_count;
        nodes[static_cast<std::size_t>(node)]->value = value;
    }
    void restore_changes(int snapshot){
        while(snapshot < change_count){
            const int change = --change_count;
            nodes[static_cast<std::size_t>(changed_node[static_cast<std::size_t>(change)])]->value = *old_value[static_cast<std::size_t>(change)];
            old_value[static_cast<std::size_t>(change)].reset();
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

    S prod_impl(int node, long long left, long long right, long long query_left, long long query_right) const{
        if(node == -1 || right <= query_left || query_right <= left) return Monoid.e();
        if(query_left <= left && right <= query_right) return value_of(node);
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

    void undo_one(){
        const int operation = --operation_count;
        restore_changes(operation_change_snapshot[static_cast<std::size_t>(operation)]);
        const int parent = operation_link_parent[static_cast<std::size_t>(operation)];
        if(parent != -1){
            if(operation_link_right[static_cast<std::size_t>(operation)]) nodes[static_cast<std::size_t>(parent)]->right = -1;
            else nodes[static_cast<std::size_t>(parent)]->left = -1;
        }
        root = operation_old_root[static_cast<std::size_t>(operation)];
        reset_nodes(operation_node_snapshot[static_cast<std::size_t>(operation)]);
    }

public:
    RollbackDynamicSegtree() = default;
    RollbackDynamicSegtree(const RollbackDynamicSegtree&) = delete;
    RollbackDynamicSegtree& operator=(const RollbackDynamicSegtree&) = delete;

    int history_size() const{ return operation_count; }
    int nodes_used() const{ return node_count; }
    int changes_used() const{ return change_count; }
    bool can_undo() const{ return operation_count != 0; }

    void set(long long index, const S& value){
        if(index < 0 || MAX_SIZE <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        const int missing = required_nodes(index);
        if(static_cast<long long>(node_count) + missing > MAX_NODES)[[unlikely]] throw std::runtime_error("library assertion fault: node capacity exceeded (set).");
        if(static_cast<long long>(change_count) + path_length(index) > MAX_CHANGES || operation_count == MAX_CHANGES)[[unlikely]] throw std::runtime_error("library assertion fault: history capacity exceeded (set).");

        const int node_snapshot = node_count;
        const int change_snapshot = change_count;
        const int old_root_snapshot = root;
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
            write(path[static_cast<std::size_t>(path_size - 1)], value);
            for(int position = path_size - 2; position >= 0; --position){
                node = path[static_cast<std::size_t>(position)];
                write(node, Monoid.op(value_of(left_of(node)), value_of(right_of(node))));
            }
        }catch(...){
            restore_changes(change_snapshot);
            if(linked_parent != -1){
                if(linked_right) nodes[static_cast<std::size_t>(linked_parent)]->right = -1;
                else nodes[static_cast<std::size_t>(linked_parent)]->left = -1;
            }
            root = old_root_snapshot;
            reset_nodes(node_snapshot);
            throw;
        }
        const int operation = operation_count++;
        operation_change_snapshot[static_cast<std::size_t>(operation)] = change_snapshot;
        operation_node_snapshot[static_cast<std::size_t>(operation)] = node_snapshot;
        operation_old_root[static_cast<std::size_t>(operation)] = old_root_snapshot;
        operation_link_parent[static_cast<std::size_t>(operation)] = linked_parent;
        operation_link_right[static_cast<std::size_t>(operation)] = static_cast<char>(linked_right);
    }

    S get(long long index) const{
        if(index < 0 || MAX_SIZE <= index)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        int node = root;
        long long left = 0, right = MAX_SIZE;
        while(node != -1 && right - left > 1){
            const long long middle = left + (right - left) / 2;
            if(index < middle){ node = left_of(node); right = middle; }
            else{ node = right_of(node); left = middle; }
        }
        return value_of(node);
    }
    S prod(long long left, long long right) const{
        if(left < 0 || right < left || MAX_SIZE < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (prod).");
        return prod_impl(root, 0, MAX_SIZE, left, right);
    }
    S all_prod() const{ return value_of(root); }

    long long max_right(long long left, auto predicate, auto&&... args) const{
        if(left < 0 || MAX_SIZE < left)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (max_right).");
        S sum = Monoid.e();
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
        if(left == MAX_SIZE) return MAX_SIZE;
        long long result = max_right_impl(root, 0, MAX_SIZE, left, sum, predicate, args...);
        return result == -1 ? MAX_SIZE : result;
    }
    long long min_left(long long right, auto predicate, auto&&... args) const{
        if(right < 0 || MAX_SIZE < right)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (min_left).");
        S sum = Monoid.e();
        if(!std::invoke(predicate, sum, args...))[[unlikely]] throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
        if(right == 0) return 0;
        long long result = min_left_impl(root, 0, MAX_SIZE, right, sum, predicate, args...);
        return result == -1 ? 0 : result;
    }

    Snapshot snapshot() const{ return operation_count; }
    void undo(){
        if(!can_undo())[[unlikely]] throw std::runtime_error("library assertion fault: undo history is empty (undo).");
        undo_one();
    }
    void rollback(Snapshot target){
        if(target < 0 || operation_count < target)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (rollback).");
        while(target < operation_count) undo_one();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_DYNAMIC_SEGTREE_HPP_INCLUDED
