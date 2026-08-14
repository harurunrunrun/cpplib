#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_STATIC_RANGE_MEX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_STATIC_RANGE_MEX_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

class StaticRangeMex{
    struct Node{
        int left = -1;
        int right = -1;
        int minimum_last = -1;
    };

    std::size_t size_ = 0;
    std::size_t universe_ = 1;
    std::vector<Node> nodes_;
    std::vector<int> roots_;

    int build(std::size_t left, std::size_t right){
        const int node = static_cast<int>(nodes_.size());
        nodes_.push_back(Node{});
        if(right - left > 1){
            const std::size_t middle = left + (right - left) / 2;
            const int left_child = build(left, middle);
            const int right_child = build(middle, right);
            nodes_[static_cast<std::size_t>(node)].left = left_child;
            nodes_[static_cast<std::size_t>(node)].right = right_child;
        }
        return node;
    }

    int set_last(
        int source,
        std::size_t left,
        std::size_t right,
        std::size_t position,
        int last
    ){
        const int node = static_cast<int>(nodes_.size());
        nodes_.push_back(nodes_[static_cast<std::size_t>(source)]);
        if(right - left == 1){
            nodes_[static_cast<std::size_t>(node)].minimum_last = last;
            return node;
        }
        const std::size_t middle = left + (right - left) / 2;
        if(position < middle){
            nodes_[static_cast<std::size_t>(node)].left = set_last(
                nodes_[static_cast<std::size_t>(node)].left,
                left,
                middle,
                position,
                last
            );
        }else{
            nodes_[static_cast<std::size_t>(node)].right = set_last(
                nodes_[static_cast<std::size_t>(node)].right,
                middle,
                right,
                position,
                last
            );
        }
        nodes_[static_cast<std::size_t>(node)].minimum_last = std::min(
            nodes_[static_cast<std::size_t>(nodes_[static_cast<std::size_t>(node)].left)].minimum_last,
            nodes_[static_cast<std::size_t>(nodes_[static_cast<std::size_t>(node)].right)].minimum_last
        );
        return node;
    }

    std::size_t first_last_before(
        int node,
        std::size_t left,
        std::size_t right,
        int boundary
    ) const{
        if(right - left == 1) return left;
        const Node& current = nodes_[static_cast<std::size_t>(node)];
        const std::size_t middle = left + (right - left) / 2;
        if(nodes_[static_cast<std::size_t>(current.left)].minimum_last < boundary){
            return first_last_before(current.left, left, middle, boundary);
        }
        return first_last_before(current.right, middle, right, boundary);
    }

public:
    explicit StaticRangeMex(const std::vector<std::int64_t>& values):
        size_(values.size()), universe_(values.size() + 1){
        nodes_.reserve(2 * universe_ + size_ * 24);
        roots_.reserve(size_ + 1);
        roots_.push_back(build(0, universe_));
        for(std::size_t index = 0; index < size_; index++){
            int root = roots_.back();
            const std::int64_t value = values[index];
            if(value >= 0 && static_cast<std::uint64_t>(value) < universe_){
                root = set_last(
                    root,
                    0,
                    universe_,
                    static_cast<std::size_t>(value),
                    static_cast<int>(index)
                );
            }
            roots_.push_back(root);
        }
    }

    std::size_t size() const noexcept{ return size_; }
    std::size_t node_count() const noexcept{ return nodes_.size(); }

    std::size_t range_mex(std::size_t left, std::size_t right) const{
        if(left > right || right > size_){
            throw std::out_of_range("StaticRangeMex::range_mex: invalid half-open range");
        }
        return first_last_before(roots_[right], 0, universe_, static_cast<int>(left));
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_STATIC_RANGE_MEX_HPP_INCLUDED
