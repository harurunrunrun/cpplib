#ifndef CPPLIB_SRC_STRUCTURE_OTHER_PRIORITY_SUM_STRUCTURE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_PRIORITY_SUM_STRUCTURE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, class Sum>
struct PrioritySumDefaultProjection{
    Sum operator()(const T& value) const{
        return static_cast<Sum>(value);
    }
};

template<
    class T,
    class Sum = T,
    class Compare = std::less<T>,
    class Projection = PrioritySumDefaultProjection<T, Sum>
>
struct PrioritySumStructure{
private:
    struct Node{
        T value;
        Sum weight;
        Sum subtree_sum;
        std::size_t subtree_size = 1;
        int height = 1;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        Node(const T& value_, const Sum& weight_):
            value(value_), weight(weight_), subtree_sum(weight_){}
    };

    std::unique_ptr<Node> root;
    std::size_t selected_count = 0;
    Sum cached_smallest{};
    Sum cached_largest{};
    Compare comp;
    Projection projection;

    static std::size_t node_size(const std::unique_ptr<Node>& node){
        return node ? node->subtree_size : 0;
    }

    static int node_height(const std::unique_ptr<Node>& node){
        return node ? node->height : 0;
    }

    static Sum node_sum(const std::unique_ptr<Node>& node){
        return node ? node->subtree_sum : Sum{};
    }

    static void pull(Node* node){
        node->subtree_size = node_size(node->left) + 1 + node_size(node->right);
        node->height = std::max(node_height(node->left), node_height(node->right)) + 1;
        node->subtree_sum = node_sum(node->left);
        node->subtree_sum += node->weight;
        node->subtree_sum += node_sum(node->right);
    }

    static std::unique_ptr<Node> rotate_left(std::unique_ptr<Node> node){
        std::unique_ptr<Node> top = std::move(node->right);
        node->right = std::move(top->left);
        pull(node.get());
        top->left = std::move(node);
        pull(top.get());
        return top;
    }

    static std::unique_ptr<Node> rotate_right(std::unique_ptr<Node> node){
        std::unique_ptr<Node> top = std::move(node->left);
        node->left = std::move(top->right);
        pull(node.get());
        top->right = std::move(node);
        pull(top.get());
        return top;
    }

    static void rebalance(std::unique_ptr<Node>& node){
        if(!node) return;
        pull(node.get());
        const int balance = node_height(node->left) - node_height(node->right);
        if(balance > 1){
            if(node_height(node->left->left) < node_height(node->left->right)){
                node->left = rotate_left(std::move(node->left));
            }
            node = rotate_right(std::move(node));
        }else if(balance < -1){
            if(node_height(node->right->right) < node_height(node->right->left)){
                node->right = rotate_right(std::move(node->right));
            }
            node = rotate_left(std::move(node));
        }
    }

    void insert_node(std::unique_ptr<Node>& node, const T& value, const Sum& weight){
        if(!node){
            node = std::make_unique<Node>(value, weight);
            return;
        }
        if(comp(value, node->value)) insert_node(node->left, value, weight);
        else insert_node(node->right, value, weight);
        rebalance(node);
    }

    std::unique_ptr<Node> detach_min(std::unique_ptr<Node>& node){
        if(!node->left){
            std::unique_ptr<Node> result = std::move(node);
            node = std::move(result->right);
            result->right.reset();
            return result;
        }
        std::unique_ptr<Node> result = detach_min(node->left);
        rebalance(node);
        return result;
    }

    void erase_node(std::unique_ptr<Node>& node, const T& value){
        if(comp(value, node->value)){
            erase_node(node->left, value);
        }else if(comp(node->value, value)){
            erase_node(node->right, value);
        }else if(!node->left){
            node = std::move(node->right);
            return;
        }else if(!node->right){
            node = std::move(node->left);
            return;
        }else{
            std::unique_ptr<Node> left = std::move(node->left);
            std::unique_ptr<Node> right = std::move(node->right);
            std::unique_ptr<Node> replacement = detach_min(right);
            replacement->left = std::move(left);
            replacement->right = std::move(right);
            node = std::move(replacement);
        }
        rebalance(node);
    }

    Sum prefix_sum(std::size_t count) const{
        if(count == 0) return Sum{};
        if(count == size()) return root->subtree_sum;
        Sum result{};
        const Node* node = root.get();
        while(count != 0){
            const std::size_t left_size = node_size(node->left);
            if(count <= left_size){
                node = node->left.get();
            }else{
                result += node_sum(node->left);
                result += node->weight;
                count -= left_size + 1;
                node = node->right.get();
            }
        }
        return result;
    }

    Sum suffix_sum(std::size_t count) const{
        if(count == 0) return Sum{};
        if(count == size()) return root->subtree_sum;
        Sum result{};
        const Node* node = root.get();
        while(count != 0){
            const std::size_t right_size = node_size(node->right);
            if(count <= right_size){
                node = node->right.get();
            }else{
                result += node_sum(node->right);
                result += node->weight;
                count -= right_size + 1;
                node = node->left.get();
            }
        }
        return result;
    }

    void refresh_cache(){
        cached_smallest = prefix_sum(selected_count);
        cached_largest = suffix_sum(selected_count);
    }

    bool equivalent(const T& left, const T& right) const{
        return !comp(left, right) && !comp(right, left);
    }

public:
    explicit PrioritySumStructure(
        const Compare& compare = Compare(),
        const Projection& project = Projection()
    ): comp(compare), projection(project){}

    explicit PrioritySumStructure(
        const std::vector<T>& values,
        std::size_t k,
        const Compare& compare = Compare(),
        const Projection& project = Projection()
    ): selected_count(k), comp(compare), projection(project){
        if(k > values.size())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: k exceeds size (PrioritySumStructure constructor)."
            );
        }
        for(const T& value: values){
            const Sum weight = projection(value);
            insert_node(root, value, weight);
        }
        refresh_cache();
    }

    PrioritySumStructure(const PrioritySumStructure&) = delete;
    PrioritySumStructure& operator=(const PrioritySumStructure&) = delete;
    PrioritySumStructure(PrioritySumStructure&&) = default;
    PrioritySumStructure& operator=(PrioritySumStructure&&) = default;

    std::size_t size() const{ return node_size(root); }
    bool empty() const{ return !root; }
    std::size_t k() const{ return selected_count; }

    bool contains(const T& value) const{
        const Node* node = root.get();
        while(node){
            if(equivalent(value, node->value)) return true;
            node = comp(value, node->value) ? node->left.get() : node->right.get();
        }
        return false;
    }

    void insert(const T& value){
        const Sum weight = projection(value);
        insert_node(root, value, weight);
        refresh_cache();
    }

    void erase(const T& value){
        if(!contains(value))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: missing value (PrioritySumStructure::erase)."
            );
        }
        if(size() == selected_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: erase would make k exceed size "
                "(PrioritySumStructure::erase)."
            );
        }
        erase_node(root, value);
        refresh_cache();
    }

    void set_k(std::size_t k){
        if(k > size())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: k exceeds size (PrioritySumStructure::set_k)."
            );
        }
        const Sum next_smallest = prefix_sum(k);
        const Sum next_largest = suffix_sum(k);
        selected_count = k;
        cached_smallest = next_smallest;
        cached_largest = next_largest;
    }

    const Sum& smallest_sum() const{ return cached_smallest; }
    const Sum& largest_sum() const{ return cached_largest; }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_PRIORITY_SUM_STRUCTURE_HPP_INCLUDED
