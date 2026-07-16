#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Weight>
class RangeEdgeGraph{
public:
    struct Edge{
        int to;
        Weight weight;
    };

    using weight_type = Weight;
    using edge_type = Edge;
    using graph_type = std::vector<std::vector<edge_type>>;

private:
    int original_size_;
    std::size_t segment_size_ = 1;
    graph_type graph_;
    std::size_t edge_count_ = 0;
    weight_type zero_;

    std::size_t tree_node_count() const noexcept{
        return segment_size_ * 2 - 1;
    }

    int out_node(std::size_t tree_index) const noexcept{
        return static_cast<int>(
            static_cast<std::size_t>(original_size_) + tree_index - 1
        );
    }

    int in_node(std::size_t tree_index) const noexcept{
        return static_cast<int>(
            static_cast<std::size_t>(original_size_)
            + tree_node_count()
            + tree_index - 1
        );
    }

    void validate_vertex(int vertex) const{
        if(vertex < 0 || vertex >= original_size_)[[unlikely]]{
            throw std::out_of_range("RangeEdgeGraph: vertex is out of range");
        }
    }

    void validate_range(int left, int right) const{
        if(left > right)[[unlikely]]{
            throw std::invalid_argument(
                "RangeEdgeGraph: left endpoint exceeds right endpoint"
            );
        }
        if(left < 0 || right > original_size_)[[unlikely]]{
            throw std::out_of_range("RangeEdgeGraph: range is out of bounds");
        }
    }

    void append_edge(int from, int to, const weight_type& weight){
        graph_[static_cast<std::size_t>(from)].push_back({to, weight});
        ++edge_count_;
    }

    template<class Function>
    void for_each_cover(int left, int right, Function&& function) const{
        std::size_t l = segment_size_ + static_cast<std::size_t>(left);
        std::size_t r = segment_size_ + static_cast<std::size_t>(right);
        while(l < r){
            if((l & 1U) != 0U) function(l++);
            if((r & 1U) != 0U) function(--r);
            l >>= 1;
            r >>= 1;
        }
    }

public:
    explicit RangeEdgeGraph(
        int original_size,
        weight_type zero = weight_type()
    )
        : original_size_(original_size), zero_(std::move(zero)){
        if(original_size_ <= 0)[[unlikely]]{
            throw std::invalid_argument(
                "RangeEdgeGraph: original_size must be positive"
            );
        }

        while(segment_size_ < static_cast<std::size_t>(original_size_)){
            segment_size_ <<= 1;
        }
        const std::size_t total_vertices =
            static_cast<std::size_t>(original_size_)
            + 2 * tree_node_count();
        if(total_vertices
           > static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::length_error(
                "RangeEdgeGraph: auxiliary vertex count exceeds int"
            );
        }
        graph_.resize(total_vertices);

        for(std::size_t node = 1; node < segment_size_; ++node){
            append_edge(out_node(node), out_node(node * 2), zero_);
            append_edge(out_node(node), out_node(node * 2 + 1), zero_);
            append_edge(in_node(node * 2), in_node(node), zero_);
            append_edge(in_node(node * 2 + 1), in_node(node), zero_);
        }
        for(int vertex = 0; vertex < original_size_; ++vertex){
            const std::size_t leaf =
                segment_size_ + static_cast<std::size_t>(vertex);
            append_edge(out_node(leaf), vertex, zero_);
            append_edge(vertex, in_node(leaf), zero_);
        }
    }

    int original_size() const noexcept{
        return original_size_;
    }

    int vertex_count() const noexcept{
        return static_cast<int>(graph_.size());
    }

    std::size_t edge_count() const noexcept{
        return edge_count_;
    }

    void add_edge(int from, int to, const weight_type& weight){
        validate_vertex(from);
        validate_vertex(to);
        append_edge(from, to, weight);
    }

    void add_point_to_range(
        int from,
        int left,
        int right,
        const weight_type& weight
    ){
        validate_vertex(from);
        validate_range(left, right);
        for_each_cover(left, right, [&](std::size_t node){
            append_edge(from, out_node(node), weight);
        });
    }

    void add_range_to_point(
        int left,
        int right,
        int to,
        const weight_type& weight
    ){
        validate_range(left, right);
        validate_vertex(to);
        for_each_cover(left, right, [&](std::size_t node){
            append_edge(in_node(node), to, weight);
        });
    }

    void add_range_to_range(
        int source_left,
        int source_right,
        int destination_left,
        int destination_right,
        const weight_type& weight
    ){
        validate_range(source_left, source_right);
        validate_range(destination_left, destination_right);
        if(source_left == source_right
           || destination_left == destination_right){
            return;
        }
        if(graph_.size()
           == static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::length_error(
                "RangeEdgeGraph: auxiliary vertex count exceeds int"
            );
        }
        const int hub = static_cast<int>(graph_.size());
        graph_.emplace_back();

        for_each_cover(source_left, source_right, [&](std::size_t node){
            append_edge(in_node(node), hub, zero_);
        });
        for_each_cover(
            destination_left,
            destination_right,
            [&](std::size_t node){
                append_edge(hub, out_node(node), weight);
            }
        );
    }

    const graph_type& view() const noexcept{
        return graph_;
    }

    graph_type build() const{
        return graph_;
    }
};
