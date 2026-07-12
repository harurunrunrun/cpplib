#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
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
        std::uint64_t priority = 0;
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
    Compare comp;

    static std::uint64_t splitmix64(std::uint64_t x){
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    bool equal_key(const Key& a, const Key& b) const{
        return !comp(a, b) && !comp(b, a);
    }

    int size_or_zero(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].size;
    }

    S aggregate_or_e(int node) const{
        return node == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(node)].aggregate;
    }

    void pull(int node){
        auto& current = nodes[static_cast<std::size_t>(node)];
        int left = current.left;
        int right = current.right;
        int left_size = size_or_zero(left);
        int right_size = size_or_zero(right);
        current.size = left_size + 1 + right_size;
        current.min_key = left == -1 ? current.key : nodes[static_cast<std::size_t>(left)].min_key;
        current.max_key = right == -1 ? current.key : nodes[static_cast<std::size_t>(right)].max_key;

        S prefix = MonoidActLen.op(aggregate_or_e(left), left_size, current.value, 1);
        current.aggregate = MonoidActLen.op(prefix, left_size + 1, aggregate_or_e(right), right_size);
    }

    int copy_node(int node){
        if(node == -1) return -1;
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyRedBlackTree).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
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
        current.priority = splitmix64(static_cast<std::uint64_t>(node) + 1);
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
        if(node == -1) return;
        auto& current = nodes[static_cast<std::size_t>(node)];
        if(!current.lazy_flag) return;
        if(current.left != -1){
            current.left = copy_node(current.left);
            all_apply_node(current.left, current.lazy);
        }
        if(current.right != -1){
            current.right = copy_node(current.right);
            all_apply_node(current.right, current.lazy);
        }
        current.lazy = MonoidActLen.id();
        current.lazy_flag = false;
    }

    std::pair<int, int> split_less(int root, const Key& key){
        if(root == -1) return {-1, -1};
        int node = copy_node(root);
        push(node);
        auto& current = nodes[static_cast<std::size_t>(node)];
        if(comp(current.key, key)){
            auto [left, right] = split_less(current.right, key);
            current.right = left;
            pull(node);
            return {node, right};
        }else{
            auto [left, right] = split_less(current.left, key);
            current.left = right;
            pull(node);
            return {left, node};
        }
    }

    int merge(int left, int right){
        if(left == -1) return right;
        if(right == -1) return left;
        if(nodes[static_cast<std::size_t>(left)].priority < nodes[static_cast<std::size_t>(right)].priority){
            int node = copy_node(left);
            push(node);
            nodes[static_cast<std::size_t>(node)].right = merge(nodes[static_cast<std::size_t>(node)].right, right);
            pull(node);
            return node;
        }else{
            int node = copy_node(right);
            push(node);
            nodes[static_cast<std::size_t>(node)].left = merge(left, nodes[static_cast<std::size_t>(node)].left);
            pull(node);
            return node;
        }
    }

    int erase_rec(int root, const Key& key, bool& erased){
        if(root == -1) return -1;
        int node = copy_node(root);
        push(node);
        auto& current = nodes[static_cast<std::size_t>(node)];
        if(comp(key, current.key)){
            current.left = erase_rec(current.left, key, erased);
            pull(node);
            return node;
        }
        if(comp(current.key, key)){
            current.right = erase_rec(current.right, key, erased);
            pull(node);
            return node;
        }
        erased = true;
        return merge(current.left, current.right);
    }

    int set_rec(int root, const Key& key, const S& value, bool& found){
        if(root == -1) return -1;
        int node = copy_node(root);
        push(node);
        auto& current = nodes[static_cast<std::size_t>(node)];
        if(comp(key, current.key)){
            current.left = set_rec(current.left, key, value, found);
            pull(node);
            return node;
        }
        if(comp(current.key, key)){
            current.right = set_rec(current.right, key, value, found);
            pull(node);
            return node;
        }
        found = true;
        current.value = value;
        pull(node);
        return node;
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
        S mid_value = MonoidActLen.e();
        int mid_size = 0;
        if(in_range(current.key, l, r)){
            mid_value = MonoidActLen.mapping(carry, current.value, 1);
            mid_size = 1;
        }
        auto [right_value, right_size] = prod_rec(current.right, l, r, child_carry);
        S prefix = MonoidActLen.op(left_value, left_size, mid_value, mid_size);
        return {
            MonoidActLen.op(prefix, left_size + mid_size, right_value, right_size),
            left_size + mid_size + right_size
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

    int register_root(int root){
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentLazyRedBlackTree).");
        }
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
        int root = roots[static_cast<std::size_t>(version)];
        if(contains(key, version)) return register_root(root);
        auto [left, right] = split_less(root, key);
        int node = new_node(key, value);
        return register_root(merge(merge(left, node), right));
    }

    int erase(const Key& key, int version = 0){
        check_version(version);
        int root = roots[static_cast<std::size_t>(version)];
        if(!contains(key, version)) return register_root(root);
        bool erased = false;
        return register_root(erase_rec(root, key, erased));
    }

    int set(const Key& key, const S& value, int version = 0){
        check_version(version);
        int root = roots[static_cast<std::size_t>(version)];
        if(!contains(key, version)) return register_root(root);
        bool found = false;
        return register_root(set_rec(root, key, value, found));
    }

    std::optional<S> get(const Key& key, int version = 0) const{
        check_version(version);
        return get_rec(roots[static_cast<std::size_t>(version)], key, MonoidActLen.id());
    }

    int apply(const Key& l, const Key& r, const T& f, int version = 0){
        check_version(version);
        int root = roots[static_cast<std::size_t>(version)];
        if(root == -1 || !comp(l, r)) return register_root(root);
        auto [less_l, ge_l] = split_less(root, l);
        auto [mid, ge_r] = split_less(ge_l, r);
        all_apply_node(mid, f);
        return register_root(merge(merge(less_l, mid), ge_r));
    }

    S prod(const Key& l, const Key& r, int version = 0) const{
        check_version(version);
        if(!comp(l, r)) return MonoidActLen.e();
        return prod_rec(roots[static_cast<std::size_t>(version)], l, r, MonoidActLen.id()).first;
    }

    int all_apply(const T& f, int version = 0){
        check_version(version);
        int root = roots[static_cast<std::size_t>(version)];
        if(root == -1) return register_root(root);
        root = copy_node(root);
        all_apply_node(root, f);
        return register_root(root);
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
