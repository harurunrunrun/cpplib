#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_PERSISTENT_MEX_MULTISET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_PERSISTENT_MEX_MULTISET_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

class PersistentMexMultiset{
    struct Node{
        int left = -1;
        int right = -1;
        std::size_t total = 0;
        std::size_t distinct = 0;
    };

    std::size_t universe_;
    std::size_t leaf_count_ = 1;
    std::vector<Node> nodes_;
    std::vector<int> roots_;

    int build(std::size_t left, std::size_t right){
        const int node = static_cast<int>(nodes_.size());
        nodes_.push_back(Node{});
        if(right - left == 1){
            if(left >= universe_) nodes_[static_cast<std::size_t>(node)].distinct = 1;
        }else{
            const std::size_t middle = left + (right - left) / 2;
            const int left_child = build(left, middle);
            const int right_child = build(middle, right);
            nodes_[static_cast<std::size_t>(node)].left = left_child;
            nodes_[static_cast<std::size_t>(node)].right = right_child;
            nodes_[static_cast<std::size_t>(node)].distinct =
                nodes_[static_cast<std::size_t>(left_child)].distinct +
                nodes_[static_cast<std::size_t>(right_child)].distinct;
        }
        return node;
    }

    std::size_t count_at(
        int node,
        std::size_t left,
        std::size_t right,
        std::size_t position
    ) const{
        if(right - left == 1) return nodes_[static_cast<std::size_t>(node)].total;
        const std::size_t middle = left + (right - left) / 2;
        if(position < middle){
            return count_at(nodes_[static_cast<std::size_t>(node)].left, left, middle, position);
        }
        return count_at(nodes_[static_cast<std::size_t>(node)].right, middle, right, position);
    }

    int add_at(
        int source,
        std::size_t left,
        std::size_t right,
        std::size_t position,
        int difference
    ){
        const int node = static_cast<int>(nodes_.size());
        nodes_.push_back(nodes_[static_cast<std::size_t>(source)]);
        if(right - left == 1){
            Node& leaf = nodes_[static_cast<std::size_t>(node)];
            if(difference > 0) ++leaf.total;
            else --leaf.total;
            leaf.distinct = leaf.total == 0 ? 0 : 1;
            return node;
        }
        const std::size_t middle = left + (right - left) / 2;
        if(position < middle){
            const int child = add_at(
                nodes_[static_cast<std::size_t>(node)].left,
                left,
                middle,
                position,
                difference
            );
            nodes_[static_cast<std::size_t>(node)].left = child;
        }else{
            const int child = add_at(
                nodes_[static_cast<std::size_t>(node)].right,
                middle,
                right,
                position,
                difference
            );
            nodes_[static_cast<std::size_t>(node)].right = child;
        }
        const int left_child = nodes_[static_cast<std::size_t>(node)].left;
        const int right_child = nodes_[static_cast<std::size_t>(node)].right;
        nodes_[static_cast<std::size_t>(node)].total =
            nodes_[static_cast<std::size_t>(left_child)].total +
            nodes_[static_cast<std::size_t>(right_child)].total;
        nodes_[static_cast<std::size_t>(node)].distinct =
            nodes_[static_cast<std::size_t>(left_child)].distinct +
            nodes_[static_cast<std::size_t>(right_child)].distinct;
        return node;
    }

    void validate_version(std::size_t version) const{
        if(version >= roots_.size()) throw std::out_of_range("PersistentMexMultiset: version is out of range");
    }

    void validate_value(std::size_t value) const{
        if(value >= universe_) throw std::out_of_range("PersistentMexMultiset: value is outside the universe");
    }

public:
    explicit PersistentMexMultiset(std::size_t universe): universe_(universe){
        while(leaf_count_ < universe_) leaf_count_ <<= 1;
        roots_.push_back(build(0, leaf_count_));
    }

    std::size_t universe() const noexcept{ return universe_; }
    std::size_t version_count() const noexcept{ return roots_.size(); }
    std::size_t node_count() const noexcept{ return nodes_.size(); }

    std::size_t count(std::size_t version, std::size_t value) const{
        validate_version(version);
        validate_value(value);
        return count_at(roots_[version], 0, leaf_count_, value);
    }

    std::size_t insert(std::size_t version, std::size_t value){
        validate_version(version);
        validate_value(value);
        roots_.push_back(add_at(roots_[version], 0, leaf_count_, value, 1));
        return roots_.size() - 1;
    }

    std::size_t erase(std::size_t version, std::size_t value){
        validate_version(version);
        validate_value(value);
        if(count_at(roots_[version], 0, leaf_count_, value) == 0){
            roots_.push_back(roots_[version]);
        }else{
            roots_.push_back(add_at(roots_[version], 0, leaf_count_, value, -1));
        }
        return roots_.size() - 1;
    }

    std::size_t distinct_size(std::size_t version) const{
        validate_version(version);
        return nodes_[static_cast<std::size_t>(roots_[version])].distinct -
            (leaf_count_ - universe_);
    }

    std::size_t mex(std::size_t version) const{
        validate_version(version);
        if(distinct_size(version) == universe_) return universe_;
        int node = roots_[version];
        std::size_t left = 0;
        std::size_t right = leaf_count_;
        while(right - left > 1){
            const std::size_t middle = left + (right - left) / 2;
            const int left_child = nodes_[static_cast<std::size_t>(node)].left;
            if(nodes_[static_cast<std::size_t>(left_child)].distinct < middle - left){
                node = left_child;
                right = middle;
            }else{
                node = nodes_[static_cast<std::size_t>(node)].right;
                left = middle;
            }
        }
        return left;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_PERSISTENT_MEX_MULTISET_HPP_INCLUDED
