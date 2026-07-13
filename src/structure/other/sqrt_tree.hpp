#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<auto Semigroup, int MAX_SIZE>
struct SqrtTree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Semigroup)>::S;

    static constexpr int MAX_LOG = []{
        int result = 0;
        std::size_t size = 1;
        while(size < static_cast<std::size_t>(MAX_SIZE)){
            size <<= 1;
            result++;
        }
        return result;
    }();

    static constexpr int MAX_LEVELS = []{
        int exponent = MAX_LOG;
        int result = 0;
        while(exponent > 0){
            result++;
            if(exponent == 1) break;
            exponent = (exponent + 1) >> 1;
        }
        return result == 0 ? 1 : result;
    }();

private:
    static constexpr int BETWEEN_CAPACITY = 3 * MAX_SIZE;

    struct State{
        std::array<S, MAX_SIZE> value;
        std::array<S, MAX_LEVELS * MAX_SIZE> prefix;
        std::array<S, MAX_LEVELS * MAX_SIZE> suffix;
        std::array<S, MAX_LEVELS * BETWEEN_CAPACITY> between;
    };

    int length = 0;
    int level_count = 0;
    std::array<int, MAX_LEVELS> layer_log{};
    std::array<int, MAX_LOG + 1> layer_for_bit{};
    std::unique_ptr<State> state;

    S& prefix_at(int level, int index){
        return state->prefix[static_cast<std::size_t>(level * MAX_SIZE + index)];
    }

    const S& prefix_at(int level, int index) const{
        return state->prefix[static_cast<std::size_t>(level * MAX_SIZE + index)];
    }

    S& suffix_at(int level, int index){
        return state->suffix[static_cast<std::size_t>(level * MAX_SIZE + index)];
    }

    const S& suffix_at(int level, int index) const{
        return state->suffix[static_cast<std::size_t>(level * MAX_SIZE + index)];
    }

    S& between_at(int level, std::size_t index){
        return state->between[
            static_cast<std::size_t>(level) * BETWEEN_CAPACITY + index
        ];
    }

    const S& between_at(int level, std::size_t index) const{
        return state->between[
            static_cast<std::size_t>(level) * BETWEEN_CAPACITY + index
        ];
    }

    void build_layer(int level){
        int exponent = layer_log[static_cast<std::size_t>(level)];
        int next_exponent = exponent == 1 ? 0 : (exponent + 1) >> 1;
        std::size_t segment_size = std::size_t(1) << exponent;
        std::size_t block_size = std::size_t(1) << next_exponent;
        std::size_t blocks_per_segment = segment_size / block_size;
        std::size_t block_square = blocks_per_segment * blocks_per_segment;

        std::size_t segment_count =
            (static_cast<std::size_t>(length) + segment_size - 1) / segment_size;
        if(segment_count * block_square > static_cast<std::size_t>(BETWEEN_CAPACITY))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: internal capacity exceeded (SqrtTree)."
            );
        }
        for(std::size_t segment_start = 0;
            segment_start < static_cast<std::size_t>(length);
            segment_start += segment_size){
            std::size_t segment_end = std::min(
                segment_start + segment_size,
                static_cast<std::size_t>(length)
            );
            std::size_t block_count =
                (segment_end - segment_start + block_size - 1) / block_size;

            for(std::size_t block = 0; block < block_count; block++){
                std::size_t block_start = segment_start + block * block_size;
                std::size_t block_end = std::min(block_start + block_size, segment_end);
                prefix_at(level, static_cast<int>(block_start)) =
                    state->value[block_start];
                for(std::size_t i = block_start + 1; i < block_end; i++){
                    prefix_at(level, static_cast<int>(i)) = Semigroup.op(
                        prefix_at(level, static_cast<int>(i - 1)),
                        state->value[i]
                    );
                }
                suffix_at(level, static_cast<int>(block_end - 1)) =
                    state->value[block_end - 1];
                for(std::size_t i = block_end - 1; i > block_start; i--){
                    suffix_at(level, static_cast<int>(i - 1)) = Semigroup.op(
                        state->value[i - 1],
                        suffix_at(level, static_cast<int>(i))
                    );
                }
            }

            std::size_t segment_index = segment_start / segment_size;
            std::size_t base = segment_index * block_square;
            for(std::size_t first = 0; first < block_count; first++){
                std::size_t first_end = std::min(
                    segment_start + (first + 1) * block_size,
                    segment_end
                );
                S aggregate = prefix_at(level, static_cast<int>(first_end - 1));
                between_at(
                    level,
                    base + first * blocks_per_segment + first
                ) = aggregate;
                for(std::size_t last = first + 1; last < block_count; last++){
                    std::size_t last_end = std::min(
                        segment_start + (last + 1) * block_size,
                        segment_end
                    );
                    aggregate = Semigroup.op(
                        aggregate,
                        prefix_at(level, static_cast<int>(last_end - 1))
                    );
                    between_at(
                        level,
                        base + first * blocks_per_segment + last
                    ) = aggregate;
                }
            }
        }
    }

