#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../algorithm/tree/euler_tour.hpp"
#include "../segtree/lazysegtree.hpp"
#include "../types/monoid_act.hpp"

namespace subtree_add_sum_minimum_tree_internal{

struct Aggregate{
    long long sum;
    long long minimum;
    long long count;
};

constexpr Aggregate operation(Aggregate left, Aggregate right){
    return {
        left.sum + right.sum,
        std::min(left.minimum, right.minimum),
        left.count + right.count,
    };
}

constexpr Aggregate identity(){
    return {0, std::numeric_limits<long long>::max(), 0};
}

constexpr Aggregate add_mapping(long long delta, Aggregate aggregate){
    if(aggregate.count != 0){
        aggregate.sum += delta * aggregate.count;
        aggregate.minimum += delta;
    }
    return aggregate;
}

constexpr long long add_composition(long long added, long long previous){
    return added + previous;
}

constexpr long long no_add(){
    return 0;
}

constexpr auto action = Monoid_Act<
    operation,
    identity,
    add_mapping,
    add_composition,
    no_add
>{};

} // namespace subtree_add_sum_minimum_tree_internal

template<int MAX_SIZE>
class SubtreeAddSumMinimumTree{
    static_assert(0 < MAX_SIZE);

    int vertex_count_;
    bool built_ = false;
    std::vector<long long> initial_value_;
    std::unique_ptr<TreeEulerTour<MAX_SIZE>> tour_;
    std::unique_ptr<LazySegtree<
        subtree_add_sum_minimum_tree_internal::action,
        MAX_SIZE
    >> segment_tree_;

    static int checked_size(int vertex_count){
        if(vertex_count < 0 || MAX_SIZE < vertex_count)[[unlikely]]{
            throw std::out_of_range(
                "SubtreeAddSumMinimumTree size is out of range"
            );
        }
        return vertex_count;
    }

    void check_vertex(int vertex) const{
        if(vertex < 0 || vertex_count_ <= vertex)[[unlikely]]{
            throw std::out_of_range(
                "SubtreeAddSumMinimumTree vertex is out of range"
            );
        }
    }

    void require_not_built(const char* operation) const{
        if(built_)[[unlikely]]{
            throw std::logic_error(operation);
        }
    }

    void require_built(const char* operation) const{
        if(!built_)[[unlikely]]{
            throw std::logic_error(operation);
        }
    }

    std::pair<int, int> subtree_range(int vertex) const{
        require_built("SubtreeAddSumMinimumTree is not built");
        check_vertex(vertex);
        return tour_->subtree_range(vertex);
    }

public:
    explicit SubtreeAddSumMinimumTree(int vertex_count)
        : vertex_count_(checked_size(vertex_count)),
          initial_value_(static_cast<std::size_t>(vertex_count_)),
          tour_(std::make_unique<TreeEulerTour<MAX_SIZE>>(vertex_count_)){}

    int size() const{
        return vertex_count_;
    }

    void set_value(int vertex, long long value){
        require_not_built(
            "SubtreeAddSumMinimumTree cannot set values after build"
        );
        check_vertex(vertex);
        initial_value_[static_cast<std::size_t>(vertex)] = value;
    }

    void add_edge(int left, int right){
        require_not_built(
            "SubtreeAddSumMinimumTree cannot add edges after build"
        );
        tour_->add_edge(left, right);
    }

    void build(int root = 0){
        require_not_built(
            "SubtreeAddSumMinimumTree is already built"
        );
        tour_->build(root);
        using subtree_add_sum_minimum_tree_internal::Aggregate;
        std::vector<Aggregate> preorder(
            static_cast<std::size_t>(vertex_count_)
        );
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            const long long value =
                initial_value_[static_cast<std::size_t>(vertex)];
            preorder[static_cast<std::size_t>(tour_->tin(vertex))] = {
                value, value, 1
            };
        }
        segment_tree_ = std::make_unique<LazySegtree<
            subtree_add_sum_minimum_tree_internal::action,
            MAX_SIZE
        >>(preorder);
        built_ = true;
    }

    long long subtree_sum(int vertex) const{
        const auto [left, right] = subtree_range(vertex);
        return segment_tree_->prod(left, right).sum;
    }

    long long subtree_minimum(int vertex) const{
        const auto [left, right] = subtree_range(vertex);
        return segment_tree_->prod(left, right).minimum;
    }

    void subtree_add(int vertex, long long delta){
        const auto [left, right] = subtree_range(vertex);
        segment_tree_->apply(left, right, delta);
    }

    long long add_subtree_by_capped_minimum(
        int vertex,
        long long cap
    ){
        const auto [left, right] = subtree_range(vertex);
        const long long delta = std::min(
            cap, segment_tree_->prod(left, right).minimum
        );
        segment_tree_->apply(left, right, delta);
        return delta;
    }
};
