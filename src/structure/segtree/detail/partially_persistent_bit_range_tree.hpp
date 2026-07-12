#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>

#include "../../other/partially_persistent_storage.hpp"

namespace bit_range_tree_detail{

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentBitRangeTree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_NODES > 0);
    static_assert(MAX_VERSIONS > 0);
    static_assert(MAX_CHANGES >= 0);

private:
    static constexpr std::int8_t NONE = -1;
    static constexpr std::int8_t SET_ZERO = 0;
    static constexpr std::int8_t SET_ONE = 1;
    static constexpr std::int8_t FLIP = 2;

    struct Node{
        int left = -1;
        int right = -1;
        long long sum = 0;
        std::int8_t lazy = NONE;
    };

    long long _n = MAX_SIZE;
    int node_count = 0;
    int version_count = 1;
    PartiallyPersistentStorage<Node, MAX_NODES, MAX_CHANGES> history;
    std::array<int, MAX_NODES> created_version{};
    std::array<int, MAX_VERSIONS> roots{};

    static std::int8_t compose(std::int8_t newer, std::int8_t older){
        if(newer == NONE) return older;
        if(newer == SET_ZERO || newer == SET_ONE) return newer;
        if(older == NONE) return FLIP;
        if(older == SET_ZERO) return SET_ONE;
        if(older == SET_ONE) return SET_ZERO;
        return NONE;
    }
    static long long mapped_sum(long long sum, long long length, std::int8_t operation){
        if(operation == SET_ZERO) return 0;
        if(operation == SET_ONE) return length;
        if(operation == FLIP) return length - sum;
        return sum;
    }
    static void apply(Node& node, long long length, std::int8_t operation){
        node.sum = mapped_sum(node.sum, length, operation);
        node.lazy = compose(operation, node.lazy);
    }

    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (version).");
        }
    }
    void check_latest(int version) const{
        check_version(version);
        if(version != version_count - 1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: update requires latest version.");
        }
    }
    void check_range(long long left, long long right) const{
        if(left < 0 || left > right || _n < right)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (bit range tree).");
        }
    }
    void check_position(long long position) const{
        if(position < 0 || _n <= position)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (bit range tree).");
        }
    }
    void check_version_capacity() const{
        if(version_count == MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (bit range tree).");
        }
    }

    int make_node(const Node& initial, int version){
        if(node_count == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: node capacity exceeded (bit range tree).");
        }
        const int result = node_count++;
        created_version[static_cast<std::size_t>(result)] = version;
        history.initialize(result, initial);
        return result;
    }
    Node current_node(int node) const{
        return node == -1 ? Node{} : history.current(node);
    }
    Node node_at(int node, int version) const{
        if(node == -1 || version < created_version[static_cast<std::size_t>(node)]) return Node{};
        return history.get(node, version);
    }
    long long current_sum(int node) const{
        return node == -1 ? 0 : history.current(node).sum;
    }

    int update(
        int node,
        long long left,
        long long right,
        long long query_left,
        long long query_right,
        std::int8_t operation,
        int version
    ){
        if(right <= query_left || query_right <= left) return node;
        if(query_left <= left && right <= query_right && operation == SET_ZERO) return -1;
        if(node == -1) node = make_node(Node{}, version);
        Node current = history.current(node);
        if(query_left <= left && right <= query_right){
            apply(current, right - left, operation);
            history.write(node, version, current);
            return node;
        }
        const long long middle = left + (right - left) / 2;
        if(current.lazy != NONE){
            if(current.lazy == SET_ZERO){
                current.left = -1;
                current.right = -1;
            }else{
                Node left_state = current_node(current.left);
                Node right_state = current_node(current.right);
                apply(left_state, middle - left, current.lazy);
                apply(right_state, right - middle, current.lazy);
                if(current.left == -1) current.left = make_node(left_state, version);
                else history.write(current.left, version, left_state);
                if(current.right == -1) current.right = make_node(right_state, version);
                else history.write(current.right, version, right_state);
            }
            current.lazy = NONE;
        }
        current.left = update(current.left, left, middle, query_left, query_right, operation, version);
        current.right = update(current.right, middle, right, query_left, query_right, operation, version);
        current.sum = current_sum(current.left) + current_sum(current.right);
        history.write(node, version, current);
        return node;
    }

    long long query(
        int node,
        long long left,
        long long right,
        long long query_left,
        long long query_right,
        std::int8_t accumulated,
        int version
    ) const{
        if(right <= query_left || query_right <= left) return 0;
        const Node current = node_at(node, version);
        if(query_left <= left && right <= query_right){
            return mapped_sum(current.sum, right - left, accumulated);
        }
        const std::int8_t next = compose(accumulated, current.lazy);
        const long long middle = left + (right - left) / 2;
        return query(current.left, left, middle, query_left, query_right, next, version) +
               query(current.right, middle, right, query_left, query_right, next, version);
    }

    int append_update(int base, long long left, long long right, std::int8_t operation){
        check_latest(base);
        check_range(left, right);
        check_version_capacity();
        const int new_version = version_count;
        const int node_snapshot = node_count;
        const int history_snapshot = history.changes();
        try{
            int root = roots[static_cast<std::size_t>(base)];
            if(left != right) root = update(root, 0, _n, left, right, operation, new_version);
            roots[static_cast<std::size_t>(new_version)] = root;
            ++version_count;
            return new_version;
        }catch(...){
            history.rollback(history_snapshot);
            node_count = node_snapshot;
            throw;
        }
    }

public:
    explicit PartiallyPersistentBitRangeTree(long long n = MAX_SIZE): _n(n){
        if(n <= 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        roots.fill(-1);
        created_version.fill(MAX_VERSIONS);
    }

    long long size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }
    int changes_used() const{ return history.changes(); }

    int fork(int version){
        check_latest(version);
        check_version_capacity();
        roots[static_cast<std::size_t>(version_count)] = roots[static_cast<std::size_t>(version)];
        return version_count++;
    }
    int assign(int version, long long left, long long right, bool value){
        return append_update(version, left, right, value ? SET_ONE : SET_ZERO);
    }
    int assign(long long left, long long right, bool value){
        return assign(latest_version(), left, right, value);
    }
    int flip(int version, long long left, long long right){
        return append_update(version, left, right, FLIP);
    }
    int flip(long long left, long long right){ return flip(latest_version(), left, right); }
    int set_one(int version, long long left, long long right){
        return append_update(version, left, right, SET_ONE);
    }
    int set_one(long long left, long long right){ return set_one(latest_version(), left, right); }
    int set(int version, long long left, long long right){
        return set_one(version, left, right);
    }
    int set(long long left, long long right){ return set_one(left, right); }
    long long sum(int version, long long left, long long right) const{
        check_version(version);
        check_range(left, right);
        return query(roots[static_cast<std::size_t>(version)], 0, _n, left, right, NONE, version);
    }
    bool get(int version, long long position) const{
        check_position(position);
        return sum(version, position, position + 1) != 0;
    }
};

} // namespace bit_range_tree_detail
