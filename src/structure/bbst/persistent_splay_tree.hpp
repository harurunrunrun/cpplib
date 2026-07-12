#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

template<class Key, int MAX_NODE, int MAX_VERSION, class Compare = std::less<Key>>
struct PersistentSplaySet{
    static_assert(MAX_NODE > 0);
    static_assert(MAX_VERSION > 0);

private:
    struct Node{
        int left = -1;
        int right = -1;
        int size = 0;
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

    int size_or_zero(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].size;
    }

    void pull(int node){
        auto& current = nodes[static_cast<std::size_t>(node)];
        current.size = 1 + size_or_zero(current.left) + size_or_zero(current.right);
    }

    int copy_node(int node){
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentSplaySet).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
    }

    int make_mutable(int node){
        if(node == -1 || mutation_begin <= node) return node;
        return copy_node(node);
    }

    int new_node(const Key& key, int left = -1, int right = -1){
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentSplaySet).");
        }
        int node = used++;
        nodes[static_cast<std::size_t>(node)] = {left, right, 1, key};
        pull(node);
        return node;
    }

    int rotate_left(int root){
        int right = make_mutable(nodes[static_cast<std::size_t>(root)].right);
        nodes[static_cast<std::size_t>(root)].right =
            nodes[static_cast<std::size_t>(right)].left;
        nodes[static_cast<std::size_t>(right)].left = root;
        pull(root);
        pull(right);
        return right;
    }

    int rotate_right(int root){
        int left = make_mutable(nodes[static_cast<std::size_t>(root)].left);
        nodes[static_cast<std::size_t>(root)].left =
            nodes[static_cast<std::size_t>(left)].right;
        nodes[static_cast<std::size_t>(left)].right = root;
        pull(root);
        pull(left);
        return left;
    }

    int splay(int root, const Key& key){
        if(root == -1) return -1;
        root = make_mutable(root);

        if(comp(key, nodes[static_cast<std::size_t>(root)].key)){
            int left = nodes[static_cast<std::size_t>(root)].left;
            if(left == -1) return root;

            if(comp(key, nodes[static_cast<std::size_t>(left)].key)){
                left = make_mutable(left);
                nodes[static_cast<std::size_t>(root)].left = left;
                nodes[static_cast<std::size_t>(left)].left =
                    splay(nodes[static_cast<std::size_t>(left)].left, key);
                pull(left);
                root = rotate_right(root);
            }else if(comp(nodes[static_cast<std::size_t>(left)].key, key)){
                left = make_mutable(left);
                nodes[static_cast<std::size_t>(root)].left = left;
                nodes[static_cast<std::size_t>(left)].right =
                    splay(nodes[static_cast<std::size_t>(left)].right, key);
                pull(left);
                if(nodes[static_cast<std::size_t>(left)].right != -1){
                    nodes[static_cast<std::size_t>(root)].left = rotate_left(left);
                }
            }
            pull(root);
            if(nodes[static_cast<std::size_t>(root)].left == -1) return root;
            return rotate_right(root);
        }

        if(comp(nodes[static_cast<std::size_t>(root)].key, key)){
            int right = nodes[static_cast<std::size_t>(root)].right;
            if(right == -1) return root;

            if(comp(nodes[static_cast<std::size_t>(right)].key, key)){
                right = make_mutable(right);
                nodes[static_cast<std::size_t>(root)].right = right;
                nodes[static_cast<std::size_t>(right)].right =
                    splay(nodes[static_cast<std::size_t>(right)].right, key);
                pull(right);
                root = rotate_left(root);
            }else if(comp(key, nodes[static_cast<std::size_t>(right)].key)){
                right = make_mutable(right);
                nodes[static_cast<std::size_t>(root)].right = right;
                nodes[static_cast<std::size_t>(right)].left =
                    splay(nodes[static_cast<std::size_t>(right)].left, key);
                pull(right);
                if(nodes[static_cast<std::size_t>(right)].left != -1){
                    nodes[static_cast<std::size_t>(root)].right = rotate_right(right);
                }
            }
            pull(root);
            if(nodes[static_cast<std::size_t>(root)].right == -1) return root;
            return rotate_left(root);
        }

        return root;
    }

    void check_version_capacity() const{
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (PersistentSplaySet)."
            );
        }
    }

    int register_root(int root){
        roots[static_cast<std::size_t>(version_count)] = root;
        return version_count++;
    }

    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentSplaySet).");
        }
    }

public:
    explicit PersistentSplaySet(const Compare& compare = Compare()) : comp(compare){
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
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            if(root == -1) return register_root(new_node(key));
            root = splay(root, key);
            if(equal_key(nodes[static_cast<std::size_t>(root)].key, key)){
                return register_root(root);
            }

            int node;
            if(comp(key, nodes[static_cast<std::size_t>(root)].key)){
                int left = nodes[static_cast<std::size_t>(root)].left;
                nodes[static_cast<std::size_t>(root)].left = -1;
                pull(root);
                node = new_node(key, left, root);
            }else{
                int right = nodes[static_cast<std::size_t>(root)].right;
                nodes[static_cast<std::size_t>(root)].right = -1;
                pull(root);
                node = new_node(key, root, right);
            }
            return register_root(node);
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    int erase(const Key& key, int version = 0){
        check_version(version);
        check_version_capacity();
        int root = roots[static_cast<std::size_t>(version)];
        int checkpoint = used;
        mutation_begin = checkpoint;
        try{
            root = splay(root, key);
            if(root == -1 || !equal_key(nodes[static_cast<std::size_t>(root)].key, key)){
                return register_root(root);
            }

            int left = nodes[static_cast<std::size_t>(root)].left;
            int right = nodes[static_cast<std::size_t>(root)].right;
            if(left == -1) return register_root(right);
            left = splay(left, key);
            nodes[static_cast<std::size_t>(left)].right = right;
            pull(left);
            return register_root(left);
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
