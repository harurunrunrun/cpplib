#ifndef CPPLIB_SRC_STRUCTURE_BBST_MAP_PERSISTENT_LAZY_RED_BLACK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_MAP_PERSISTENT_LAZY_RED_BLACK_TREE_HPP_INCLUDED

#include <array>
#include "../../array/static_or_vector.hpp"
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<class Key, auto MonoidActLen, int MAX_NODE = -1, int MAX_VERSION = -1, class Compare = std::less<Key>>
struct PersistentLazyRedBlackTree{
    static_assert(MAX_NODE == -1 || MAX_NODE > 0);
    static_assert(MAX_VERSION == -1 || MAX_VERSION > 0);

    using S = typename std::remove_cvref_t<decltype(MonoidActLen)>::S;
    using T = typename std::remove_cvref_t<decltype(MonoidActLen)>::T;

private:
    struct Node{
        int left = -1;
        int right = -1;
        int size = 0;
        bool red = false;
        bool lazy_flag = false;
        Key key{};
        Key min_key{};
        Key max_key{};
        S value{};
        S aggregate{};
        T lazy{};
    };

    cpplib_detail::StaticOrVector<Node, (MAX_NODE < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_NODE))> nodes{};
    cpplib_detail::StaticOrVector<int, (MAX_VERSION < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSION))> roots{};
    int used = 0;
    int version_count = 1;
    int mutation_begin = 0;
    Compare comp;

    bool equal_key(const Key& a, const Key& b) const{
        return !comp(a, b) && !comp(b, a);
    }

    bool is_red(int node) const{
        return node != -1 && nodes[static_cast<std::size_t>(node)].red;
    }

    int size_or_zero(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].size;
    }

    int left_of(int node) const{
        return node == -1 ? -1 : nodes[static_cast<std::size_t>(node)].left;
    }

    S aggregate_or_e(int node) const{
        return node == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(node)].aggregate;
    }

    void pull(int node){
        const auto current_node_index_1 = static_cast<std::size_t>(node);
        int left_size = size_or_zero(nodes[current_node_index_1].left);
        int right_size = size_or_zero(nodes[current_node_index_1].right);
        nodes[current_node_index_1].size = left_size + 1 + right_size;
        nodes[current_node_index_1].min_key = nodes[current_node_index_1].left == -1
            ? nodes[current_node_index_1].key
            : nodes[static_cast<std::size_t>(nodes[current_node_index_1].left)].min_key;
        nodes[current_node_index_1].max_key = nodes[current_node_index_1].right == -1
            ? nodes[current_node_index_1].key
            : nodes[static_cast<std::size_t>(nodes[current_node_index_1].right)].max_key;
        S prefix = MonoidActLen.op(aggregate_or_e(nodes[current_node_index_1].left), left_size, nodes[current_node_index_1].value, 1);
        nodes[current_node_index_1].aggregate = MonoidActLen.op(
            prefix,
            left_size + 1,
            aggregate_or_e(nodes[current_node_index_1].right),
            right_size
        );
    }

    int copy_node(int node){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyRedBlackTree).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
    }

    int make_mutable(int node){
        if(node == -1 || mutation_begin <= node) return node;
        return copy_node(node);
    }

    int new_node(const Key& key, const S& value){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyRedBlackTree).");
        }
        int node = used++;
        const auto current_node_index_2 = static_cast<std::size_t>(node);
        nodes[current_node_index_2].left = -1;
        nodes[current_node_index_2].right = -1;
        nodes[current_node_index_2].size = 1;
        nodes[current_node_index_2].red = true;
        nodes[current_node_index_2].lazy_flag = false;
        nodes[current_node_index_2].key = key;
        nodes[current_node_index_2].min_key = key;
        nodes[current_node_index_2].max_key = key;
        nodes[current_node_index_2].value = value;
        nodes[current_node_index_2].aggregate = value;
        nodes[current_node_index_2].lazy = MonoidActLen.id();
        return node;
    }

    void all_apply_node(int node, const T& f){
        if(node == -1) return;
        const auto current_node_index_3 = static_cast<std::size_t>(node);
        nodes[current_node_index_3].value = MonoidActLen.mapping(f, nodes[current_node_index_3].value, 1);
        nodes[current_node_index_3].aggregate = MonoidActLen.mapping(f, nodes[current_node_index_3].aggregate, nodes[current_node_index_3].size);
        nodes[current_node_index_3].lazy = MonoidActLen.composition(f, nodes[current_node_index_3].lazy);
        nodes[current_node_index_3].lazy_flag = true;
    }

    void push(int node){
        const auto current_node_index_4 = static_cast<std::size_t>(node);
        if(!nodes[current_node_index_4].lazy_flag) return;
        nodes[current_node_index_4].left = make_mutable(nodes[current_node_index_4].left);
        nodes[current_node_index_4].right = make_mutable(nodes[current_node_index_4].right);
        all_apply_node(nodes[current_node_index_4].left, nodes[current_node_index_4].lazy);
        all_apply_node(nodes[current_node_index_4].right, nodes[current_node_index_4].lazy);
        nodes[current_node_index_4].lazy = MonoidActLen.id();
        nodes[current_node_index_4].lazy_flag = false;
    }

    int rotate_left(int root){
        push(root);
        const auto current_node_index_5 = static_cast<std::size_t>(root);
        nodes[current_node_index_5].right = make_mutable(nodes[current_node_index_5].right);
        int right = nodes[current_node_index_5].right;
        push(right);
        nodes[current_node_index_5].right = nodes[static_cast<std::size_t>(right)].left;
        nodes[static_cast<std::size_t>(right)].left = root;
        nodes[static_cast<std::size_t>(right)].red = nodes[current_node_index_5].red;
        nodes[current_node_index_5].red = true;
        pull(root);
        pull(right);
        return right;
    }

    int rotate_right(int root){
        push(root);
        const auto current_node_index_6 = static_cast<std::size_t>(root);
        nodes[current_node_index_6].left = make_mutable(nodes[current_node_index_6].left);
        int left = nodes[current_node_index_6].left;
        push(left);
        nodes[current_node_index_6].left = nodes[static_cast<std::size_t>(left)].right;
        nodes[static_cast<std::size_t>(left)].right = root;
        nodes[static_cast<std::size_t>(left)].red = nodes[current_node_index_6].red;
        nodes[current_node_index_6].red = true;
        pull(root);
        pull(left);
        return left;
    }

    void color_flip(int root){
        const auto current_node_index_7 = static_cast<std::size_t>(root);
        nodes[current_node_index_7].left = make_mutable(nodes[current_node_index_7].left);
        nodes[current_node_index_7].right = make_mutable(nodes[current_node_index_7].right);
        nodes[current_node_index_7].red = !nodes[current_node_index_7].red;
        if(nodes[current_node_index_7].left != -1){
            nodes[static_cast<std::size_t>(nodes[current_node_index_7].left)].red =
                !nodes[static_cast<std::size_t>(nodes[current_node_index_7].left)].red;
        }
        if(nodes[current_node_index_7].right != -1){
            nodes[static_cast<std::size_t>(nodes[current_node_index_7].right)].red =
                !nodes[static_cast<std::size_t>(nodes[current_node_index_7].right)].red;
        }
    }

    int fix_up(int root){
        push(root);
        if(is_red(nodes[static_cast<std::size_t>(root)].right) &&
           !is_red(nodes[static_cast<std::size_t>(root)].left)){
            root = rotate_left(root);
        }
        if(is_red(nodes[static_cast<std::size_t>(root)].left) &&
           is_red(left_of(nodes[static_cast<std::size_t>(root)].left))){
            root = rotate_right(root);
        }
        if(is_red(nodes[static_cast<std::size_t>(root)].left) &&
           is_red(nodes[static_cast<std::size_t>(root)].right)){
            color_flip(root);
        }
        pull(root);
        return root;
    }

    int move_red_left(int root){
        push(root);
        color_flip(root);
        int right = nodes[static_cast<std::size_t>(root)].right;
        if(is_red(left_of(right))){
            nodes[static_cast<std::size_t>(root)].right = rotate_right(right);
            root = rotate_left(root);
            color_flip(root);
        }
        return root;
    }

    int move_red_right(int root){
        push(root);
        color_flip(root);
        if(is_red(left_of(nodes[static_cast<std::size_t>(root)].left))){
            root = rotate_right(root);
            color_flip(root);
        }
        return root;
    }

    int insert_rec(int root, const Key& key, const S& value){
        if(root == -1) return new_node(key, value);
        root = make_mutable(root);
        push(root);
        const auto current_node_index_8 = static_cast<std::size_t>(root);
        if(comp(key, nodes[current_node_index_8].key)){
            nodes[current_node_index_8].left = insert_rec(nodes[current_node_index_8].left, key, value);
        }else{
            nodes[current_node_index_8].right = insert_rec(nodes[current_node_index_8].right, key, value);
        }
        return fix_up(root);
    }

    std::pair<Key, S> min_entry(int root, T carry) const{
        while(true){
            const auto current_node_index_9 = static_cast<std::size_t>(root);
            if(nodes[current_node_index_9].left == -1){
                return {nodes[current_node_index_9].key, MonoidActLen.mapping(carry, nodes[current_node_index_9].value, 1)};
            }
            if(nodes[current_node_index_9].lazy_flag){
                carry = MonoidActLen.composition(carry, nodes[current_node_index_9].lazy);
            }
            root = nodes[current_node_index_9].left;
        }
    }

    int erase_min(int root){
        root = make_mutable(root);
        push(root);
        if(nodes[static_cast<std::size_t>(root)].left == -1) return -1;
        if(!is_red(nodes[static_cast<std::size_t>(root)].left) &&
           !is_red(left_of(nodes[static_cast<std::size_t>(root)].left))){
            root = move_red_left(root);
        }
        nodes[static_cast<std::size_t>(root)].left =
            erase_min(nodes[static_cast<std::size_t>(root)].left);
        return fix_up(root);
    }

    int erase_rec(int root, const Key& key){
        root = make_mutable(root);
        push(root);
        if(comp(key, nodes[static_cast<std::size_t>(root)].key)){
            if(!is_red(nodes[static_cast<std::size_t>(root)].left) &&
               !is_red(left_of(nodes[static_cast<std::size_t>(root)].left))){
                root = move_red_left(root);
            }
            nodes[static_cast<std::size_t>(root)].left =
                erase_rec(nodes[static_cast<std::size_t>(root)].left, key);
        }else{
            if(is_red(nodes[static_cast<std::size_t>(root)].left)){
                root = rotate_right(root);
            }
            if(equal_key(key, nodes[static_cast<std::size_t>(root)].key) &&
               nodes[static_cast<std::size_t>(root)].right == -1){
                return -1;
            }
            if(!is_red(nodes[static_cast<std::size_t>(root)].right) &&
               !is_red(left_of(nodes[static_cast<std::size_t>(root)].right))){
                root = move_red_right(root);
            }
            if(equal_key(key, nodes[static_cast<std::size_t>(root)].key)){
                auto successor = min_entry(
                    nodes[static_cast<std::size_t>(root)].right,
                    MonoidActLen.id()
                );
                nodes[static_cast<std::size_t>(root)].key = successor.first;
                nodes[static_cast<std::size_t>(root)].value = successor.second;
                nodes[static_cast<std::size_t>(root)].right =
                    erase_min(nodes[static_cast<std::size_t>(root)].right);
            }else{
                nodes[static_cast<std::size_t>(root)].right =
                    erase_rec(nodes[static_cast<std::size_t>(root)].right, key);
            }
        }
        return fix_up(root);
    }

    int set_rec(int root, const Key& key, const S& value){
        root = make_mutable(root);
        push(root);
        const auto current_node_index_10 = static_cast<std::size_t>(root);
        if(comp(key, nodes[current_node_index_10].key)){
            nodes[current_node_index_10].left = set_rec(nodes[current_node_index_10].left, key, value);
        }else if(comp(nodes[current_node_index_10].key, key)){
            nodes[current_node_index_10].right = set_rec(nodes[current_node_index_10].right, key, value);
        }else{
            nodes[current_node_index_10].value = value;
        }
        pull(root);
        return root;
    }

    bool disjoint(int node, const Key& l, const Key& r) const{
        const auto current_node_index_11 = static_cast<std::size_t>(node);
        return comp(nodes[current_node_index_11].max_key, l) || !comp(nodes[current_node_index_11].min_key, r);
    }

    bool covered(int node, const Key& l, const Key& r) const{
        const auto current_node_index_12 = static_cast<std::size_t>(node);
        return !comp(nodes[current_node_index_12].min_key, l) && comp(nodes[current_node_index_12].max_key, r);
    }

    bool in_range(const Key& key, const Key& l, const Key& r) const{
        return !comp(key, l) && comp(key, r);
    }

    int apply_rec(int root, const Key& l, const Key& r, const T& f){
        if(root == -1 || disjoint(root, l, r)) return root;
        root = make_mutable(root);
        if(covered(root, l, r)){
            all_apply_node(root, f);
            return root;
        }
        push(root);
        const auto current_node_index_13 = static_cast<std::size_t>(root);
        nodes[current_node_index_13].left = apply_rec(nodes[current_node_index_13].left, l, r, f);
        if(in_range(nodes[current_node_index_13].key, l, r)){
            nodes[current_node_index_13].value = MonoidActLen.mapping(f, nodes[current_node_index_13].value, 1);
        }
        nodes[current_node_index_13].right = apply_rec(nodes[current_node_index_13].right, l, r, f);
        pull(root);
        return root;
    }

    T compose_carry(int node, const T& carry) const{
        const auto current_node_index_14 = static_cast<std::size_t>(node);
        if(!nodes[current_node_index_14].lazy_flag) return carry;
        return MonoidActLen.composition(carry, nodes[current_node_index_14].lazy);
    }

    std::optional<S> get_rec(int node, const Key& key, const T& carry) const{
        if(node == -1) return std::nullopt;
        const auto current_node_index_15 = static_cast<std::size_t>(node);
        if(equal_key(nodes[current_node_index_15].key, key)){
            return MonoidActLen.mapping(carry, nodes[current_node_index_15].value, 1);
        }
        T child_carry = compose_carry(node, carry);
        if(comp(key, nodes[current_node_index_15].key)){
            return get_rec(nodes[current_node_index_15].left, key, child_carry);
        }
        return get_rec(nodes[current_node_index_15].right, key, child_carry);
    }

    std::pair<S, int> prod_rec(int node, const Key& l, const Key& r, const T& carry) const{
        if(node == -1 || disjoint(node, l, r)) return {MonoidActLen.e(), 0};
        const auto current_node_index_16 = static_cast<std::size_t>(node);
        if(covered(node, l, r)){
            return {MonoidActLen.mapping(carry, nodes[current_node_index_16].aggregate, nodes[current_node_index_16].size), nodes[current_node_index_16].size};
        }
        T child_carry = compose_carry(node, carry);
        auto [left_value, left_size] = prod_rec(nodes[current_node_index_16].left, l, r, child_carry);
        S middle_value = MonoidActLen.e();
        int middle_size = 0;
        if(in_range(nodes[current_node_index_16].key, l, r)){
            middle_value = MonoidActLen.mapping(carry, nodes[current_node_index_16].value, 1);
            middle_size = 1;
        }
        auto [right_value, right_size] = prod_rec(nodes[current_node_index_16].right, l, r, child_carry);
        S prefix = MonoidActLen.op(left_value, left_size, middle_value, middle_size);
        return {
            MonoidActLen.op(prefix, left_size + middle_size, right_value, right_size),
            left_size + middle_size + right_size
        };
    }

    void to_vector_rec(int node, const T& carry, std::vector<std::pair<Key, S>>& result) const{
        if(node == -1) return;
        const auto current_node_index_17 = static_cast<std::size_t>(node);
        T child_carry = compose_carry(node, carry);
        to_vector_rec(nodes[current_node_index_17].left, child_carry, result);
        result.push_back({nodes[current_node_index_17].key, MonoidActLen.mapping(carry, nodes[current_node_index_17].value, 1)});
        to_vector_rec(nodes[current_node_index_17].right, child_carry, result);
    }

    void check_version_capacity() const{
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentLazyRedBlackTree).");
        }
    }

    int register_root(int root){
        if constexpr(MAX_VERSION < 0) roots.ensure(static_cast<std::size_t>(version_count) + 1);
        check_version_capacity();
        if constexpr(MAX_VERSION < 0) roots.ensure(static_cast<std::size_t>(version_count) + 1);
        roots[static_cast<std::size_t>(version_count)] = root;
        return version_count++;
    }

    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentLazyRedBlackTree).");
        }
    }

