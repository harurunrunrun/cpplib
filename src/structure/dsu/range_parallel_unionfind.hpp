#ifndef CPPLIB_SRC_STRUCTURE_DSU_RANGE_PARALLEL_UNIONFIND_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_RANGE_PARALLEL_UNIONFIND_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

template<int MAX_SIZE>
class RangeParallelUnionFind{
    static_assert(MAX_SIZE > 0);

    static constexpr int level_count(){
        int levels = 1;
        while((std::size_t(1) << levels) <= static_cast<std::size_t>(MAX_SIZE)){
            ++levels;
        }
        return levels;
    }

    static constexpr int LEVEL_COUNT = level_count();

    struct State{
        std::array<std::array<int, MAX_SIZE>, LEVEL_COUNT> parent_or_size{};
    };

    int n_ = 0;
    int group_count_ = 0;
    std::unique_ptr<State> state_;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || n_ <= vertex)[[unlikely]] throw std::runtime_error(message);
    }

    void check_range(int left, int right, const char* message) const{
        if(left < 0 || right < left || n_ < right)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int leader_at_level(int level, int vertex){
        auto& parent = state_->parent_or_size[static_cast<std::size_t>(level)];
        int root = vertex;
        while(parent[static_cast<std::size_t>(root)] >= 0){
            root = parent[static_cast<std::size_t>(root)];
        }
        while(vertex != root){
            const int next = parent[static_cast<std::size_t>(vertex)];
            parent[static_cast<std::size_t>(vertex)] = root;
            vertex = next;
        }
        return root;
    }

    std::pair<int, int> unite_at_level(int level, int left, int right){
        left = leader_at_level(level, left);
        right = leader_at_level(level, right);
        if(left == right) return {left, -1};
        auto& parent = state_->parent_or_size[static_cast<std::size_t>(level)];
        if(-parent[static_cast<std::size_t>(left)] <
           -parent[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        parent[static_cast<std::size_t>(left)] +=
            parent[static_cast<std::size_t>(right)];
        parent[static_cast<std::size_t>(right)] = left;
        return {left, right};
    }

    template<class MergeCallback>
    int merge_level(int level, int left, int right, MergeCallback& callback){
        auto [new_root, absorbed_root] = unite_at_level(level, left, right);
        if(absorbed_root == -1) return 0;
        if(level == 0){
            --group_count_;
            callback(new_root, absorbed_root);
            return 1;
        }
        const int half = 1 << (level - 1);
        return merge_level(level - 1, left, right, callback) +
            merge_level(level - 1, left + half, right + half, callback);
    }

public:
    explicit RangeParallelUnionFind(int n = MAX_SIZE):
        n_(n), group_count_(n), state_(std::make_unique<State>()){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (RangeParallelUnionFind constructor)."
            );
        }
        for(int level = 0; level < LEVEL_COUNT; ++level){
            const int length = 1 << level;
            const int count = n_ - length + 1;
            if(count <= 0) break;
            auto& parent = state_->parent_or_size[static_cast<std::size_t>(level)];
            for(int index = 0; index < count; ++index){
                parent[static_cast<std::size_t>(index)] = -1;
            }
        }
    }

    RangeParallelUnionFind(const RangeParallelUnionFind&) = delete;
    RangeParallelUnionFind& operator=(const RangeParallelUnionFind&) = delete;
    RangeParallelUnionFind(RangeParallelUnionFind&&) noexcept = default;
    RangeParallelUnionFind& operator=(RangeParallelUnionFind&&) noexcept = default;

    int size() const noexcept{ return n_; }
    int groups() const noexcept{ return group_count_; }

    int leader(int vertex){
        check_vertex(vertex, "library assertion fault: range violation (leader).");
        return leader_at_level(0, vertex);
    }

    bool same(int left, int right){
        check_vertex(left, "library assertion fault: range violation (same).");
        check_vertex(right, "library assertion fault: range violation (same).");
        return leader_at_level(0, left) == leader_at_level(0, right);
    }

    int component_size(int vertex){
        check_vertex(vertex, "library assertion fault: range violation (component_size).");
        const int root = leader_at_level(0, vertex);
        return -state_->parent_or_size[0][static_cast<std::size_t>(root)];
    }

    template<class MergeCallback>
    int merge(int left, int right, MergeCallback&& callback){
        check_vertex(left, "library assertion fault: range violation (merge).");
        check_vertex(right, "library assertion fault: range violation (merge).");
        auto&& callback_reference = callback;
        return merge_level(0, left, right, callback_reference);
    }

    int merge(int left, int right){
        auto ignore = [](int, int){};
        return merge(left, right, ignore);
    }

    template<class MergeCallback>
    int merge_ranges(
        int first_left,
        int first_right,
        int second_left,
        int second_right,
        MergeCallback&& callback
    ){
        check_range(
            first_left,
            first_right,
            "library assertion fault: range violation (merge_ranges)."
        );
        check_range(
            second_left,
            second_right,
            "library assertion fault: range violation (merge_ranges)."
        );
        const int length = first_right - first_left;
        if(second_right - second_left != length)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: unequal lengths (merge_ranges)."
            );
        }
        if(length == 0) return 0;
        auto&& callback_reference = callback;
        if(length == 1){
            return merge_level(0, first_left, second_left, callback_reference);
        }
        int level = 0;
        while((1 << (level + 1)) < length) ++level;
        const int block_length = 1 << level;
        int result = merge_level(level, first_left, second_left, callback_reference);
        result += merge_level(
            level,
            first_right - block_length,
            second_right - block_length,
            callback_reference
        );
        return result;
    }

    int merge_ranges(
        int first_left,
        int first_right,
        int second_left,
        int second_right
    ){
        auto ignore = [](int, int){};
        return merge_ranges(
            first_left, first_right, second_left, second_right, ignore
        );
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_RANGE_PARALLEL_UNIONFIND_HPP_INCLUDED
