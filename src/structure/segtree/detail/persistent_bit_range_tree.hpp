#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_DETAIL_PERSISTENT_BIT_RANGE_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_DETAIL_PERSISTENT_BIT_RANGE_TREE_HPP_INCLUDED

#include <array>
#include <cstdint>
#include <stdexcept>

namespace bit_range_tree_detail{

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentBitRangeTree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_NODES > 0);
    static_assert(MAX_VERSIONS > 0);

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
    std::array<Node, MAX_NODES> nodes{};
    std::array<int, MAX_VERSIONS> roots{};

    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (version).");
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

    int clone_node(int source){
        if(node_count == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: node capacity exceeded (bit range tree).");
        }
        const int result = node_count++;
        nodes[static_cast<std::size_t>(result)] = source == -1 ? Node{} : nodes[static_cast<std::size_t>(source)];
        return result;
    }

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

    void apply_node(int node, long long length, std::int8_t operation){
        Node& current = nodes[static_cast<std::size_t>(node)];
        current.sum = mapped_sum(current.sum, length, operation);
        current.lazy = compose(operation, current.lazy);
    }

    long long node_sum(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].sum;
    }

    void push(int node, long long left, long long right){
        Node& current = nodes[static_cast<std::size_t>(node)];
        if(current.lazy == NONE || right - left == 1) return;
        const long long middle = left + (right - left) / 2;
        if(current.lazy == SET_ZERO){
            current.left = -1;
            current.right = -1;
        }else{
            current.left = clone_node(current.left);
            current.right = clone_node(current.right);
            apply_node(current.left, middle - left, current.lazy);
            apply_node(current.right, right - middle, current.lazy);
        }
        current.lazy = NONE;
    }

    int update(
        int source,
        long long left,
        long long right,
        long long query_left,
        long long query_right,
        std::int8_t operation
    ){
        if(right <= query_left || query_right <= left) return source;
        if(query_left <= left && right <= query_right && operation == SET_ZERO) return -1;
        const int result = clone_node(source);
        if(query_left <= left && right <= query_right){
            apply_node(result, right - left, operation);
            return result;
        }
        push(result, left, right);
        const long long middle = left + (right - left) / 2;
        Node& current = nodes[static_cast<std::size_t>(result)];
        current.left = update(current.left, left, middle, query_left, query_right, operation);
        current.right = update(current.right, middle, right, query_left, query_right, operation);
        current.sum = node_sum(current.left) + node_sum(current.right);
        return result;
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
        const long long sum = node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].sum;
        if(query_left <= left && right <= query_right){
            return mapped_sum(sum, right - left, accumulated);
        }
        const std::int8_t local = node == -1 ? NONE : nodes[static_cast<std::size_t>(node)].lazy;
        const std::int8_t next = compose(accumulated, local);
        const long long middle = left + (right - left) / 2;
        const int left_node = node == -1 ? -1 : nodes[static_cast<std::size_t>(node)].left;
        const int right_node = node == -1 ? -1 : nodes[static_cast<std::size_t>(node)].right;
        return query(left_node, left, middle, query_left, query_right, next) +
               query(right_node, middle, right, query_left, query_right, next);
    }

    int append_update(int version, long long left, long long right, std::int8_t operation){
        check_version(version);
        check_range(left, right);
        check_version_capacity();
        if(left == right){
            roots[static_cast<std::size_t>(version_count)] = roots[static_cast<std::size_t>(version)];
            return version_count++;
        }
        const int snapshot = node_count;
        try{
            const int root = update(
                roots[static_cast<std::size_t>(version)],
                0,
                _n,
                left,
                right,
                operation
            );
            roots[static_cast<std::size_t>(version_count)] = root;
            return version_count++;
        }catch(...){
            node_count = snapshot;
            throw;
        }
    }

public:
    explicit PersistentBitRangeTree(long long n = MAX_SIZE): _n(n){
        if(n <= 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        roots.fill(-1);
    }

    long long size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }

    int fork(int version){
        check_version(version);
        check_version_capacity();
        roots[static_cast<std::size_t>(version_count)] = roots[static_cast<std::size_t>(version)];
        return version_count++;
    }
    int assign(int version, long long left, long long right, bool value){
        return append_update(version, left, right, value ? SET_ONE : SET_ZERO);
    }
    int flip(int version, long long left, long long right){
        return append_update(version, left, right, FLIP);
    }
    int set_one(int version, long long left, long long right){
        return append_update(version, left, right, SET_ONE);
    }
    long long sum(int version, long long left, long long right) const{
        check_version(version);
        check_range(left, right);
        return query(roots[static_cast<std::size_t>(version)], 0, _n, left, right, NONE);
    }
    bool get(int version, long long position) const{
        check_position(position);
        return sum(version, position, position + 1) != 0;
    }
};

} // namespace bit_range_tree_detail

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_DETAIL_PERSISTENT_BIT_RANGE_TREE_HPP_INCLUDED