public:
    explicit PersistentLazyRedBlackTree(const Compare& compare = Compare()) : comp(compare){
        roots.ensure(1); roots.fill(-1);
    }

    int versions() const{
        return version_count;
    }

    int size(int version = 0) const{
        check_version(version);
        return size_or_zero(roots[static_cast<std::size_t>(version)]);
    }

    bool empty(int version = 0) const{
        return size(version) == 0;
    }

    bool contains(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        while(node != -1){
            const auto current_node_index_18 = static_cast<std::size_t>(node);
            if(equal_key(nodes[current_node_index_18].key, key)) return true;
            node = comp(key, nodes[current_node_index_18].key) ? nodes[current_node_index_18].left : nodes[current_node_index_18].right;
        }
        return false;
    }

    int insert(const Key& key, const S& value, int version = 0){
        check_version(version);
        check_version_capacity();
        int root = roots[static_cast<std::size_t>(version)];
        if(contains(key, version)) return register_root(root);
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            root = insert_rec(root, key, value);
            nodes[static_cast<std::size_t>(root)].red = false;
            return register_root(root);
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    int erase(const Key& key, int version = 0){
        check_version(version);
        check_version_capacity();
        int root = roots[static_cast<std::size_t>(version)];
        if(!contains(key, version)) return register_root(root);
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            root = make_mutable(root);
            push(root);
            if(!is_red(nodes[static_cast<std::size_t>(root)].left) &&
               !is_red(nodes[static_cast<std::size_t>(root)].right)){
                nodes[static_cast<std::size_t>(root)].red = true;
            }
            root = erase_rec(root, key);
            if(root != -1) nodes[static_cast<std::size_t>(root)].red = false;
            return register_root(root);
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    int set(const Key& key, const S& value, int version = 0){
        check_version(version);
        check_version_capacity();
        int root = roots[static_cast<std::size_t>(version)];
        if(!contains(key, version)) return register_root(root);
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            root = set_rec(root, key, value);
            return register_root(root);
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    std::optional<S> get(const Key& key, int version = 0) const{
        check_version(version);
        return get_rec(roots[static_cast<std::size_t>(version)], key, MonoidActLen.id());
    }

    int apply(const Key& l, const Key& r, const T& f, int version = 0){
        check_version(version);
        check_version_capacity();
        int root = roots[static_cast<std::size_t>(version)];
        if(root == -1 || !comp(l, r)) return register_root(root);
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            root = apply_rec(root, l, r, f);
            return register_root(root);
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    S prod(const Key& l, const Key& r, int version = 0) const{
        check_version(version);
        if(!comp(l, r)) return MonoidActLen.e();
        return prod_rec(roots[static_cast<std::size_t>(version)], l, r, MonoidActLen.id()).first;
    }

    int all_apply(const T& f, int version = 0){
        check_version(version);
        check_version_capacity();
        int root = roots[static_cast<std::size_t>(version)];
        if(root == -1) return register_root(root);
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            root = make_mutable(root);
            all_apply_node(root, f);
            return register_root(root);
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    S all_prod(int version = 0) const{
        check_version(version);
        return aggregate_or_e(roots[static_cast<std::size_t>(version)]);
    }

    int order_of_key(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        int result = 0;
        while(node != -1){
            const auto current_node_index_19 = static_cast<std::size_t>(node);
            if(comp(nodes[current_node_index_19].key, key)){
                result += size_or_zero(nodes[current_node_index_19].left) + 1;
                node = nodes[current_node_index_19].right;
            }else{
                node = nodes[current_node_index_19].left;
            }
        }
        return result;
    }

    std::optional<Key> kth_key(int k, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        if(k < 0 || size_or_zero(node) <= k) return std::nullopt;
        while(node != -1){
            const auto current_node_index_20 = static_cast<std::size_t>(node);
            int left_size = size_or_zero(nodes[current_node_index_20].left);
            if(k < left_size){
                node = nodes[current_node_index_20].left;
            }else if(k == left_size){
                return nodes[current_node_index_20].key;
            }else{
                k -= left_size + 1;
                node = nodes[current_node_index_20].right;
            }
        }
        return std::nullopt;
    }

    std::vector<std::pair<Key, S>> to_vector(int version = 0) const{
        check_version(version);
        std::vector<std::pair<Key, S>> result;
        result.reserve(static_cast<std::size_t>(size(version)));
        to_vector_rec(roots[static_cast<std::size_t>(version)], MonoidActLen.id(), result);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_MAP_PERSISTENT_LAZY_RED_BLACK_TREE_HPP_INCLUDED