public:
    SqrtTree(): state(std::make_unique<State>()){
        layer_for_bit.fill(-1);
    }

    explicit SqrtTree(const std::vector<S>& values): SqrtTree(){
        build(values);
    }

    SqrtTree(const SqrtTree&) = delete;
    SqrtTree& operator=(const SqrtTree&) = delete;
    SqrtTree(SqrtTree&&) = default;
    SqrtTree& operator=(SqrtTree&&) = default;

    int size() const{ return length; }
    bool empty() const{ return length == 0; }

    void build(const std::vector<S>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (SqrtTree::build)."
            );
        }
        length = static_cast<int>(values.size());
        for(int i = 0; i < length; i++){
            state->value[static_cast<std::size_t>(i)] = values[static_cast<std::size_t>(i)];
        }
        level_count = 0;
        layer_for_bit.fill(-1);
        if(length <= 1) return;

        int top_log = static_cast<int>(
            std::bit_width(static_cast<unsigned>(length - 1))
        );
        int exponent = top_log;
        while(exponent > 0){
            layer_log[static_cast<std::size_t>(level_count++)] = exponent;
            if(exponent == 1) break;
            exponent = (exponent + 1) >> 1;
        }
        for(int level = 0; level < level_count; level++){
            int current = layer_log[static_cast<std::size_t>(level)];
            int next = current == 1 ? 0 : (current + 1) >> 1;
            for(int bit = next; bit < current; bit++){
                layer_for_bit[static_cast<std::size_t>(bit)] = level;
            }
            build_layer(level);
        }
    }

    S get(int index) const{
        if(index < 0 || length <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (SqrtTree::get)."
            );
        }
        return state->value[static_cast<std::size_t>(index)];
    }

    S prod(int left, int right) const{
        if(left < 0 || right <= left || length < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (SqrtTree::prod)."
            );
        }
        if(right - left == 1) return state->value[static_cast<std::size_t>(left)];

        int differing_bit = static_cast<int>(
            std::bit_width(static_cast<unsigned>(left ^ (right - 1)))
        ) - 1;
        int level = layer_for_bit[static_cast<std::size_t>(differing_bit)];
        int exponent = layer_log[static_cast<std::size_t>(level)];
        int next_exponent = exponent == 1 ? 0 : (exponent + 1) >> 1;
        std::size_t segment_size = std::size_t(1) << exponent;
        std::size_t block_size = std::size_t(1) << next_exponent;
        std::size_t blocks_per_segment = segment_size / block_size;
        std::size_t segment_index = static_cast<std::size_t>(left) / segment_size;
        std::size_t segment_start = segment_index * segment_size;
        std::size_t left_block =
            (static_cast<std::size_t>(left) - segment_start) / block_size;
        std::size_t right_block =
            (static_cast<std::size_t>(right - 1) - segment_start) / block_size;

        S result = suffix_at(level, left);
        if(left_block + 1 < right_block){
            std::size_t base =
                segment_index * blocks_per_segment * blocks_per_segment;
            result = Semigroup.op(
                result,
                between_at(
                    level,
                    base + (left_block + 1) * blocks_per_segment + right_block - 1
                )
            );
        }
        return Semigroup.op(result, prefix_at(level, right - 1));
    }

    S all_prod() const{
        if(length == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty tree (SqrtTree::all_prod)."
            );
        }
        return prod(0, length);
    }
};
