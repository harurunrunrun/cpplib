#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "suffix_array.hpp"

struct SuffixTree{
    struct Node{
        int parent = -1;
        int edge_begin = 0;
        int edge_end = 0;
        int string_depth = 0;
        int suffix_begin = -1;
        int leaf_count = 0;
        std::vector<int> children;
    };

private:
    std::string text_;
    std::vector<Node> nodes_;

    int add_node(Node node){
        nodes_.push_back(std::move(node));
        return static_cast<int>(nodes_.size()) - 1;
    }

    int child_by_symbol(int node, unsigned char symbol) const{
        const auto& children = nodes_[static_cast<std::size_t>(node)].children;
        const auto iterator = std::lower_bound(
            children.begin(), children.end(), symbol,
            [&](int child, unsigned char value){
                return static_cast<unsigned char>(text_[static_cast<std::size_t>(
                    nodes_[static_cast<std::size_t>(child)].edge_begin
                )]) < value;
            }
        );
        if(iterator == children.end()) return -1;
        const int child = *iterator;
        return static_cast<unsigned char>(text_[static_cast<std::size_t>(
            nodes_[static_cast<std::size_t>(child)].edge_begin
        )]) == symbol ? child : -1;
    }

public:
    explicit SuffixTree(std::string text): text_(std::move(text)){
        nodes_.push_back(Node{});
        const int length = static_cast<int>(text_.size());
        if(length == 0) return;
        const std::vector<int> suffixes = suffix_array(text_);
        const std::vector<int> lcp = lcp_array(text_, suffixes);
        std::vector<int> stack{0};
        for(int order = 0; order < length; ++order){
            const int common = order == 0
                ? 0 : lcp[static_cast<std::size_t>(order - 1)];
            while(nodes_[static_cast<std::size_t>(stack.back())].string_depth
                > common){
                stack.pop_back();
            }
            if(nodes_[static_cast<std::size_t>(stack.back())].string_depth
                < common){
                const int parent = stack.back();
                const int old_child =
                    nodes_[static_cast<std::size_t>(parent)].children.back();
                Node internal;
                internal.parent = parent;
                internal.edge_begin =
                    suffixes[static_cast<std::size_t>(order - 1)]
                    + nodes_[static_cast<std::size_t>(parent)].string_depth;
                internal.edge_end =
                    suffixes[static_cast<std::size_t>(order - 1)] + common;
                internal.string_depth = common;
                internal.children.push_back(old_child);
                const int internal_id = add_node(std::move(internal));
                nodes_[static_cast<std::size_t>(old_child)].parent = internal_id;
                nodes_[static_cast<std::size_t>(old_child)].edge_begin +=
                    common
                    - nodes_[static_cast<std::size_t>(parent)].string_depth;
                nodes_[static_cast<std::size_t>(parent)].children.back() =
                    internal_id;
                stack.push_back(internal_id);
            }
            const int parent = stack.back();
            const int suffix = suffixes[static_cast<std::size_t>(order)];
            Node leaf;
            leaf.parent = parent;
            leaf.edge_begin =
                suffix + nodes_[static_cast<std::size_t>(parent)].string_depth;
            leaf.edge_end = length;
            leaf.string_depth = length - suffix;
            leaf.suffix_begin = suffix;
            leaf.leaf_count = 1;
            const int leaf_id = add_node(std::move(leaf));
            nodes_[static_cast<std::size_t>(parent)].children.push_back(leaf_id);
            stack.push_back(leaf_id);
        }
        std::vector<int> order{0};
        for(std::size_t index = 0; index < order.size(); ++index){
            const auto& children =
                nodes_[static_cast<std::size_t>(order[index])].children;
            order.insert(order.end(), children.begin(), children.end());
        }
        for(std::size_t index = order.size(); index-- > 1;){
            const int node = order[index];
            const int parent = nodes_[static_cast<std::size_t>(node)].parent;
            nodes_[static_cast<std::size_t>(parent)].leaf_count +=
                nodes_[static_cast<std::size_t>(node)].leaf_count;
        }
    }

    int node_count() const noexcept{
        return static_cast<int>(nodes_.size());
    }

    const Node& operator[](int node) const{
        if(node < 0 || node_count() <= node)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(SuffixTree::operator[])."
            );
        }
        return nodes_[static_cast<std::size_t>(node)];
    }

    const std::string& text() const noexcept{
        return text_;
    }

    int find_locus(std::string_view pattern) const{
        if(pattern.empty()) return 0;
        int node = 0;
        std::size_t consumed = 0;
        while(consumed < pattern.size()){
            const int child = child_by_symbol(
                node, static_cast<unsigned char>(pattern[consumed])
            );
            if(child == -1) return -1;
            const Node& next = nodes_[static_cast<std::size_t>(child)];
            for(int position = next.edge_begin;
                position < next.edge_end && consumed < pattern.size();
                ++position, ++consumed){
                if(text_[static_cast<std::size_t>(position)]
                    != pattern[consumed]){
                    return -1;
                }
            }
            node = child;
        }
        return node;
    }

    bool contains(std::string_view pattern) const{
        return find_locus(pattern) != -1;
    }

    int count(std::string_view pattern) const{
        if(pattern.empty()) return static_cast<int>(text_.size()) + 1;
        const int locus = find_locus(pattern);
        return locus == -1
            ? 0 : nodes_[static_cast<std::size_t>(locus)].leaf_count;
    }

    std::vector<int> locate(std::string_view pattern) const{
        if(pattern.empty()){
            std::vector<int> result(text_.size() + 1);
            for(std::size_t index = 0; index <= text_.size(); ++index){
                result[index] = static_cast<int>(index);
            }
            return result;
        }
        const int locus = find_locus(pattern);
        if(locus == -1) return {};
        std::vector<int> result;
        result.reserve(static_cast<std::size_t>(
            nodes_[static_cast<std::size_t>(locus)].leaf_count
        ));
        std::vector<int> stack{locus};
        while(!stack.empty()){
            const int node = stack.back();
            stack.pop_back();
            const Node& current = nodes_[static_cast<std::size_t>(node)];
            if(current.suffix_begin != -1){
                result.push_back(current.suffix_begin);
            }
                stack.insert(
                    stack.end(), current.children.begin(), current.children.end()
                );
        }
        std::sort(result.begin(), result.end());
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_TREE_HPP_INCLUDED
