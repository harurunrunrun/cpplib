#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>

namespace bit_range_tree_detail{

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct RollbackBitRangeTree{
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
    struct Change{
        int node = -1;
        Node previous;
    };

    long long _n = MAX_SIZE;
    int root = -1;
    int node_count = 0;
    int change_count = 0;
    int version_count = 1;
    std::array<Node, MAX_NODES> nodes{};
    std::array<Change, MAX_CHANGES> changes{};
    std::array<int, MAX_VERSIONS> root_at{};
    std::array<int, MAX_VERSIONS> node_count_at{};
    std::array<int, MAX_VERSIONS> change_count_at{};

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

    int make_node(const Node& initial = Node{}){
        if(node_count == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: node capacity exceeded (bit range tree).");
        }
        nodes[static_cast<std::size_t>(node_count)] = initial;
        return node_count++;
    }
    void write(int node, const Node& value){
        if(change_count == MAX_CHANGES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: change capacity exceeded (bit range tree).");
        }
        changes[static_cast<std::size_t>(change_count++)] = {node, nodes[static_cast<std::size_t>(node)]};
        nodes[static_cast<std::size_t>(node)] = value;
    }
    Node node_value(int node) const{ return node == -1 ? Node{} : nodes[static_cast<std::size_t>(node)]; }
    long long node_sum(int node) const{ return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].sum; }

    int update(
        int node,
        long long left,
        long long right,
        long long query_left,
        long long query_right,
        std::int8_t operation
    ){
        if(right <= query_left || query_right <= left) return node;
        if(query_left <= left && right <= query_right && operation == SET_ZERO) return -1;
        if(node == -1) node = make_node();
        Node current = nodes[static_cast<std::size_t>(node)];
        if(query_left <= left && right <= query_right){
            apply(current, right - left, operation);
            write(node, current);
            return node;
        }
        const long long middle = left + (right - left) / 2;
        if(current.lazy != NONE){
            if(current.lazy == SET_ZERO){
                current.left = -1;
                current.right = -1;
            }else{
                Node left_state = node_value(current.left);
                Node right_state = node_value(current.right);
                apply(left_state, middle - left, current.lazy);
                apply(right_state, right - middle, current.lazy);
                if(current.left == -1) current.left = make_node(left_state);
                else write(current.left, left_state);
                if(current.right == -1) current.right = make_node(right_state);
                else write(current.right, right_state);
            }
            current.lazy = NONE;
        }
        current.left = update(current.left, left, middle, query_left, query_right, operation);
        current.right = update(current.right, middle, right, query_left, query_right, operation);
        current.sum = node_sum(current.left) + node_sum(current.right);
        write(node, current);
        return node;
    }

    long long query(
        int node,
        long long left,
        long long right,
        long long query_left,
        long long query_right,
        std::int8_t accumulated
    ) const{
        if(right <= query_left || query_right <= left) return 0;
        const Node current = node_value(node);
        if(query_left <= left && right <= query_right){
            return mapped_sum(current.sum, right - left, accumulated);
        }
        const std::int8_t next = compose(accumulated, current.lazy);
        const long long middle = left + (right - left) / 2;
        return query(current.left, left, middle, query_left, query_right, next) +
               query(current.right, middle, right, query_left, query_right, next);
    }

    int append_update(long long left, long long right, std::int8_t operation){
        check_range(left, right);
        check_version_capacity();
        const int root_snapshot = root;
        const int node_snapshot = node_count;
        const int change_snapshot = change_count;
        try{
            if(left != right) root = update(root, 0, _n, left, right, operation);
            root_at[static_cast<std::size_t>(version_count)] = root;
            node_count_at[static_cast<std::size_t>(version_count)] = node_count;
            change_count_at[static_cast<std::size_t>(version_count)] = change_count;
            return version_count++;
        }catch(...){
            while(change_count > change_snapshot){
                const Change& change = changes[static_cast<std::size_t>(--change_count)];
                nodes[static_cast<std::size_t>(change.node)] = change.previous;
            }
            root = root_snapshot;
            node_count = node_snapshot;
            throw;
        }
    }

public:
    explicit RollbackBitRangeTree(long long n = MAX_SIZE): _n(n){
        if(n <= 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        root_at.fill(-1);
        node_count_at.fill(0);
        change_count_at.fill(0);
    }

    long long size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int snapshot() const{ return latest_version(); }
    int nodes_used() const{ return node_count; }
    int changes_used() const{ return change_count; }

    int fork(){
        check_version_capacity();
        root_at[static_cast<std::size_t>(version_count)] = root;
        node_count_at[static_cast<std::size_t>(version_count)] = node_count;
        change_count_at[static_cast<std::size_t>(version_count)] = change_count;
        return version_count++;
    }
    int assign(long long left, long long right, bool value){
        return append_update(left, right, value ? SET_ONE : SET_ZERO);
    }
    int flip(long long left, long long right){ return append_update(left, right, FLIP); }
    int set_one(long long left, long long right){ return append_update(left, right, SET_ONE); }
    int set(long long left, long long right){ return set_one(left, right); }
    long long sum(long long left, long long right) const{
        check_range(left, right);
        return query(root, 0, _n, left, right, NONE);
    }
    bool get(long long position) const{
        check_position(position);
        return sum(position, position + 1) != 0;
    }
    void rollback(int version){
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (rollback).");
        }
        const int target_changes = change_count_at[static_cast<std::size_t>(version)];
        while(change_count > target_changes){
            const Change& change = changes[static_cast<std::size_t>(--change_count)];
            nodes[static_cast<std::size_t>(change.node)] = change.previous;
        }
        root = root_at[static_cast<std::size_t>(version)];
        node_count = node_count_at[static_cast<std::size_t>(version)];
        version_count = version + 1;
    }
};

} // namespace bit_range_tree_detail
