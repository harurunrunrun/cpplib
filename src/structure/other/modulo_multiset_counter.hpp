#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

// Maintains integer-key counts under a global x <- x mod m operation.
// The optional dense initial part [1, initial_maximum] is represented by
// differences of prefix-constant functions, rather than by individual keys.
struct ModuloMultisetCounter{
    using Key = std::uint64_t;
    using Count = long long;
    using WideCount = __int128_t;

private:
    struct Node{
        Key key = 0;
        WideCount coefficient = 0;
        WideCount subtree_sum = 0;
        std::uint64_t priority = 0;
        std::size_t subtree_size = 1;
        Node* left = nullptr;
        Node* right = nullptr;
    };

    Node* root = nullptr;
    std::map<Key, WideCount> explicit_counts;
    std::uint64_t random_state = 0x6a09e667f3bcc909ULL;
    std::vector<std::unique_ptr<Node>> storage;
    std::vector<Node*> free_nodes;

    static Count narrow(WideCount value){
        if(value < static_cast<WideCount>(std::numeric_limits<Count>::min()) ||
           value > static_cast<WideCount>(std::numeric_limits<Count>::max())){
            throw std::overflow_error(
                "ModuloMultisetCounter count does not fit long long"
            );
        }
        return static_cast<Count>(value);
    }

    static WideCount subtree_sum(const Node* node){
        return node == nullptr ? WideCount{0} : node->subtree_sum;
    }

    static std::size_t node_size(const Node* node){
        return node == nullptr ? 0 : node->subtree_size;
    }

    static void pull(Node* node){
        if(node == nullptr) return;
        node->subtree_sum = subtree_sum(node->left)
            + node->coefficient
            + subtree_sum(node->right);
        node->subtree_size = 1 + node_size(node->left) + node_size(node->right);
    }

    std::uint64_t next_random(){
        std::uint64_t value = (random_state += 0x9e3779b97f4a7c15ULL);
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }

    Node* make_node(Key key, WideCount coefficient){
        Node* node = nullptr;
        if(free_nodes.empty()){
            storage.push_back(std::make_unique<Node>());
            node = storage.back().get();
        }else{
            node = free_nodes.back();
            free_nodes.pop_back();
        }
        *node = Node{key, coefficient, coefficient,
                     next_random(), 1, nullptr, nullptr};
        return node;
    }

    void recycle(Node* node){
        if(node == nullptr) return;
        node->left = nullptr;
        node->right = nullptr;
        free_nodes.push_back(node);
    }

    static Node* merge(Node* left, Node* right){
        if(left == nullptr) return right;
        if(right == nullptr) return left;
        if(left->priority > right->priority){
            left->right = merge(left->right, right);
            pull(left);
            return left;
        }
        right->left = merge(left, right->left);
        pull(right);
        return right;
    }

    static Node* rotate_right(Node* node){
        Node* top = node->left;
        node->left = top->right;
        top->right = node;
        pull(node);
        pull(top);
        return top;
    }

    static Node* rotate_left(Node* node){
        Node* top = node->right;
        node->right = top->left;
        top->left = node;
        pull(node);
        pull(top);
        return top;
    }

    Node* add_coefficient(Node* node, Key key, WideCount delta){
        if(delta == 0) return node;
        if(node == nullptr) return make_node(key, delta);
        if(key == node->key){
            const WideCount next = node->coefficient + delta;
            if(next == 0){
                Node* result = merge(node->left, node->right);
                recycle(node);
                return result;
            }
            node->coefficient = next;
            pull(node);
            return node;
        }
        if(key < node->key){
            node->left = add_coefficient(node->left, key, delta);
            if(node->left != nullptr && node->left->priority > node->priority){
                node = rotate_right(node);
            }
        }else{
            node->right = add_coefficient(node->right, key, delta);
            if(node->right != nullptr && node->right->priority > node->priority){
                node = rotate_left(node);
            }
        }
        pull(node);
        return node;
    }

    static void split_less(
        Node* node,
        Key border,
        Node*& less,
        Node*& greater_equal
    ){
        if(node == nullptr){
            less = nullptr;
            greater_equal = nullptr;
        }else if(node->key < border){
            less = node;
            split_less(node->right, border, less->right, greater_equal);
            pull(less);
        }else{
            greater_equal = node;
            split_less(node->left, border, less, greater_equal->left);
            pull(greater_equal);
        }
    }

