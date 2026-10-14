#ifndef CPPLIB_SRC_STRUCTURE_BBST_MAP_PERSISTENT_RED_BLACK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_MAP_PERSISTENT_RED_BLACK_TREE_HPP_INCLUDED

#include <array>
#include "../../array/static_or_vector.hpp"
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Key, int MAX_NODE = -1, int MAX_VERSION = -1, class Compare = std::less<Key>>
struct PersistentRedBlackSet{
    static_assert(MAX_NODE == -1 || MAX_NODE > 0);
    static_assert(MAX_VERSION == -1 || MAX_VERSION > 0);

private:
    struct Node{
        int left = -1;
        int right = -1;
        int size = 0;
        bool red = false;
        Key key{};
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

    void pull(int node){
        const auto current_node_index_1 = static_cast<std::size_t>(node);
        nodes[current_node_index_1].size = 1 + size_or_zero(nodes[current_node_index_1].left) + size_or_zero(nodes[current_node_index_1].right);
    }

    int copy_node(int node){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentRedBlackSet).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
    }

    int make_mutable(int node){
        if(node == -1 || mutation_begin <= node) return node;
        return copy_node(node);
    }

    int new_node(const Key& key){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentRedBlackSet).");
        }
        int node = used++;
        nodes[static_cast<std::size_t>(node)] = {-1, -1, 1, true, key};
        return node;
    }

    int rotate_left(int root){
        int right = make_mutable(nodes[static_cast<std::size_t>(root)].right);
        nodes[static_cast<std::size_t>(root)].right = nodes[static_cast<std::size_t>(right)].left;
        nodes[static_cast<std::size_t>(right)].left = root;
        nodes[static_cast<std::size_t>(right)].red = nodes[static_cast<std::size_t>(root)].red;
        nodes[static_cast<std::size_t>(root)].red = true;
        pull(root);
        pull(right);
        return right;
    }

    int rotate_right(int root){
        int left = make_mutable(nodes[static_cast<std::size_t>(root)].left);
        nodes[static_cast<std::size_t>(root)].left = nodes[static_cast<std::size_t>(left)].right;
        nodes[static_cast<std::size_t>(left)].right = root;
        nodes[static_cast<std::size_t>(left)].red = nodes[static_cast<std::size_t>(root)].red;
        nodes[static_cast<std::size_t>(root)].red = true;
        pull(root);
        pull(left);
        return left;
    }

    void color_flip(int root){
        const auto current_node_index_2 = static_cast<std::size_t>(root);
        nodes[current_node_index_2].left = make_mutable(nodes[current_node_index_2].left);
        nodes[current_node_index_2].right = make_mutable(nodes[current_node_index_2].right);
        nodes[current_node_index_2].red = !nodes[current_node_index_2].red;
        if(nodes[current_node_index_2].left != -1){
            nodes[static_cast<std::size_t>(nodes[current_node_index_2].left)].red =
                !nodes[static_cast<std::size_t>(nodes[current_node_index_2].left)].red;
        }
        if(nodes[current_node_index_2].right != -1){
            nodes[static_cast<std::size_t>(nodes[current_node_index_2].right)].red =
                !nodes[static_cast<std::size_t>(nodes[current_node_index_2].right)].red;
        }
    }

    int fix_up(int root){
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
        color_flip(root);
        if(is_red(left_of(nodes[static_cast<std::size_t>(root)].left))){
            root = rotate_right(root);
            color_flip(root);
        }
        return root;
    }

    int insert_rec(int root, const Key& key){
        if(root == -1) return new_node(key);
        root = make_mutable(root);
        const auto current_node_index_3 = static_cast<std::size_t>(root);
        if(comp(key, nodes[current_node_index_3].key)){
            nodes[current_node_index_3].left = insert_rec(nodes[current_node_index_3].left, key);
        }else{
            nodes[current_node_index_3].right = insert_rec(nodes[current_node_index_3].right, key);
        }
        return fix_up(root);
    }

    int min_node(int root) const{
        while(nodes[static_cast<std::size_t>(root)].left != -1){
            root = nodes[static_cast<std::size_t>(root)].left;
        }
        return root;
    }

    int erase_min(int root){
        root = make_mutable(root);
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
                int successor = min_node(nodes[static_cast<std::size_t>(root)].right);
                nodes[static_cast<std::size_t>(root)].key =
                    nodes[static_cast<std::size_t>(successor)].key;
                nodes[static_cast<std::size_t>(root)].right =
                    erase_min(nodes[static_cast<std::size_t>(root)].right);
            }else{
                nodes[static_cast<std::size_t>(root)].right =
                    erase_rec(nodes[static_cast<std::size_t>(root)].right, key);
            }
        }
        return fix_up(root);
    }

    void check_version_capacity() const{
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentRedBlackSet).");
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
            throw std::runtime_error("library assertion fault: range violation (PersistentRedBlackSet).");
        }
    }

