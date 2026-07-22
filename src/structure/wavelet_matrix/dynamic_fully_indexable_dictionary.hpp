#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

template<int MAX_SIZE>
struct DynamicFullyIndexableDictionary{
    static_assert(MAX_SIZE >= 0);

private:
    struct Node{
        bool value;
        std::uint64_t priority;
        Node* left = nullptr;
        Node* right = nullptr;
        int size = 1;
        int ones;

        Node(bool bit, std::uint64_t priority_value):
            value(bit), priority(priority_value), ones(bit ? 1 : 0){}
    };

    Node* root = nullptr;
    std::uint64_t random_state = 0x243f6a8885a308d3ULL;

    static int node_size(const Node* node){ return node == nullptr ? 0 : node->size; }
    static int node_ones(const Node* node){ return node == nullptr ? 0 : node->ones; }

    static void pull(Node* node){
        if(node == nullptr) return;
        node->size = node_size(node->left) + 1 + node_size(node->right);
        node->ones = node_ones(node->left) + (node->value ? 1 : 0) +
            node_ones(node->right);
    }

    std::uint64_t next_priority(){
        random_state += 0x9e3779b97f4a7c15ULL;
        std::uint64_t value = random_state;
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }

    static std::pair<Node*, Node*> split(Node* node, int left_size){
        if(node == nullptr) return {nullptr, nullptr};
        if(node_size(node->left) >= left_size){
            auto [left, middle] = split(node->left, left_size);
            node->left = middle;
            pull(node);
            return {left, node};
        }
        auto [middle, right] = split(
            node->right, left_size - node_size(node->left) - 1
        );
        node->right = middle;
        pull(node);
        return {node, right};
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

    static void destroy(Node* node){
        if(node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    static Node* clone(const Node* node){
        if(node == nullptr) return nullptr;
        Node* result = new Node(node->value, node->priority);
        result->left = clone(node->left);
        result->right = clone(node->right);
        pull(result);
        return result;
    }

    static void pull_all(Node* node){
        if(node == nullptr) return;
        pull_all(node->left);
        pull_all(node->right);
        pull(node);
    }

    void build(const std::vector<bool>& values){
        std::vector<Node*> stack;
        stack.reserve(values.size());
        for(bool value: values){
            Node* current = new Node(value, next_priority());
            Node* last = nullptr;
            while(!stack.empty() && stack.back()->priority < current->priority){
                last = stack.back();
                stack.pop_back();
            }
            current->left = last;
            if(stack.empty()) root = current;
            else stack.back()->right = current;
            stack.push_back(current);
        }
        pull_all(root);
    }

    static void set_recursive(Node* node, int position, bool value){
        const int left_size = node_size(node->left);
        if(position < left_size){
            set_recursive(node->left, position, value);
        }else if(position == left_size){
            node->value = value;
        }else{
            set_recursive(node->right, position - left_size - 1, value);
        }
        pull(node);
    }

    void check_index(int k, const char* message) const{
        if(k < 0 || size() <= k)[[unlikely]] throw std::runtime_error(message);
    }

public:
    explicit DynamicFullyIndexableDictionary(int n = 0){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        build(std::vector<bool>(static_cast<std::size_t>(n), false));
    }

    explicit DynamicFullyIndexableDictionary(const std::vector<bool>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        build(values);
    }

    DynamicFullyIndexableDictionary(const DynamicFullyIndexableDictionary& other):
        root(clone(other.root)), random_state(other.random_state){}

    DynamicFullyIndexableDictionary& operator=(
        const DynamicFullyIndexableDictionary& other
    ){
        if(this == &other) return *this;
        Node* copied = clone(other.root);
        destroy(root);
        root = copied;
        random_state = other.random_state;
        return *this;
    }

    DynamicFullyIndexableDictionary(DynamicFullyIndexableDictionary&& other) noexcept:
        root(other.root), random_state(other.random_state){
        other.root = nullptr;
    }

    DynamicFullyIndexableDictionary& operator=(
        DynamicFullyIndexableDictionary&& other
    ) noexcept{
        if(this == &other) return *this;
        destroy(root);
        root = other.root;
        random_state = other.random_state;
        other.root = nullptr;
        return *this;
    }

    ~DynamicFullyIndexableDictionary(){ destroy(root); }

    int size() const{ return node_size(root); }

    bool operator[](int k) const{
        check_index(k, "library assertion fault: range violation (operator[]).");
        const Node* node = root;
        while(node != nullptr){
            const int left_size = node_size(node->left);
            if(k < left_size) node = node->left;
            else if(k == left_size) return node->value;
            else{
                k -= left_size + 1;
                node = node->right;
            }
        }
        throw std::logic_error("DynamicFullyIndexableDictionary access failure.");
    }

    void set(int k, bool value){
        check_index(k, "library assertion fault: range violation (set).");
        set_recursive(root, k, value);
    }

    void flip(int k){
        check_index(k, "library assertion fault: range violation (flip).");
        set_recursive(root, k, !(*this)[k]);
    }

    void insert(int position, bool value){
        if(position < 0 || size() < position || size() == MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (insert)."
            );
        }
        auto [left, right] = split(root, position);
        root = merge(merge(left, new Node(value, next_priority())), right);
    }

    bool erase(int position){
        check_index(position, "library assertion fault: range violation (erase).");
        auto [left, suffix] = split(root, position);
        auto [middle, right] = split(suffix, 1);
        if(middle == nullptr){
            throw std::logic_error("DynamicFullyIndexableDictionary erase failure.");
        }
        const bool result = middle->value;
        middle->left = nullptr;
        middle->right = nullptr;
        delete middle;
        root = merge(left, right);
        return result;
    }

    void push_back(bool value){ insert(size(), value); }

    bool pop_back(){
        if(size() == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (pop_back)."
            );
        }
        return erase(size() - 1);
    }

    int rank(bool value, int r) const{
        if(r < 0 || size() < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rank)."
            );
        }
        const int length = r;
        int ones = 0;
        const Node* node = root;
        while(node != nullptr && r > 0){
            const int left_size = node_size(node->left);
            if(r <= left_size){
                node = node->left;
                continue;
            }
            ones += node_ones(node->left) + (node->value ? 1 : 0);
            r -= left_size + 1;
            node = node->right;
        }
        return value ? ones : length - ones;
    }

    int rank(bool value, int l, int r) const{
        if(l < 0 || r < l || size() < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rank)."
            );
        }
        return rank(value, r) - rank(value, l);
    }

    int select(bool value, int k) const{
        if(k < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (select)."
            );
        }
        const int total = value ? node_ones(root) : size() - node_ones(root);
        if(total <= k) return size();
        const Node* node = root;
        int offset = 0;
        while(node != nullptr){
            const int left_count = value
                ? node_ones(node->left)
                : node_size(node->left) - node_ones(node->left);
            if(k < left_count){
                node = node->left;
                continue;
            }
            k -= left_count;
            const int left_size = node_size(node->left);
            if(node->value == value){
                if(k == 0) return offset + left_size;
                k--;
            }
            offset += left_size + 1;
            node = node->right;
        }
        return size();
    }
};