    static WideCount suffix_sum(const Node* node, Key lower_bound){
        WideCount result = 0;
        while(node != nullptr){
            if(node->key >= lower_bound){
                result += subtree_sum(node->right)
                    + node->coefficient;
                node = node->left;
            }else{
                node = node->right;
            }
        }
        return result;
    }

    void collect_modulo_transform(
        Node* node,
        Key modulus,
        std::vector<std::pair<Key, WideCount>>& updates,
        WideCount& uniform_coefficient
    ){
        if(node == nullptr) return;
        collect_modulo_transform(
            node->left, modulus, updates, uniform_coefficient
        );
        collect_modulo_transform(
            node->right, modulus, updates, uniform_coefficient
        );

        const unsigned __int128 length =
            static_cast<unsigned __int128>(node->key) + 1;
        const unsigned __int128 quotient = length / modulus;
        const unsigned __int128 remainder = length % modulus;
        const WideCount coefficient = node->coefficient;
        uniform_coefficient += coefficient * static_cast<WideCount>(quotient);
        if(remainder != 0){
            updates.emplace_back(
                static_cast<Key>(remainder - 1), coefficient
            );
        }
        recycle(node);
    }

    void transform_dense_part(Key modulus){
        Node* unchanged = nullptr;
        Node* transformed = nullptr;
        split_less(root, modulus, unchanged, transformed);
        root = unchanged;
        if(transformed == nullptr) return;

        std::vector<std::pair<Key, WideCount>> updates;
        updates.reserve(node_size(transformed) + 1);
        WideCount uniform_coefficient = 0;
        collect_modulo_transform(
            transformed, modulus, updates, uniform_coefficient
        );
        if(uniform_coefficient != 0){
            updates.emplace_back(modulus - 1, uniform_coefficient);
        }
        std::sort(updates.begin(), updates.end(), [](const auto& left, const auto& right){
            return left.first < right.first;
        });
        for(std::size_t first = 0; first < updates.size();){
            std::size_t last = first + 1;
            WideCount delta = updates[first].second;
            while(last < updates.size() && updates[last].first == updates[first].first){
                delta += updates[last].second;
                ++last;
            }
            root = add_coefficient(root, updates[first].first, delta);
            first = last;
        }
    }

    void add_explicit(Key key, WideCount delta){
        if(delta == 0) return;
        auto [iterator, inserted] = explicit_counts.try_emplace(key, delta);
        if(inserted) return;
        const WideCount next = iterator->second + delta;
        if(next == 0) explicit_counts.erase(iterator);
        else iterator->second = next;
    }

public:
    explicit ModuloMultisetCounter(Key initial_maximum = 0){
        if(initial_maximum != 0){
            root = add_coefficient(root, initial_maximum, 1);
            root = add_coefficient(root, 0, -1);
        }
    }

    ModuloMultisetCounter(const ModuloMultisetCounter&) = delete;
    ModuloMultisetCounter& operator=(const ModuloMultisetCounter&) = delete;
    ModuloMultisetCounter(ModuloMultisetCounter&&) = default;
    ModuloMultisetCounter& operator=(ModuloMultisetCounter&&) = default;

    void apply_mod(Key modulus){
        if(modulus == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: zero modulus (ModuloMultisetCounter::apply_mod)."
            );
        }
        transform_dense_part(modulus);
        auto iterator = explicit_counts.lower_bound(modulus);
        while(iterator != explicit_counts.end()){
            const Key value = iterator->first;
            const WideCount count = iterator->second;
            iterator = explicit_counts.erase(iterator);
            add_explicit(value % modulus, count);
        }
    }

    void add(Key value, Count delta){
        add_explicit(value, delta);
    }

    WideCount count_wide(Key value) const{
        WideCount result = suffix_sum(root, value);
        const auto iterator = explicit_counts.find(value);
        if(iterator != explicit_counts.end()) result += iterator->second;
        return result;
    }

    Count count(Key value) const{
        return narrow(count_wide(value));
    }

    std::size_t explicit_key_count() const noexcept{
        return explicit_counts.size();
    }
};

using ModCounter = ModuloMultisetCounter;

