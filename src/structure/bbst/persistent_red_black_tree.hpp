#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
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
        std::uint64_t priority = 0;
        Key key{};
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

    void pull(int node){
        nodes[static_cast<std::size_t>(node)].size =
            1 + size_or_zero(nodes[static_cast<std::size_t>(node)].left) +
            size_or_zero(nodes[static_cast<std::size_t>(node)].right);
    }

    int copy_node(int node){
        if(node == -1) return -1;
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentRedBlackSet).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
    }

    int new_node(const Key& key){
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentRedBlackSet).");
        }
        int node = used++;
        nodes[static_cast<std::size_t>(node)] = {-1, -1, 1, splitmix64(static_cast<std::uint64_t>(node) + 1), key};
        return node;
    }

    std::pair<int, int> split_less(int root, const Key& key){
        if(root == -1) return {-1, -1};
        int node = copy_node(root);
        if(comp(nodes[static_cast<std::size_t>(node)].key, key)){
            auto [left, right] = split_less(nodes[static_cast<std::size_t>(node)].right, key);
            nodes[static_cast<std::size_t>(node)].right = left;
            pull(node);
            return {node, right};
        }else{
            auto [left, right] = split_less(nodes[static_cast<std::size_t>(node)].left, key);
            nodes[static_cast<std::size_t>(node)].left = right;
            pull(node);
            return {left, node};
        }
    }

    int merge(int left, int right){
        if(left == -1) return right;
        if(right == -1) return left;
        if(nodes[static_cast<std::size_t>(left)].priority < nodes[static_cast<std::size_t>(right)].priority){
            int node = copy_node(left);
            nodes[static_cast<std::size_t>(node)].right = merge(nodes[static_cast<std::size_t>(node)].right, right);
            pull(node);
            return node;
        }else{
            int node = copy_node(right);
            nodes[static_cast<std::size_t>(node)].left = merge(left, nodes[static_cast<std::size_t>(node)].left);
            pull(node);
            return node;
        }
    }

    int erase_rec(int root, const Key& key, bool& erased){
        if(root == -1) return -1;
        int node = copy_node(root);
        if(comp(key, nodes[static_cast<std::size_t>(node)].key)){
            nodes[static_cast<std::size_t>(node)].left = erase_rec(nodes[static_cast<std::size_t>(node)].left, key, erased);
            pull(node);
            return node;
        }
        if(comp(nodes[static_cast<std::size_t>(node)].key, key)){
            nodes[static_cast<std::size_t>(node)].right = erase_rec(nodes[static_cast<std::size_t>(node)].right, key, erased);
            pull(node);
            return node;
        }
        erased = true;
        return merge(nodes[static_cast<std::size_t>(node)].left, nodes[static_cast<std::size_t>(node)].right);
    }
    int register_root(int root){
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentRedBlackSet).");
        }
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
        int root = roots[static_cast<std::size_t>(version)];
        if(contains(key, version)) return register_root(root);
        auto [left, right] = split_less(root, key);
        int node = new_node(key);
        return register_root(merge(merge(left, node), right));
    }

    int erase(const Key& key, int version = 0){
        check_version(version);
        int root = roots[static_cast<std::size_t>(version)];
        if(!contains(key, version)) return register_root(root);
        bool erased = false;
        return register_root(erase_rec(root, key, erased));
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
