#ifndef CPPLIB_SRC_STRUCTURE_BBST_PERSISTENT_RED_BLACK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_PERSISTENT_RED_BLACK_TREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Key, int MAX_NODE, int MAX_VERSION, class Compare = std::less<Key>>
struct PersistentRedBlackSet{
    static_assert(MAX_NODE > 0);
    static_assert(MAX_VERSION > 0);

private:
    struct Node{
        int left = -1;
        int right = -1;
        int size = 0;
        bool red = false;
        Key key{};
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

    void pull(int node){
        auto& current = nodes[static_cast<std::size_t>(node)];
        current.size = 1 + size_or_zero(current.left) + size_or_zero(current.right);
    }

    int copy_node(int node){
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
        auto& current = nodes[static_cast<std::size_t>(root)];
        if(comp(key, current.key)){
            current.left = insert_rec(current.left, key);
        }else{
            current.right = insert_rec(current.right, key);
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
        check_version_capacity();
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

    int order_of_key_upper(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        int result = 0;
        while(node != -1){
            const auto& current = nodes[static_cast<std::size_t>(node)];
            if(!comp(key, current.key)){
                result += size_or_zero(current.left) + 1;
                node = current.right;
            }else{
                node = current.left;
            }
        }
        return result;
    }

    std::optional<Key> kth(int k, int version = 0) const{
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

    std::optional<Key> lower_bound(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto& current = nodes[static_cast<std::size_t>(node)];
            if(!comp(current.key, key)){
                result = current.key;
                node = current.left;
            }else{
                node = current.right;
            }
        }
        return result;
    }

    std::optional<Key> upper_bound(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto& current = nodes[static_cast<std::size_t>(node)];
            if(comp(key, current.key)){
                result = current.key;
                node = current.left;
            }else{
                node = current.right;
            }
        }
        return result;
    }

    std::optional<Key> max_leq(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto& current = nodes[static_cast<std::size_t>(node)];
            if(!comp(key, current.key)){
                result = current.key;
                node = current.right;
            }else{
                node = current.left;
            }
        }
        return result;
    }

    std::optional<Key> max_less(const Key& key, int version = 0) const{
        check_version(version);
        int node = roots[static_cast<std::size_t>(version)];
        std::optional<Key> result;
        while(node != -1){
            const auto& current = nodes[static_cast<std::size_t>(node)];
            if(comp(current.key, key)){
                result = current.key;
                node = current.right;
            }else{
                node = current.left;
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

#endif  // CPPLIB_SRC_STRUCTURE_BBST_PERSISTENT_RED_BLACK_TREE_HPP_INCLUDED
