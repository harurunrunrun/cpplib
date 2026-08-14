#ifndef CPPLIB_SRC_STRUCTURE_INTERVAL_INTERVAL_MEX_SET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_INTERVAL_INTERVAL_MEX_SET_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

class IntervalMexSet{
    struct Node{
        int left = -1;
        int right = -1;
        std::uint64_t present = 0;
        unsigned char lazy = 0;  // 0: none, 1: clear, 2: fill, 3: flip
    };

    std::uint64_t universe_;
    std::vector<Node> nodes_{Node{}};

    int make_node(){
        nodes_.push_back(Node{});
        return static_cast<int>(nodes_.size() - 1);
    }

    void apply(int node, std::uint64_t length, unsigned char operation){
        Node& current = nodes_[static_cast<std::size_t>(node)];
        if(operation == 1){
            current.present = 0;
            current.lazy = 1;
        }else if(operation == 2){
            current.present = length;
            current.lazy = 2;
        }else{
            current.present = length - current.present;
            if(current.lazy == 0) current.lazy = 3;
            else if(current.lazy == 1) current.lazy = 2;
            else if(current.lazy == 2) current.lazy = 1;
            else current.lazy = 0;
        }
    }

    void push(int node, std::uint64_t left, std::uint64_t right){
        if(right - left <= 1) return;
        int left_child = nodes_[static_cast<std::size_t>(node)].left;
        int right_child = nodes_[static_cast<std::size_t>(node)].right;
        if(left_child < 0) left_child = make_node();
        if(right_child < 0) right_child = make_node();
        nodes_[static_cast<std::size_t>(node)].left = left_child;
        nodes_[static_cast<std::size_t>(node)].right = right_child;
        const unsigned char lazy = nodes_[static_cast<std::size_t>(node)].lazy;
        if(lazy != 0){
            const std::uint64_t middle = left + (right - left) / 2;
            apply(left_child, middle - left, lazy);
            apply(right_child, right - middle, lazy);
            nodes_[static_cast<std::size_t>(node)].lazy = 0;
        }
    }

    void update(
        int node,
        std::uint64_t left,
        std::uint64_t right,
        std::uint64_t query_left,
        std::uint64_t query_right,
        unsigned char operation
    ){
        if(query_left <= left && right <= query_right){
            apply(node, right - left, operation);
            return;
        }
        push(node, left, right);
        const std::uint64_t middle = left + (right - left) / 2;
        const int left_child = nodes_[static_cast<std::size_t>(node)].left;
        const int right_child = nodes_[static_cast<std::size_t>(node)].right;
        if(query_left < middle){
            update(left_child, left, middle, query_left, query_right, operation);
        }
        if(middle < query_right){
            update(right_child, middle, right, query_left, query_right, operation);
        }
        nodes_[static_cast<std::size_t>(node)].present =
            nodes_[static_cast<std::size_t>(left_child)].present +
            nodes_[static_cast<std::size_t>(right_child)].present;
    }

    void validate_range(std::uint64_t left, std::uint64_t right) const{
        if(left > right || right > universe_){
            throw std::out_of_range("IntervalMexSet: invalid half-open range");
        }
    }

    std::uint64_t first_missing(
        int node,
        std::uint64_t left,
        std::uint64_t right
    ){
        if(right - left == 1) return left;
        push(node, left, right);
        const std::uint64_t middle = left + (right - left) / 2;
        const int left_child = nodes_[static_cast<std::size_t>(node)].left;
        if(nodes_[static_cast<std::size_t>(left_child)].present < middle - left){
            return first_missing(left_child, left, middle);
        }
        return first_missing(nodes_[static_cast<std::size_t>(node)].right, middle, right);
    }

public:
    explicit IntervalMexSet(std::uint64_t universe): universe_(universe){
        if(universe_ == 0) throw std::invalid_argument("IntervalMexSet: universe must be positive");
    }

    std::uint64_t universe() const noexcept{ return universe_; }
    std::uint64_t distinct_size() const noexcept{ return nodes_[0].present; }
    std::size_t node_count() const noexcept{ return nodes_.size(); }

    void assign(std::uint64_t left, std::uint64_t right, bool present){
        validate_range(left, right);
        if(left == right) return;
        update(0, 0, universe_, left, right, present ? 2 : 1);
    }

    void flip(std::uint64_t left, std::uint64_t right){
        validate_range(left, right);
        if(left == right) return;
        update(0, 0, universe_, left, right, 3);
    }

    std::uint64_t mex(){
        if(nodes_[0].present == universe_) return universe_;
        return first_missing(0, 0, universe_);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_INTERVAL_INTERVAL_MEX_SET_HPP_INCLUDED