public:
    explicit PersistentRedBlackSet(const Compare& compare = Compare()) : comp(compare){
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
            const auto current_node_index_4 = static_cast<std::size_t>(node);
            if(equal_key(nodes[current_node_index_4].key, key)) return true;
            node = comp(key, nodes[current_node_index_4].key) ? nodes[current_node_index_4].left : nodes[current_node_index_4].right;
        }
        return false;
    }

    int insert(const Key& key, int version = 0){
        check_version(version);
        check_version_capacity();
        int root = roots[static_cast<std::size_t>(version)];
        if(contains(key, version)) return register_root(root);
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            root = insert_rec(root, key);
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

    int order_of_key(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        int result = 0;
        while(node != -1){
            const auto current_node_index_5 = static_cast<std::size_t>(node);
            if(comp(nodes[current_node_index_5].key, key)){
                result += size_or_zero(nodes[current_node_index_5].left) + 1;
                node = nodes[current_node_index_5].right;
            }else{
                node = nodes[current_node_index_5].left;
            }
        }
        return result;
    }

    int order_of_key_upper(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        int result = 0;
        while(node != -1){
            const auto current_node_index_6 = static_cast<std::size_t>(node);
            if(!comp(key, nodes[current_node_index_6].key)){
                result += size_or_zero(nodes[current_node_index_6].left) + 1;
                node = nodes[current_node_index_6].right;
            }else{
                node = nodes[current_node_index_6].left;
            }
        }
        return result;
    }

    std::optional<Key> kth(int k, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        if(k < 0 || size_or_zero(node) <= k) return std::nullopt;
        while(node != -1){
            const auto current_node_index_7 = static_cast<std::size_t>(node);
            int left_size = size_or_zero(nodes[current_node_index_7].left);
            if(k < left_size){
                node = nodes[current_node_index_7].left;
            }else if(k == left_size){
                return nodes[current_node_index_7].key;
            }else{
                k -= left_size + 1;
                node = nodes[current_node_index_7].right;
            }
        }
        return std::nullopt;
    }

    std::optional<Key> lower_bound(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto current_node_index_8 = static_cast<std::size_t>(node);
            if(!comp(nodes[current_node_index_8].key, key)){
                result = nodes[current_node_index_8].key;
                node = nodes[current_node_index_8].left;
            }else{
                node = nodes[current_node_index_8].right;
            }
        }
        return result;
    }

    std::optional<Key> upper_bound(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto current_node_index_9 = static_cast<std::size_t>(node);
            if(comp(key, nodes[current_node_index_9].key)){
                result = nodes[current_node_index_9].key;
                node = nodes[current_node_index_9].left;
            }else{
                node = nodes[current_node_index_9].right;
            }
        }
        return result;
    }

    std::optional<Key> max_leq(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto current_node_index_10 = static_cast<std::size_t>(node);
            if(!comp(key, nodes[current_node_index_10].key)){
                result = nodes[current_node_index_10].key;
                node = nodes[current_node_index_10].right;
            }else{
                node = nodes[current_node_index_10].left;
            }
        }
        return result;
    }

    std::optional<Key> max_less(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto current_node_index_11 = static_cast<std::size_t>(node);
            if(comp(nodes[current_node_index_11].key, key)){
                result = nodes[current_node_index_11].key;
                node = nodes[current_node_index_11].right;
            }else{
                node = nodes[current_node_index_11].left;
            }
        }
        return result;
    }

    std::vector<Key> to_vector(int version = 0) const{
        check_version(version);
        std::vector<Key> result;
        result.reserve(static_cast<std::size_t>(size(version)));
        std::vector<int> stack;
        int node = roots[static_cast<std::size_t>(version)];
        while(node != -1 || !stack.empty()){
            while(node != -1){
                stack.push_back(node);
                node = nodes[static_cast<std::size_t>(node)].left;
            }
            node = stack.back();
            stack.pop_back();
            result.push_back(nodes[static_cast<std::size_t>(node)].key);
            node = nodes[static_cast<std::size_t>(node)].right;
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_MAP_PERSISTENT_RED_BLACK_TREE_HPP_INCLUDED
