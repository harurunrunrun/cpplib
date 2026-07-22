#ifndef CPPLIB_SRC_STRUCTURE_BBST_PERSISTENT_LAZY_RED_BLACK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_PERSISTENT_LAZY_RED_BLACK_TREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<class Key, auto MonoidActLen, int MAX_NODE, int MAX_VERSION, class Compare = std::less<Key>>
struct PersistentLazyRedBlackTree{
    static_assert(MAX_NODE > 0);
    static_assert(MAX_VERSION > 0);

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

    std::array<Node, MAX_NODE> nodes{};
    std::array<int, MAX_VERSION> roots{};
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
        auto& current = nodes[static_cast<std::size_t>(node)];
        int left_size = size_or_zero(current.left);
        int right_size = size_or_zero(current.right);
        current.size = left_size + 1 + right_size;
        current.min_key = current.left == -1
            ? current.key
            : nodes[static_cast<std::size_t>(current.left)].min_key;
        current.max_key = current.right == -1
            ? current.key
            : nodes[static_cast<std::size_t>(current.right)].max_key;
        S prefix = MonoidActLen.op(aggregate_or_e(current.left), left_size, current.value, 1);
        current.aggregate = MonoidActLen.op(
            prefix,
            left_size + 1,
            aggregate_or_e(current.right),
            right_size
        );
    }

    int copy_node(int node){
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
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyRedBlackTree).");
        }
        int node = used++;
        auto& current = nodes[static_cast<std::size_t>(node)];
        current.left = -1;
        current.right = -1;
        current.size = 1;
        current.red = true;
        current.lazy_flag = false;
        current.key = key;
        current.min_key = key;
        current.max_key = key;
        current.value = value;
        current.aggregate = value;
        current.lazy = MonoidActLen.id();
        return node;
    }

    void all_apply_node(int node, const T& f){
        if(node == -1) return;
        auto& current = nodes[static_cast<std::size_t>(node)];
        current.value = MonoidActLen.mapping(f, current.value, 1);
        current.aggregate = MonoidActLen.mapping(f, current.aggregate, current.size);
        current.lazy = MonoidActLen.composition(f, current.lazy);
        current.lazy_flag = true;
    }

    void push(int node){
        auto& current = nodes[static_cast<std::size_t>(node)];
        if(!current.lazy_flag) return;
        current.left = make_mutable(current.left);
        current.right = make_mutable(current.right);
        all_apply_node(current.left, current.lazy);
        all_apply_node(current.right, current.lazy);
        current.lazy = MonoidActLen.id();
        current.lazy_flag = false;
    }

    int rotate_left(int root){
        push(root);
        auto& current = nodes[static_cast<std::size_t>(root)];
        current.right = make_mutable(current.right);
        int right = current.right;
        push(right);
        current.right = nodes[static_cast<std::size_t>(right)].left;
        nodes[static_cast<std::size_t>(right)].left = root;
        nodes[static_cast<std::size_t>(right)].red = current.red;
        current.red = true;
        pull(root);
        pull(right);
        return right;
    }

    int rotate_right(int root){
        push(root);
        auto& current = nodes[static_cast<std::size_t>(root)];
        current.left = make_mutable(current.left);
        int left = current.left;
        push(left);
        current.left = nodes[static_cast<std::size_t>(left)].right;
        nodes[static_cast<std::size_t>(left)].right = root;
        nodes[static_cast<std::size_t>(left)].red = current.red;
        current.red = true;
        pull(root);
        pull(left);
        return left;
    }

    void color_flip(int root){
        auto& current = nodes[static_cast<std::size_t>(root)];
        current.left = make_mutable(current.left);
        current.right = make_mutable(current.right);
        current.red = !current.red;
        if(current.left != -1){
            nodes[static_cast<std::size_t>(current.left)].red =
                !nodes[static_cast<std::size_t>(current.left)].red;
        }
        if(current.right != -1){
            nodes[static_cast<std::size_t>(current.right)].red =
                !nodes[static_cast<std::size_t>(current.right)].red;
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
        auto& current = nodes[static_cast<std::size_t>(root)];
        if(comp(key, current.key)){
            current.left = insert_rec(current.left, key, value);
        }else{
            current.right = insert_rec(current.right, key, value);
        }
        return fix_up(root);
    }

    std::pair<Key, S> min_entry(int root, T carry) const{
        while(true){
            const auto& current = nodes[static_cast<std::size_t>(root)];
            if(current.left == -1){
                return {current.key, MonoidActLen.mapping(carry, current.value, 1)};
            }
            if(current.lazy_flag){
                carry = MonoidActLen.composition(carry, current.lazy);
            }
            root = current.left;
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
        auto& current = nodes[static_cast<std::size_t>(root)];
        if(comp(key, current.key)){
            current.left = set_rec(current.left, key, value);
        }else if(comp(current.key, key)){
            current.right = set_rec(current.right, key, value);
        }else{
            current.value = value;
        }
        pull(root);
        return root;
    }

    bool disjoint(int node, const Key& l, const Key& r) const{
        const auto& current = nodes[static_cast<std::size_t>(node)];
        return comp(current.max_key, l) || !comp(current.min_key, r);
    }

    bool covered(int node, const Key& l, const Key& r) const{
        const auto& current = nodes[static_cast<std::size_t>(node)];
        return !comp(current.min_key, l) && comp(current.max_key, r);
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
        auto& current = nodes[static_cast<std::size_t>(root)];
        current.left = apply_rec(current.left, l, r, f);
        if(in_range(current.key, l, r)){
            current.value = MonoidActLen.mapping(f, current.value, 1);
        }
        current.right = apply_rec(current.right, l, r, f);
        pull(root);
        return root;
    }

    T compose_carry(int node, const T& carry) const{
        const auto& current = nodes[static_cast<std::size_t>(node)];
        if(!current.lazy_flag) return carry;
        return MonoidActLen.composition(carry, current.lazy);
    }

    std::optional<S> get_rec(int node, const Key& key, const T& carry) const{
        if(node == -1) return std::nullopt;
        const auto& current = nodes[static_cast<std::size_t>(node)];
        if(equal_key(current.key, key)){
            return MonoidActLen.mapping(carry, current.value, 1);
        }
        T child_carry = compose_carry(node, carry);
        if(comp(key, current.key)){
            return get_rec(current.left, key, child_carry);
        }
        return get_rec(current.right, key, child_carry);
    }

    std::pair<S, int> prod_rec(int node, const Key& l, const Key& r, const T& carry) const{
        if(node == -1 || disjoint(node, l, r)) return {MonoidActLen.e(), 0};
        const auto& current = nodes[static_cast<std::size_t>(node)];
        if(covered(node, l, r)){
            return {MonoidActLen.mapping(carry, current.aggregate, current.size), current.size};
        }
        T child_carry = compose_carry(node, carry);
        auto [left_value, left_size] = prod_rec(current.left, l, r, child_carry);
        S middle_value = MonoidActLen.e();
        int middle_size = 0;
        if(in_range(current.key, l, r)){
            middle_value = MonoidActLen.mapping(carry, current.value, 1);
            middle_size = 1;
        }
        auto [right_value, right_size] = prod_rec(current.right, l, r, child_carry);
        S prefix = MonoidActLen.op(left_value, left_size, middle_value, middle_size);
        return {
            MonoidActLen.op(prefix, left_size + middle_size, right_value, right_size),
            left_size + middle_size + right_size
        };
    }

    void to_vector_rec(int node, const T& carry, std::vector<std::pair<Key, S>>& result) const{
        if(node == -1) return;
        const auto& current = nodes[static_cast<std::size_t>(node)];
        T child_carry = compose_carry(node, carry);
        to_vector_rec(current.left, child_carry, result);
        result.push_back({current.key, MonoidActLen.mapping(carry, current.value, 1)});
        to_vector_rec(current.right, child_carry, result);
    }

    void check_version_capacity() const{
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentLazyRedBlackTree).");
        }
    }

    int register_root(int root){
        check_version_capacity();
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
        roots.fill(-1);
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
            const auto& current = nodes[static_cast<std::size_t>(node)];
            if(equal_key(current.key, key)) return true;
            node = comp(key, current.key) ? current.left : current.right;
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
            const auto& current = nodes[static_cast<std::size_t>(node)];
            if(comp(current.key, key)){
                result += size_or_zero(current.left) + 1;
                node = current.right;
            }else{
                node = current.left;
            }
        }
        return result;
    }

    std::optional<Key> kth_key(int k, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        if(k < 0 || size_or_zero(node) <= k) return std::nullopt;
        while(node != -1){
            const auto& current = nodes[static_cast<std::size_t>(node)];
            int left_size = size_or_zero(current.left);
            if(k < left_size){
                node = current.left;
            }else if(k == left_size){
                return current.key;
            }else{
                k -= left_size + 1;
                node = current.right;
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

#endif  // CPPLIB_SRC_STRUCTURE_BBST_PERSISTENT_LAZY_RED_BLACK_TREE_HPP_INCLUDED
