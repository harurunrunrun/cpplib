#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

template<std::size_t MAX_SIZE>
class IncrementalTreeCentroid{
    static_assert(MAX_SIZE <= static_cast<std::size_t>(
        std::numeric_limits<int>::max() / 2
    ), "IncrementalTreeCentroid capacity is too large");

    static constexpr std::size_t LOG = []{
        std::size_t result = 1;
        std::size_t value = MAX_SIZE;
        while(value > 1){
            value >>= 1;
            ++result;
        }
        return result;
    }();

    struct Marker{
        int left = -1;
        int right = -1;
        int parent = -1;
        int size = 1;
        std::uint64_t priority = 0;
    };

    struct State{
        std::array<Marker, 2 * MAX_SIZE> markers{};
        std::array<int, MAX_SIZE> enter{};
        std::array<int, MAX_SIZE> leave{};
        std::array<int, MAX_SIZE> tree_parent{};
        std::array<int, MAX_SIZE> depth{};
        std::array<std::array<int, LOG>, MAX_SIZE> ancestor{};
    };

    std::unique_ptr<State> state_ = std::make_unique<State>();
    std::size_t vertex_count_ = 0;
    std::size_t marker_count_ = 0;
    int sequence_root_ = -1;
    int centroid_ = -1;
    std::uint64_t random_state_ = 0x9e3779b97f4a7c15ULL;

    [[nodiscard]] int marker_size(int marker) const noexcept{
        return marker == -1 ? 0 : state_->markers[static_cast<std::size_t>(marker)].size;
    }

    void pull(int marker){
        auto& node = state_->markers[static_cast<std::size_t>(marker)];
        node.size = 1 + marker_size(node.left) + marker_size(node.right);
        if(node.left != -1){
            state_->markers[static_cast<std::size_t>(node.left)].parent = marker;
        }
        if(node.right != -1){
            state_->markers[static_cast<std::size_t>(node.right)].parent = marker;
        }
    }

    [[nodiscard]] bool above(int left, int right) const noexcept{
        const auto& first = state_->markers[static_cast<std::size_t>(left)];
        const auto& second = state_->markers[static_cast<std::size_t>(right)];
        if(first.priority != second.priority) return first.priority < second.priority;
        return left < right;
    }

    int merge(int left, int right){
        if(left == -1){
            if(right != -1) state_->markers[static_cast<std::size_t>(right)].parent = -1;
            return right;
        }
        if(right == -1){
            state_->markers[static_cast<std::size_t>(left)].parent = -1;
            return left;
        }
        if(above(left, right)){
            auto& node = state_->markers[static_cast<std::size_t>(left)];
            node.right = merge(node.right, right);
            pull(left);
            node.parent = -1;
            return left;
        }
        auto& node = state_->markers[static_cast<std::size_t>(right)];
        node.left = merge(left, node.left);
        pull(right);
        node.parent = -1;
        return right;
    }

    std::pair<int, int> split(int root, int left_size){
        if(root == -1) return {-1, -1};
        auto& node = state_->markers[static_cast<std::size_t>(root)];
        const int current_left_size = marker_size(node.left);
        if(left_size <= current_left_size){
            auto [left, middle] = split(node.left, left_size);
            node.left = middle;
            pull(root);
            node.parent = -1;
            if(left != -1) state_->markers[static_cast<std::size_t>(left)].parent = -1;
            return {left, root};
        }
        auto [middle, right] = split(
            node.right, left_size - current_left_size - 1
        );
        node.right = middle;
        pull(root);
        node.parent = -1;
        if(right != -1) state_->markers[static_cast<std::size_t>(right)].parent = -1;
        return {root, right};
    }

    std::uint64_t next_priority() noexcept{
        random_state_ ^= random_state_ << 7;
        random_state_ ^= random_state_ >> 9;
        random_state_ ^= random_state_ << 8;
        return random_state_;
    }

    int make_marker(){
        const int marker = static_cast<int>(marker_count_++);
        state_->markers[static_cast<std::size_t>(marker)] = Marker{};
        state_->markers[static_cast<std::size_t>(marker)].priority = next_priority();
        return marker;
    }

    [[nodiscard]] int rank_of(int marker) const{
        int rank = marker_size(
            state_->markers[static_cast<std::size_t>(marker)].left
        );
        while(state_->markers[static_cast<std::size_t>(marker)].parent != -1){
            const int parent = state_->markers[static_cast<std::size_t>(marker)].parent;
            const auto& parent_node = state_->markers[static_cast<std::size_t>(parent)];
            if(parent_node.right == marker){
                rank += 1 + marker_size(parent_node.left);
            }
            marker = parent;
        }
        return rank;
    }

    void check_vertex(std::size_t vertex) const{
        if(vertex >= vertex_count_){
            throw std::out_of_range("incremental tree vertex");
        }
    }

    [[nodiscard]] int jump_up(int vertex, std::size_t distance) const noexcept{
        std::size_t bit = 0;
        while(distance != 0){
            if(distance & 1U){
                vertex = state_->ancestor[static_cast<std::size_t>(vertex)][bit];
            }
            distance >>= 1;
            ++bit;
        }
        return vertex;
    }

public:
    IncrementalTreeCentroid(){
        state_->enter.fill(-1);
        state_->leave.fill(-1);
        state_->tree_parent.fill(-1);
        for(auto& row: state_->ancestor) row.fill(-1);
    }

    IncrementalTreeCentroid(const IncrementalTreeCentroid&) = delete;
    IncrementalTreeCentroid& operator=(const IncrementalTreeCentroid&) = delete;
    IncrementalTreeCentroid(IncrementalTreeCentroid&&) noexcept = default;
    IncrementalTreeCentroid& operator=(IncrementalTreeCentroid&&) noexcept = default;

    [[nodiscard]] std::size_t size() const noexcept{
        return vertex_count_;
    }

    [[nodiscard]] static constexpr std::size_t capacity() noexcept{
        return MAX_SIZE;
    }

    int add_root(){
        if(vertex_count_ != 0){
            throw std::logic_error("incremental tree already has a root");
        }
        if constexpr(MAX_SIZE == 0){
            throw std::length_error("incremental tree capacity");
        }else{
            const int enter = make_marker();
            const int leave = make_marker();
            sequence_root_ = merge(enter, leave);
            state_->enter[0] = enter;
            state_->leave[0] = leave;
            state_->depth[0] = 0;
            vertex_count_ = 1;
            centroid_ = 0;
            return 0;
        }
    }

    int add_leaf(std::size_t parent){
        check_vertex(parent);
        if(vertex_count_ >= MAX_SIZE){
            throw std::length_error("incremental tree capacity");
        }
        const int vertex = static_cast<int>(vertex_count_);
        const int parent_vertex = static_cast<int>(parent);
        state_->tree_parent[static_cast<std::size_t>(vertex)] = parent_vertex;
        state_->depth[static_cast<std::size_t>(vertex)] =
            state_->depth[parent] + 1;
        state_->ancestor[static_cast<std::size_t>(vertex)][0] = parent_vertex;
        for(std::size_t bit = 1; bit < LOG; ++bit){
            const int previous = state_->ancestor[static_cast<std::size_t>(vertex)][bit - 1];
            state_->ancestor[static_cast<std::size_t>(vertex)][bit] = previous == -1
                ? -1
                : state_->ancestor[static_cast<std::size_t>(previous)][bit - 1];
        }

        const int enter = make_marker();
        const int leave = make_marker();
        state_->enter[static_cast<std::size_t>(vertex)] = enter;
        state_->leave[static_cast<std::size_t>(vertex)] = leave;
        const int position = rank_of(state_->leave[parent]);
        auto [left, right] = split(sequence_root_, position);
        sequence_root_ = merge(merge(left, merge(enter, leave)), right);
        ++vertex_count_;

        int neighbor;
        std::size_t component_size;
        const int centroid_depth = state_->depth[static_cast<std::size_t>(centroid_)];
        const int vertex_depth = state_->depth[static_cast<std::size_t>(vertex)];
        const bool centroid_is_ancestor = centroid_depth <= vertex_depth
            && jump_up(vertex, static_cast<std::size_t>(vertex_depth - centroid_depth))
                == centroid_;
        if(centroid_is_ancestor){
            neighbor = jump_up(
                vertex,
                static_cast<std::size_t>(vertex_depth - centroid_depth - 1)
            );
            component_size = subtree_size(static_cast<std::size_t>(neighbor));
        }else{
            neighbor = state_->tree_parent[static_cast<std::size_t>(centroid_)];
            component_size = vertex_count_ - subtree_size(
                static_cast<std::size_t>(centroid_)
            );
        }
        if(component_size * 2 > vertex_count_) centroid_ = neighbor;
        return vertex;
    }

    [[nodiscard]] int centroid() const noexcept{
        return centroid_;
    }

    [[nodiscard]] int parent(std::size_t vertex) const{
        check_vertex(vertex);
        return state_->tree_parent[vertex];
    }

    [[nodiscard]] int depth(std::size_t vertex) const{
        check_vertex(vertex);
        return state_->depth[vertex];
    }

    [[nodiscard]] int kth_ancestor(std::size_t vertex, std::size_t distance) const{
        check_vertex(vertex);
        if(distance > static_cast<std::size_t>(state_->depth[vertex])) return -1;
        return jump_up(static_cast<int>(vertex), distance);
    }

    [[nodiscard]] bool is_ancestor(std::size_t ancestor, std::size_t vertex) const{
        check_vertex(ancestor);
        check_vertex(vertex);
        const int ancestor_depth = state_->depth[ancestor];
        const int vertex_depth = state_->depth[vertex];
        return ancestor_depth <= vertex_depth
            && jump_up(
                static_cast<int>(vertex),
                static_cast<std::size_t>(vertex_depth - ancestor_depth)
            ) == static_cast<int>(ancestor);
    }

    [[nodiscard]] std::size_t subtree_size(std::size_t vertex) const{
        check_vertex(vertex);
        const int enter_rank = rank_of(state_->enter[vertex]);
        const int leave_rank = rank_of(state_->leave[vertex]);
        return static_cast<std::size_t>(leave_rank - enter_rank + 1) / 2;
    }
};
