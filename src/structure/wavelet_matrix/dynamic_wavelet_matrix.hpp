#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <queue>
#include <tuple>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#if (defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86))
#include <immintrin.h>
#define DYNAMIC_WAVELET_MATRIX_X86 1
#else
#define DYNAMIC_WAVELET_MATRIX_X86 0
#endif

namespace dynamic_wavelet_matrix_detail{

constexpr int ceil_log2_nonzero(unsigned int value){
    int result = 0;
    unsigned int current = value - 1;
    while(current != 0){
        current >>= 1;
        result++;
    }
    return result;
}

template<class T, int MAX_SIZE>
struct default_sum_type{
    using U = std::make_unsigned_t<T>;
    static constexpr int value_bits = std::numeric_limits<U>::digits;
    static constexpr int count_bits = MAX_SIZE <= 1
        ? 0 : ceil_log2_nonzero(static_cast<unsigned int>(MAX_SIZE));
    static constexpr int required_digits = MAX_SIZE == 0
        ? 0
        : (std::is_signed_v<T> ? value_bits - 1 : value_bits) + count_bits;
#if defined(__SIZEOF_INT128__)
    using type = std::conditional_t<
        required_digits <= std::numeric_limits<long long>::digits,
        long long,
        __int128_t
    >;
#else
    using type = long long;
#endif
};

template<class T, int MAX_SIZE>
using default_sum_type_t = typename default_sum_type<T, MAX_SIZE>::type;

}

template<
    class T,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits,
    class SUM_TYPE = dynamic_wavelet_matrix_detail::default_sum_type_t<T, MAX_SIZE>,
    int LEAF_WORDS = 2
>
struct DynamicWaveletMatrix{
    static_assert(std::is_integral_v<T>);
    static_assert(MAX_SIZE >= 0);

    using value_type = T;
    using sum_type = SUM_TYPE;
    using unsigned_value_type = std::make_unsigned_t<T>;
    static constexpr bool has_sum_support = true;
    static constexpr bool has_bitwise_support = true;

    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;

    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>);
    static_assert(LEAF_WORDS > 0);
    static_assert(std::numeric_limits<sum_type>::is_specialized);
    static_assert(
        !std::numeric_limits<sum_type>::is_bounded
        || std::numeric_limits<sum_type>::is_signed,
        "SUM_TYPE must be signed or unbounded."
    );
    static constexpr int required_sum_digits =
        dynamic_wavelet_matrix_detail::default_sum_type<T, MAX_SIZE>::required_digits;
    static_assert(
        !std::numeric_limits<sum_type>::is_bounded
        || std::numeric_limits<sum_type>::digits >= required_sum_digits,
        "SUM_TYPE is too narrow for MAX_SIZE elements of T."
    );

    enum class BitwiseOperation{
        Xor,
        Or,
        And,
    };

private:
    class DynamicBitVector{
    public:
        struct RankPair{
            int ones_l = 0;
            int ones_r = 0;
        };

        struct RangeStats{
            int ones_l = 0;
            int ones_r = 0;
            sum_type zero_sum = 0;
            sum_type one_sum = 0;
        };

    private:
        static constexpr int leaf_words = LEAF_WORDS;
        static constexpr int leaf_capacity = leaf_words * 64;
        static constexpr int leaf_minimum = leaf_capacity / 2;
        static constexpr int internal_capacity = 16;
        static constexpr int internal_minimum = internal_capacity / 2;

        struct Node{
            bool is_leaf;
            int size = 0;
            int ones = 0;
            sum_type zero_sum = 0;
            sum_type one_sum = 0;

            explicit Node(bool leaf): is_leaf(leaf){}
            virtual ~Node() = default;
        };

        struct Leaf final : Node{
            int length = 0;
            std::array<std::uint64_t, leaf_words> words{};
            std::array<T, leaf_capacity> weights{};

            Leaf(): Node(true){}
        };

        struct Internal final : Node{
            std::vector<std::unique_ptr<Node>> children;

            Internal(): Node(false){
                children.reserve(internal_capacity + 1);
            }
        };

        std::unique_ptr<Node> root = std::make_unique<Leaf>();

        static Leaf* as_leaf(Node* node){
            return static_cast<Leaf*>(node);
        }

        static const Leaf* as_leaf(const Node* node){
            return static_cast<const Leaf*>(node);
        }

        static Internal* as_internal(Node* node){
            return static_cast<Internal*>(node);
        }

        static const Internal* as_internal(const Node* node){
            return static_cast<const Internal*>(node);
        }

        static bool leaf_get(const Leaf* leaf, int index){
            return static_cast<bool>(
                (leaf->words[static_cast<std::size_t>(index >> 6)] >> (index & 63)) & 1ULL
            );
        }

        static void leaf_set(Leaf* leaf, int index, bool value){
            std::uint64_t mask = 1ULL << (index & 63);
            auto& word = leaf->words[static_cast<std::size_t>(index >> 6)];
            if(value) word |= mask;
            else word &= ~mask;
        }

        static int leaf_prefix_ones(const Leaf* leaf, int end){
            int result = 0;
            int full_words = end >> 6;
            for(int i = 0; i < full_words; i++){
                result += std::popcount(
                    leaf->words[static_cast<std::size_t>(i)]
                );
            }
            int remainder = end & 63;
            if(remainder != 0){
                std::uint64_t mask = (1ULL << remainder) - 1;
                result += std::popcount(
                    leaf->words[static_cast<std::size_t>(full_words)] & mask
                );
            }
            return result;
        }

        static void recalculate(Leaf* leaf){
            leaf->size = leaf->length;
            leaf->ones = 0;
            leaf->zero_sum = 0;
            leaf->one_sum = 0;
            for(int i = 0; i < leaf->length; i++){
                if(leaf_get(leaf, i)){
                    leaf->ones++;
                    leaf->one_sum += leaf->weights[static_cast<std::size_t>(i)];
                }
                else{
                    leaf->zero_sum += leaf->weights[static_cast<std::size_t>(i)];
                }
            }
        }

        static void recalculate(Internal* internal){
            internal->size = 0;
            internal->ones = 0;
            internal->zero_sum = 0;
            internal->one_sum = 0;
            for(const auto& child : internal->children){
                internal->size += child->size;
                internal->ones += child->ones;
                internal->zero_sum += child->zero_sum;
                internal->one_sum += child->one_sum;
            }
        }

        static void shift_bits_right(Leaf* leaf, int position){
            int first_word = position >> 6;
            int last_word = leaf->length >> 6;
            for(int word = last_word; word > first_word; word--){
                leaf->words[static_cast<std::size_t>(word)] =
                    (leaf->words[static_cast<std::size_t>(word)] << 1)
                    | (leaf->words[static_cast<std::size_t>(word - 1)] >> 63);
            }
            int offset = position & 63;
            std::uint64_t low_mask = offset == 0
                ? 0 : (std::uint64_t{1} << offset) - 1;
            std::uint64_t current = leaf->words[static_cast<std::size_t>(first_word)];
            leaf->words[static_cast<std::size_t>(first_word)] =
                (current & low_mask) | ((current & ~low_mask) << 1);
        }

        static void shift_bits_left(Leaf* leaf, int position){
            int first_word = position >> 6;
            int last_word = (leaf->length - 1) >> 6;
            int offset = position & 63;
            std::uint64_t low_mask = offset == 0
                ? 0 : (std::uint64_t{1} << offset) - 1;
            for(int word = first_word; word <= last_word; word++){
                std::uint64_t current = leaf->words[static_cast<std::size_t>(word)];
                std::uint64_t next = word < last_word
                    ? leaf->words[static_cast<std::size_t>(word + 1)] : 0;
                if(word == first_word){
                    std::uint64_t shifted = offset == 63
                        ? 0 : ((current >> (offset + 1)) << offset);
                    leaf->words[static_cast<std::size_t>(word)] =
                        (current & low_mask) | shifted | (next << 63);
                }
                else{
                    leaf->words[static_cast<std::size_t>(word)] =
                        (current >> 1) | (next << 63);
                }
            }
        }

        static void leaf_insert(Leaf* leaf, int position, bool value, T weight){
            assert(0 <= position && position <= leaf->length);
            assert(leaf->length < leaf_capacity);
            shift_bits_right(leaf, position);
            std::memmove(
                leaf->weights.data() + position + 1,
                leaf->weights.data() + position,
                static_cast<std::size_t>(leaf->length - position) * sizeof(T)
            );
            leaf_set(leaf, position, value);
            leaf->weights[static_cast<std::size_t>(position)] = weight;
            leaf->length++;
            leaf->size++;
            leaf->ones += value ? 1 : 0;
            if(value) leaf->one_sum += static_cast<sum_type>(weight);
            else leaf->zero_sum += static_cast<sum_type>(weight);
        }

        static std::pair<bool, T> leaf_erase(Leaf* leaf, int position){
            assert(0 <= position && position < leaf->length);
            bool result = leaf_get(leaf, position);
            T weight = leaf->weights[static_cast<std::size_t>(position)];
            shift_bits_left(leaf, position);
            std::memmove(
                leaf->weights.data() + position,
                leaf->weights.data() + position + 1,
                static_cast<std::size_t>(leaf->length - position - 1) * sizeof(T)
            );
            leaf->length--;
            leaf_set(leaf, leaf->length, false);
            leaf->weights[static_cast<std::size_t>(leaf->length)] = T{0};
            leaf->size--;
            leaf->ones -= result ? 1 : 0;
            if(result) leaf->one_sum -= static_cast<sum_type>(weight);
            else leaf->zero_sum -= static_cast<sum_type>(weight);
            return {result, weight};
        }

        static void leaf_append(Leaf* destination, const Leaf* source){
            assert(destination->length + source->length <= leaf_capacity);
            int offset = destination->length;
            for(int i = 0; i < source->length; i++){
                leaf_set(destination, offset + i, leaf_get(source, i));
                destination->weights[static_cast<std::size_t>(offset + i)] = source->weights[static_cast<std::size_t>(i)];
            }
            destination->length += source->length;
            recalculate(destination);
        }

        static std::unique_ptr<Node> split_full_leaf(
            Leaf* leaf,
            int position,
            bool value,
            T weight
        ){
            std::array<std::uint64_t, leaf_words> old_words = leaf->words;
            std::array<T, leaf_capacity> old_weights = leaf->weights;

            auto old_get = [&](int index){
                return static_cast<bool>(
                    (old_words[static_cast<std::size_t>(index >> 6)] >> (index & 63)) & 1ULL
                );
            };

            auto inserted_get = [&](int index){
                if(index < position) return old_get(index);
                if(index == position) return value;
                return old_get(index - 1);
            };
            auto inserted_weight = [&](int index){
                if(index < position) return old_weights[static_cast<std::size_t>(index)];
                if(index == position) return weight;
                return old_weights[static_cast<std::size_t>(index - 1)];
            };

            constexpr int total = leaf_capacity + 1;
            constexpr int left_length = total / 2;
            constexpr int right_length = total - left_length;

            leaf->words.fill(0);
            leaf->weights.fill(0);
            leaf->length = left_length;
            for(int i = 0; i < left_length; i++){
                leaf_set(leaf, i, inserted_get(i));
                leaf->weights[static_cast<std::size_t>(i)] = inserted_weight(i);
            }
            recalculate(leaf);

            auto right = std::make_unique<Leaf>();
            right->length = right_length;
            for(int i = 0; i < right_length; i++){
                leaf_set(right.get(), i, inserted_get(left_length + i));
                right->weights[static_cast<std::size_t>(i)] = inserted_weight(left_length + i);
            }
            recalculate(right.get());
            return right;
        }

        static std::unique_ptr<Node> insert_recursive(
            std::unique_ptr<Node>& node,
            int position,
            bool value,
            T weight
        ){
            if(node->is_leaf){
                Leaf* leaf = as_leaf(node.get());
                if(leaf->length < leaf_capacity){
                    leaf_insert(leaf, position, value, weight);
                    return nullptr;
                }
                return split_full_leaf(leaf, position, value, weight);
            }

            Internal* internal = as_internal(node.get());
            std::size_t child_index = 0;
            int local_position = position;

            while(child_index + 1 < internal->children.size()){
                int child_size = internal->children[child_index]->size;
                if(local_position <= child_size) break;
                local_position -= child_size;
                child_index++;
            }

            auto sibling = insert_recursive(
                internal->children[child_index],
                local_position,
                value,
                weight
            );

            if(sibling){
                internal->children.insert(
                    internal->children.begin() + static_cast<std::ptrdiff_t>(child_index + 1),
                    std::move(sibling)
                );
            }

            recalculate(internal);
            if(static_cast<int>(internal->children.size()) <= internal_capacity){
                return nullptr;
            }

            auto right = std::make_unique<Internal>();
            constexpr int left_children = (internal_capacity + 1) / 2;
            right->children.reserve(internal_capacity + 1);

            for(
                auto iterator = internal->children.begin() + left_children;
                iterator != internal->children.end();
                iterator++
            ){
                right->children.push_back(std::move(*iterator));
            }
            internal->children.erase(
                internal->children.begin() + left_children,
                internal->children.end()
            );

            recalculate(internal);
            recalculate(right.get());
            return right;
        }

        static bool is_underfull(const Node* node){
            if(node->is_leaf){
                return as_leaf(node)->length < leaf_minimum;
            }
            return static_cast<int>(as_internal(node)->children.size()) < internal_minimum;
        }

        static bool can_lend(const Node* node){
            if(node->is_leaf){
                return as_leaf(node)->length > leaf_minimum;
            }
            return static_cast<int>(as_internal(node)->children.size()) > internal_minimum;
        }

        static void borrow_from_left(Node* left_node, Node* child_node){
            if(child_node->is_leaf){
                Leaf* left = as_leaf(left_node);
                Leaf* child = as_leaf(child_node);
                auto [value, weight] = leaf_erase(left, left->length - 1);
                leaf_insert(child, 0, value, weight);
                return;
            }

            Internal* left = as_internal(left_node);
            Internal* child = as_internal(child_node);
            child->children.insert(
                child->children.begin(),
                std::move(left->children.back())
            );
            left->children.pop_back();
            recalculate(left);
            recalculate(child);
        }

        static void borrow_from_right(Node* child_node, Node* right_node){
            if(child_node->is_leaf){
                Leaf* child = as_leaf(child_node);
                Leaf* right = as_leaf(right_node);
                auto [value, weight] = leaf_erase(right, 0);
                leaf_insert(child, child->length, value, weight);
                return;
            }

            Internal* child = as_internal(child_node);
            Internal* right = as_internal(right_node);
            child->children.push_back(std::move(right->children.front()));
            right->children.erase(right->children.begin());
            recalculate(child);
            recalculate(right);
        }

        static void merge_nodes(Node* left_node, Node* right_node){
            if(left_node->is_leaf){
                leaf_append(as_leaf(left_node), as_leaf(right_node));
                return;
            }

            Internal* left = as_internal(left_node);
            Internal* right = as_internal(right_node);
            for(auto& child : right->children){
                left->children.push_back(std::move(child));
            }
            right->children.clear();
            recalculate(left);
        }

        static void rebalance_child(Internal* parent, std::size_t child_index){
            if(!is_underfull(parent->children[child_index].get())) return;

            if(
                child_index > 0
                && can_lend(parent->children[child_index - 1].get())
            ){
                borrow_from_left(
                    parent->children[child_index - 1].get(),
                    parent->children[child_index].get()
                );
                recalculate(parent);
                return;
            }

            if(
                child_index + 1 < parent->children.size()
                && can_lend(parent->children[child_index + 1].get())
            ){
                borrow_from_right(
                    parent->children[child_index].get(),
                    parent->children[child_index + 1].get()
                );
                recalculate(parent);
                return;
            }

            if(child_index > 0){
                merge_nodes(
                    parent->children[child_index - 1].get(),
                    parent->children[child_index].get()
                );
                parent->children.erase(
                    parent->children.begin() + static_cast<std::ptrdiff_t>(child_index)
                );
            }
            else if(child_index + 1 < parent->children.size()){
                merge_nodes(
                    parent->children[child_index].get(),
                    parent->children[child_index + 1].get()
                );
                parent->children.erase(
                    parent->children.begin() + static_cast<std::ptrdiff_t>(child_index + 1)
                );
            }

            recalculate(parent);
        }

        static std::pair<bool, T> erase_recursive(std::unique_ptr<Node>& node, int position){
            if(node->is_leaf){
                return leaf_erase(as_leaf(node.get()), position);
            }

            Internal* internal = as_internal(node.get());
            std::size_t child_index = 0;
            int local_position = position;

            while(true){
                int child_size = internal->children[child_index]->size;
                if(local_position < child_size) break;
                local_position -= child_size;
                child_index++;
            }

            auto result = erase_recursive(
                internal->children[child_index],
                local_position
            );
            rebalance_child(internal, child_index);
            recalculate(internal);
            return result;
        }

        static RankPair rank_pair_recursive(const Node* node, int l, int r){
            if(node->is_leaf){
                const Leaf* leaf = as_leaf(node);
                return {leaf_prefix_ones(leaf, l), leaf_prefix_ones(leaf, r)};
            }

            const Internal* internal = as_internal(node);
            int prefix_ones = 0;
            std::size_t child_index = 0;
            int local_l = l;
            int local_r = r;

            while(
                child_index < internal->children.size()
                && local_l >= internal->children[child_index]->size
            ){
                int child_size = internal->children[child_index]->size;
                prefix_ones += internal->children[child_index]->ones;
                local_l -= child_size;
                local_r -= child_size;
                child_index++;
            }

            RankPair result{prefix_ones, prefix_ones};
            if(child_index == internal->children.size()) return result;

            const Node* child = internal->children[child_index].get();
            if(local_r <= child->size){
                RankPair partial = rank_pair_recursive(child, local_l, local_r);
                result.ones_l += partial.ones_l;
                result.ones_r += partial.ones_r;
                return result;
            }

            result.ones_l += rank1_recursive(child, local_l);
            result.ones_r += child->ones;
            local_r -= child->size;
            child_index++;

            while(
                child_index < internal->children.size()
                && local_r >= internal->children[child_index]->size
            ){
                result.ones_r += internal->children[child_index]->ones;
                local_r -= internal->children[child_index]->size;
                child_index++;
            }
            if(local_r > 0 && child_index < internal->children.size()){
                result.ones_r += rank1_recursive(
                    internal->children[child_index].get(),
                    local_r
                );
            }
            return result;
        }

        static int rank1_recursive(const Node* node, int end){
            if(node->is_leaf){
                return leaf_prefix_ones(as_leaf(node), end);
            }

            const Internal* internal = as_internal(node);
            int result = 0;
            int remaining = end;

            for(const auto& child : internal->children){
                if(remaining >= child->size){
                    result += child->ones;
                    remaining -= child->size;
                }
                else{
                    result += rank1_recursive(child.get(), remaining);
                    break;
                }
            }
            return result;
        }

        struct PrefixStats{
            int ones = 0;
            sum_type zero_sum = 0;
            sum_type one_sum = 0;
        };

        static PrefixStats prefix_stats_recursive(const Node* node, int end){
            if(node->is_leaf){
                const Leaf* leaf = as_leaf(node);
                PrefixStats result;
                for(int i = 0; i < end; i++){
                    sum_type weight = static_cast<sum_type>(
                        leaf->weights[static_cast<std::size_t>(i)]
                    );
                    if(leaf_get(leaf, i)){
                        result.ones++;
                        result.one_sum += weight;
                    }
                    else{
                        result.zero_sum += weight;
                    }
                }
                return result;
            }

            const Internal* internal = as_internal(node);
            PrefixStats result;
            int remaining = end;
            for(const auto& child : internal->children){
                if(remaining >= child->size){
                    result.ones += child->ones;
                    result.zero_sum += child->zero_sum;
                    result.one_sum += child->one_sum;
                    remaining -= child->size;
                }
                else{
                    PrefixStats partial = prefix_stats_recursive(child.get(), remaining);
                    result.ones += partial.ones;
                    result.zero_sum += partial.zero_sum;
                    result.one_sum += partial.one_sum;
                    break;
                }
            }
            return result;
        }

        static sum_type prefix_sum_recursive(const Node* node, int end, bool value){
            if(node->is_leaf){
                const Leaf* leaf = as_leaf(node);
                sum_type result = 0;
                for(int i = 0; i < end; i++){
                    if(leaf_get(leaf, i) == value){
                        result += leaf->weights[static_cast<std::size_t>(i)];
                    }
                }
                return result;
            }

            const Internal* internal = as_internal(node);
            sum_type result = 0;
            int remaining = end;
            for(const auto& child : internal->children){
                if(remaining >= child->size){
                    result += value ? child->one_sum : child->zero_sum;
                    remaining -= child->size;
                }
                else{
                    result += prefix_sum_recursive(child.get(), remaining, value);
                    break;
                }
            }
            return result;
        }

        static sum_type prefix_sum_all_recursive(const Node* node, int end){
            if(node->is_leaf){
                const Leaf* leaf = as_leaf(node);
                sum_type result = 0;
                for(int i = 0; i < end; i++){
                    result += leaf->weights[static_cast<std::size_t>(i)];
                }
                return result;
            }

            const Internal* internal = as_internal(node);
            sum_type result = 0;
            int remaining = end;
            for(const auto& child : internal->children){
                if(remaining >= child->size){
                    result += child->zero_sum + child->one_sum;
                    remaining -= child->size;
                }
                else{
                    result += prefix_sum_all_recursive(child.get(), remaining);
                    break;
                }
            }
            return result;
        }

        static bool access_recursive(const Node* node, int position){
            if(node->is_leaf){
                return leaf_get(as_leaf(node), position);
            }

            const Internal* internal = as_internal(node);
            int remaining = position;
            for(const auto& child : internal->children){
                if(remaining < child->size){
                    return access_recursive(child.get(), remaining);
                }
                remaining -= child->size;
            }
            throw std::logic_error("DynamicBitVector access failure.");
        }

#if DYNAMIC_WAVELET_MATRIX_X86 && (defined(__GNUC__) || defined(__clang__))
        static bool runtime_has_bmi2(){
            static const bool available = []{
                __builtin_cpu_init();
                return __builtin_cpu_supports("bmi2");
            }();
            return available;
        }

        __attribute__((target("bmi2")))
        static int select_set_bit_bmi2(std::uint64_t word, int k){
            return static_cast<int>(std::countr_zero(
                _pdep_u64(std::uint64_t{1} << k, word)
            ));
        }
#endif

        static int select_set_bit(std::uint64_t word, int k){
#if DYNAMIC_WAVELET_MATRIX_X86 && (defined(__GNUC__) || defined(__clang__))
            if(runtime_has_bmi2()) return select_set_bit_bmi2(word, k);
#endif
            while(k-- > 0) word &= word - 1;
            return static_cast<int>(std::countr_zero(word));
        }

        static int select_recursive(const Node* node, int k, bool value){
            if(node->is_leaf){
                const Leaf* leaf = as_leaf(node);
                int word_count = (leaf->length + 63) >> 6;
                for(int word_index = 0; word_index < word_count; word_index++){
                    std::uint64_t word = leaf->words[static_cast<std::size_t>(word_index)];
                    if(!value) word = ~word;
                    if(word_index + 1 == word_count && (leaf->length & 63) != 0){
                        word &= (std::uint64_t{1} << (leaf->length & 63)) - 1;
                    }
                    int count = std::popcount(word);
                    if(k < count){
                        return word_index * 64 + select_set_bit(word, k);
                    }
                    k -= count;
                }
                throw std::logic_error("DynamicBitVector select failure.");
            }

            const Internal* internal = as_internal(node);
            int offset = 0;
            for(const auto& child : internal->children){
                int count = value ? child->ones : child->size - child->ones;
                if(k < count){
                    return offset + select_recursive(child.get(), k, value);
                }
                k -= count;
                offset += child->size;
            }
            throw std::logic_error("DynamicBitVector select failure.");
        }

        static std::unique_ptr<Node> make_leaf(
            const std::vector<unsigned char>& bits,
            const std::vector<T>& weights,
            int first,
            int length
        ){
            auto leaf = std::make_unique<Leaf>();
            leaf->length = length;
            for(int i = 0; i < length; i++){
                if(bits[static_cast<std::size_t>(first + i)] != 0){
                    leaf_set(leaf.get(), i, true);
                }
                leaf->weights[static_cast<std::size_t>(i)] = weights[static_cast<std::size_t>(first + i)];
            }
            recalculate(leaf.get());
            return leaf;
        }

        static std::vector<int> balanced_group_sizes(
            int total,
            int capacity
        ){
            int groups = (total + capacity - 1) / capacity;
            std::vector<int> result(static_cast<std::size_t>(groups), total / groups);
            for(int i = 0; i < total % groups; i++){
                result[static_cast<std::size_t>(i)]++;
            }
            return result;
        }

        static std::unique_ptr<Node> clone_node(const Node* node){
            if(node->is_leaf){
                const Leaf* source = as_leaf(node);
                auto result = std::make_unique<Leaf>();
                result->length = source->length;
                result->words = source->words;
                result->weights = source->weights;
                recalculate(result.get());
                return result;
            }

            const Internal* source = as_internal(node);
            auto result = std::make_unique<Internal>();
            for(const auto& child : source->children){
                result->children.push_back(clone_node(child.get()));
            }
            recalculate(result.get());
            return result;
        }

    public:
        DynamicBitVector() = default;

        DynamicBitVector(const DynamicBitVector& other):
            root(clone_node(other.root.get())){}

        DynamicBitVector& operator=(const DynamicBitVector& other){
            if(this != &other){
                root = clone_node(other.root.get());
            }
            return *this;
        }
        DynamicBitVector(DynamicBitVector&&) noexcept = default;
        DynamicBitVector& operator=(DynamicBitVector&&) noexcept = default;

        int size() const{
            return root->size;
        }

        int ones() const{
            return root->ones;
        }

        void assign(const std::vector<unsigned char>& bits, const std::vector<T>& weights){
            if(bits.empty()){
                root = std::make_unique<Leaf>();
                return;
            }

            std::vector<std::unique_ptr<Node>> current;
            auto leaf_sizes = balanced_group_sizes(
                static_cast<int>(bits.size()),
                leaf_capacity
            );
            current.reserve(leaf_sizes.size());

            int first = 0;
            for(int length : leaf_sizes){
                current.push_back(make_leaf(bits, weights, first, length));
                first += length;
            }

            while(current.size() > 1){
                auto group_sizes = balanced_group_sizes(
                    static_cast<int>(current.size()),
                    internal_capacity
                );
                std::vector<std::unique_ptr<Node>> next;
                next.reserve(group_sizes.size());

                std::size_t index = 0;
                for(int group_size : group_sizes){
                    auto internal = std::make_unique<Internal>();
                    for(int i = 0; i < group_size; i++){
                        internal->children.push_back(std::move(current[index++]));
                    }
                    recalculate(internal.get());
                    next.push_back(std::move(internal));
                }
                current = std::move(next);
            }

            root = std::move(current.front());
        }

        bool access(int position) const{
            assert(0 <= position && position < size());
            return access_recursive(root.get(), position);
        }

        RankPair rank_pair(int l, int r) const{
            assert(0 <= l && l <= r && r <= size());
            return rank_pair_recursive(root.get(), l, r);
        }

        int rank1(int end) const{
            assert(0 <= end && end <= size());
            return rank1_recursive(root.get(), end);
        }

        int rank0(int end) const{
            return end - rank1(end);
        }

        int select1(int k) const{
            assert(0 <= k && k < ones());
            return select_recursive(root.get(), k, true);
        }

        int select0(int k) const{
            assert(0 <= k && k < size() - ones());
            return select_recursive(root.get(), k, false);
        }

        RangeStats range_stats(int l, int r) const{
            assert(0 <= l && l <= r && r <= size());
            PrefixStats left = prefix_stats_recursive(root.get(), l);
            PrefixStats right = prefix_stats_recursive(root.get(), r);
            return {
                left.ones,
                right.ones,
                right.zero_sum - left.zero_sum,
                right.one_sum - left.one_sum
            };
        }

        sum_type sum(bool bit, int l, int r) const{
            assert(0 <= l && l <= r && r <= size());
            return prefix_sum_recursive(root.get(), r, bit)
                - prefix_sum_recursive(root.get(), l, bit);
        }

        sum_type sum_all(int l, int r) const{
            assert(0 <= l && l <= r && r <= size());
            return prefix_sum_all_recursive(root.get(), r)
                - prefix_sum_all_recursive(root.get(), l);
        }

        void insert(int position, bool value, T weight){
            assert(0 <= position && position <= size());
            auto sibling = insert_recursive(root, position, value, weight);
            if(sibling){
                auto new_root = std::make_unique<Internal>();
                new_root->children.push_back(std::move(root));
                new_root->children.push_back(std::move(sibling));
                recalculate(new_root.get());
                root = std::move(new_root);
            }
        }

        std::pair<bool, T> erase(int position){
            assert(0 <= position && position < size());
            auto result = erase_recursive(root, position);

            while(!root->is_leaf){
                Internal* internal = as_internal(root.get());
                if(internal->children.size() != 1) break;
                root = std::move(internal->children.front());
            }
            return result;
        }
    };

    int _n = 0;
    std::vector<U> encoded_values;
    std::vector<T> original_values;
    std::array<DynamicBitVector, static_cast<std::size_t>(BIT_WIDTH)> bit_vectors{};
    std::array<int, static_cast<std::size_t>(BIT_WIDTH)> zero_count{};
    std::map<T, int> global_frequency;
    mutable std::vector<T> distinct_cache;
    mutable bool distinct_cache_dirty = true;

    static constexpr U sign_mask = std::is_signed_v<T>
        ? U{1} << (digits - 1)
        : U{0};

    static U encode(T value){
        return static_cast<U>(value) ^ sign_mask;
    }

    static T decode(U value){
        U decoded = value ^ sign_mask;
        if constexpr(std::is_signed_v<T>){
            return std::bit_cast<T>(decoded);
        }
        else{
            return static_cast<T>(decoded);
        }
    }

    static bool fits(U value){
        if constexpr(BIT_WIDTH < digits){
            return (value >> BIT_WIDTH) == 0;
        }
        else{
            return true;
        }
    }

    static U encode_checked(T value, const char* message){
        U result = encode(value);
        if(!fits(result))[[unlikely]]{
            throw std::runtime_error(message);
        }
        return result;
    }

    static bool bit_at(U value, int bit){
        return static_cast<bool>((value >> bit) & U{1});
    }

#if DYNAMIC_WAVELET_MATRIX_X86 && (defined(__GNUC__) || defined(__clang__))
    static bool runtime_has_avx512f(){
        static const bool available = []{
            __builtin_cpu_init();
            return __builtin_cpu_supports("avx512f");
        }();
        return available;
    }

    __attribute__((target("avx512f")))
    static void stable_partition_avx512_32(
        const U* input,
        U* output,
        int length,
        int bit,
        int zero_count_value
    ){
        constexpr int lanes = 16;
        int zero_position = 0;
        int one_position = zero_count_value;
        int i = 0;
        const __m512i bit_mask = _mm512_set1_epi32(
            static_cast<int>(std::uint32_t{1} << bit)
        );
        for(; i + lanes <= length; i += lanes){
            __m512i values = _mm512_loadu_si512(
                static_cast<const void*>(input + i)
            );
            __mmask16 one_mask = _mm512_test_epi32_mask(values, bit_mask);
            __mmask16 zero_mask = static_cast<__mmask16>(~one_mask);
            _mm512_mask_compressstoreu_epi32(
                static_cast<void*>(output + zero_position),
                zero_mask,
                values
            );
            _mm512_mask_compressstoreu_epi32(
                static_cast<void*>(output + one_position),
                one_mask,
                values
            );
            int ones = std::popcount(static_cast<unsigned int>(one_mask));
            zero_position += lanes - ones;
            one_position += ones;
        }
        for(; i < length; i++){
            U value = input[static_cast<std::size_t>(i)];
            if(bit_at(value, bit)){
                output[static_cast<std::size_t>(one_position++)] = value;
            }
            else{
                output[static_cast<std::size_t>(zero_position++)] = value;
            }
        }
    }

    __attribute__((target("avx512f")))
    static void stable_partition_avx512_64(
        const U* input,
        U* output,
        int length,
        int bit,
        int zero_count_value
    ){
        constexpr int lanes = 8;
        int zero_position = 0;
        int one_position = zero_count_value;
        int i = 0;
        const __m512i bit_mask = _mm512_set1_epi64(
            static_cast<long long>(std::uint64_t{1} << bit)
        );
        for(; i + lanes <= length; i += lanes){
            __m512i values = _mm512_loadu_si512(
                static_cast<const void*>(input + i)
            );
            __mmask8 one_mask = _mm512_test_epi64_mask(values, bit_mask);
            __mmask8 zero_mask = static_cast<__mmask8>(~one_mask);
            _mm512_mask_compressstoreu_epi64(
                static_cast<void*>(output + zero_position),
                zero_mask,
                values
            );
            _mm512_mask_compressstoreu_epi64(
                static_cast<void*>(output + one_position),
                one_mask,
                values
            );
            int ones = std::popcount(static_cast<unsigned int>(one_mask));
            zero_position += lanes - ones;
            one_position += ones;
        }
        for(; i < length; i++){
            U value = input[static_cast<std::size_t>(i)];
            if(bit_at(value, bit)){
                output[static_cast<std::size_t>(one_position++)] = value;
            }
            else{
                output[static_cast<std::size_t>(zero_position++)] = value;
            }
        }
    }
#endif

    static void stable_partition_values(
        const std::vector<U>& input,
        std::vector<U>& output,
        int bit,
        int zero_count_value
    ){
#if DYNAMIC_WAVELET_MATRIX_X86 && (defined(__GNUC__) || defined(__clang__))
        // AVX-512 compress-store is useful only for sufficiently large levels.
        if(input.size() >= 1024 && runtime_has_avx512f()){
            if constexpr(sizeof(U) == 4){
                stable_partition_avx512_32(
                    input.data(),
                    output.data(),
                    static_cast<int>(input.size()),
                    bit,
                    zero_count_value
                );
                return;
            }
            else if constexpr(sizeof(U) == 8){
                stable_partition_avx512_64(
                    input.data(),
                    output.data(),
                    static_cast<int>(input.size()),
                    bit,
                    zero_count_value
                );
                return;
            }
        }
#endif
        int zero_position = 0;
        int one_position = zero_count_value;
        for(U value : input){
            if(bit_at(value, bit)){
                output[static_cast<std::size_t>(one_position++)] = value;
            }
            else{
                output[static_cast<std::size_t>(zero_position++)] = value;
            }
        }
    }

    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void erase_encoded(int position, U value){
        for(int level = 0; level < BIT_WIDTH; level++){
            int ones_before = bit_vectors[static_cast<std::size_t>(level)].rank1(position);
            bool bit = bit_at(value, BIT_WIDTH - 1 - level);

            int next_position;
            if(bit){
                next_position = zero_count[static_cast<std::size_t>(level)] + ones_before;
            }
            else{
                next_position = position - ones_before;
            }

            auto [erased, erased_weight] = bit_vectors[static_cast<std::size_t>(level)].erase(position);
            if(erased != bit || erased_weight != decode(value)){
                throw std::logic_error("DynamicWaveletMatrix erase inconsistency.");
            }
            if(!bit){
                zero_count[static_cast<std::size_t>(level)]--;
            }
            position = next_position;
        }
    }

    void insert_encoded(int position, U value){
        for(int level = 0; level < BIT_WIDTH; level++){
            int ones_before = bit_vectors[static_cast<std::size_t>(level)].rank1(position);
            bool bit = bit_at(value, BIT_WIDTH - 1 - level);

            int next_position;
            if(bit){
                next_position = zero_count[static_cast<std::size_t>(level)] + ones_before;
            }
            else{
                next_position = position - ones_before;
            }

            bit_vectors[static_cast<std::size_t>(level)].insert(position, bit, decode(value));
            if(!bit){
                zero_count[static_cast<std::size_t>(level)]++;
            }
            position = next_position;
        }
    }

    int count_less_encoded(int l, int r, U upper) const{
        int result = 0;

        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto ranks = bits.rank_pair(l, r);
            int ones_l = ranks.ones_l;
            int ones_r = ranks.ones_r;
            int zeros_l = l - ones_l;
            int zeros_r = r - ones_r;
            bool bit = bit_at(upper, BIT_WIDTH - 1 - level);

            if(bit){
                result += zeros_r - zeros_l;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }
            else{
                l = zeros_l;
                r = zeros_r;
            }
        }

        return result;
    }

    int count_less_value(int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return r - l;
        return count_less_encoded(l, r, key);
    }

    std::pair<int, sum_type> count_and_sum_less_encoded(
        int l,
        int r,
        U upper
    ) const{
        int count = 0;
        sum_type sum = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            bool bit = bit_at(upper, BIT_WIDTH - 1 - level);
            if(bit){
                auto stats = bits.range_stats(l, r);
                int zeros_l = l - stats.ones_l;
                int zeros_r = r - stats.ones_r;
                count += zeros_r - zeros_l;
                sum += stats.zero_sum;
                l = zero_count[static_cast<std::size_t>(level)] + stats.ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + stats.ones_r;
            }
            else{
                auto ranks = bits.rank_pair(l, r);
                l -= ranks.ones_l;
                r -= ranks.ones_r;
            }
        }
        return {count, sum};
    }

    std::pair<int, sum_type> count_and_sum_less_value(
        int l,
        int r,
        T upper
    ) const{
        U key = encode(upper);
        if(!fits(key)) return {r - l, sum_all_unchecked(l, r)};
        return count_and_sum_less_encoded(l, r, key);
    }

    sum_type sum_all_unchecked(int l, int r) const{
        if(_n == 0) return 0;
        return bit_vectors[0].sum_all(l, r);
    }

    void rebuild_distinct_cache() const{
        if(!distinct_cache_dirty) return;
        distinct_cache.clear();
        distinct_cache.reserve(global_frequency.size());
        for(const auto& [value, frequency] : global_frequency){
            if(frequency > 0) distinct_cache.push_back(value);
        }
        distinct_cache_dirty = false;
    }

    void list_frequencies_encoded_recursive(
        int level,
        int l,
        int r,
        U prefix,
        std::vector<std::pair<T, int>>& result
    ) const{
        if(l == r) return;
        if(level == BIT_WIDTH){
            result.emplace_back(decode(prefix), r - l);
            return;
        }
        const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
        auto ranks = bits.rank_pair(l, r);
        int ones_l = ranks.ones_l;
        int ones_r = ranks.ones_r;
        int zeros_l = l - ones_l;
        int zeros_r = r - ones_r;
        list_frequencies_encoded_recursive(
            level + 1,
            zeros_l,
            zeros_r,
            prefix,
            result
        );
        list_frequencies_encoded_recursive(
            level + 1,
            zero_count[static_cast<std::size_t>(level)] + ones_l,
            zero_count[static_cast<std::size_t>(level)] + ones_r,
            prefix | (U{1} << (BIT_WIDTH - 1 - level)),
            result
        );
    }

    void list_frequencies_encoded_range_recursive(
        int level,
        int l,
        int r,
        U prefix,
        const T& lower,
        const T& upper,
        std::vector<std::pair<T, int>>& result
    ) const{
        if(l == r) return;
        U suffix_mask;
        int remaining_bits = BIT_WIDTH - level;
        if(remaining_bits == digits){
            suffix_mask = std::numeric_limits<U>::max();
        }
        else if(remaining_bits == 0){
            suffix_mask = U{0};
        }
        else{
            suffix_mask = (U{1} << remaining_bits) - 1;
        }
        T subtree_min = decode(prefix);
        T subtree_max = decode(prefix | suffix_mask);
        if(subtree_max < lower || !(subtree_min < upper)) return;

        if(level == BIT_WIDTH){
            result.emplace_back(subtree_min, r - l);
            return;
        }
        const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
        auto ranks = bits.rank_pair(l, r);
        int zeros_l = l - ranks.ones_l;
        int zeros_r = r - ranks.ones_r;
        list_frequencies_encoded_range_recursive(
            level + 1,
            zeros_l,
            zeros_r,
            prefix,
            lower,
            upper,
            result
        );
        list_frequencies_encoded_range_recursive(
            level + 1,
            zero_count[static_cast<std::size_t>(level)] + ranks.ones_l,
            zero_count[static_cast<std::size_t>(level)] + ranks.ones_r,
            prefix | (U{1} << (BIT_WIDTH - 1 - level)),
            lower,
            upper,
            result
        );
    }

    void intersect_encoded_recursive(
        int level,
        int l1,
        int r1,
        int l2,
        int r2,
        U prefix,
        std::vector<std::tuple<T, int, int>>& result
    ) const{
        if(l1 == r1 || l2 == r2) return;
        if(level == BIT_WIDTH){
            result.emplace_back(decode(prefix), r1 - l1, r2 - l2);
            return;
        }
        const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
        auto first = bits.rank_pair(l1, r1);
        auto second = bits.rank_pair(l2, r2);

        intersect_encoded_recursive(
            level + 1,
            l1 - first.ones_l,
            r1 - first.ones_r,
            l2 - second.ones_l,
            r2 - second.ones_r,
            prefix,
            result
        );
        intersect_encoded_recursive(
            level + 1,
            zero_count[static_cast<std::size_t>(level)] + first.ones_l,
            zero_count[static_cast<std::size_t>(level)] + first.ones_r,
            zero_count[static_cast<std::size_t>(level)] + second.ones_l,
            zero_count[static_cast<std::size_t>(level)] + second.ones_r,
            prefix | (U{1} << (BIT_WIDTH - 1 - level)),
            result
        );
    }

    static constexpr unsigned long long bitwise_value_mask(){
        if constexpr(BIT_WIDTH == 64){
            return std::numeric_limits<unsigned long long>::max();
        }
        else{
            return (1ULL << BIT_WIDTH) - 1ULL;
        }
    }

    static unsigned long long normalize_bitwise(unsigned long long value){
        return value & bitwise_value_mask();
    }

    static bool bitwise_value_fits(unsigned long long value){
        if constexpr(BIT_WIDTH == 64) return true;
        else return value < (1ULL << BIT_WIDTH);
    }

    static bool bitwise_upper_covers_domain(unsigned long long upper){
        if constexpr(BIT_WIDTH == 64) return false;
        else return upper >= (1ULL << BIT_WIDTH);
    }

    static bool transformed_bit(
        bool encoded_bit,
        int bit,
        unsigned long long mask,
        BitwiseOperation operation
    ){
        bool raw_bit = encoded_bit ^ static_cast<bool>((sign_mask >> bit) & U{1});
        bool mask_bit = static_cast<bool>((mask >> bit) & 1ULL);
        switch(operation){
            case BitwiseOperation::Xor: return raw_bit ^ mask_bit;
            case BitwiseOperation::Or: return raw_bit | mask_bit;
            case BitwiseOperation::And: return raw_bit & mask_bit;
        }
        throw std::logic_error("DynamicWaveletMatrix invalid bitwise operation.");
    }

    struct BitwiseState{
        int l;
        int r;
    };

    BitwiseState child_state(
        int level,
        const BitwiseState& state,
        bool encoded_bit
    ) const{
        const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
        auto ranks = bits.rank_pair(state.l, state.r);
        int ones_l = ranks.ones_l;
        int ones_r = ranks.ones_r;
        if(encoded_bit){
            return {
                zero_count[static_cast<std::size_t>(level)] + ones_l,
                zero_count[static_cast<std::size_t>(level)] + ones_r
            };
        }
        return {state.l - ones_l, state.r - ones_r};
    }

    int count_less_xor_unchecked(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper
    ) const{
        if(upper == 0) return 0;
        if(bitwise_upper_covers_domain(upper)) return r - l;
        unsigned long long effective_mask = normalize_bitwise(mask)
            ^ normalize_bitwise(static_cast<unsigned long long>(sign_mask));
        int result = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            int bit = BIT_WIDTH - 1 - level;
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto ranks = bits.rank_pair(l, r);
            std::array<BitwiseState, 2> children{{
                {l - ranks.ones_l, r - ranks.ones_r},
                {
                    zero_count[static_cast<std::size_t>(level)] + ranks.ones_l,
                    zero_count[static_cast<std::size_t>(level)] + ranks.ones_r
                }
            }};
            bool output_zero_encoded_bit = static_cast<bool>(
                (effective_mask >> bit) & 1ULL
            );
            bool upper_bit = static_cast<bool>((upper >> bit) & 1ULL);
            if(upper_bit){
                const auto& zero_child = children[
                    static_cast<std::size_t>(output_zero_encoded_bit)
                ];
                result += zero_child.r - zero_child.l;
                const auto& next_child = children[
                    static_cast<std::size_t>(!output_zero_encoded_bit)
                ];
                l = next_child.l;
                r = next_child.r;
            }
            else{
                const auto& next_child = children[
                    static_cast<std::size_t>(output_zero_encoded_bit)
                ];
                l = next_child.l;
                r = next_child.r;
            }
        }
        return result;
    }

    unsigned long long kth_smallest_xor_unchecked(
        int l,
        int r,
        int k,
        unsigned long long mask
    ) const{
        unsigned long long effective_mask = normalize_bitwise(mask)
            ^ normalize_bitwise(static_cast<unsigned long long>(sign_mask));
        unsigned long long result = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            int bit = BIT_WIDTH - 1 - level;
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto ranks = bits.rank_pair(l, r);
            std::array<BitwiseState, 2> children{{
                {l - ranks.ones_l, r - ranks.ones_r},
                {
                    zero_count[static_cast<std::size_t>(level)] + ranks.ones_l,
                    zero_count[static_cast<std::size_t>(level)] + ranks.ones_r
                }
            }};
            bool output_zero_encoded_bit = static_cast<bool>(
                (effective_mask >> bit) & 1ULL
            );
            const auto& zero_child = children[
                static_cast<std::size_t>(output_zero_encoded_bit)
            ];
            int zero_frequency = zero_child.r - zero_child.l;
            if(k < zero_frequency){
                l = zero_child.l;
                r = zero_child.r;
            }
            else{
                k -= zero_frequency;
                result |= 1ULL << bit;
                const auto& one_child = children[
                    static_cast<std::size_t>(!output_zero_encoded_bit)
                ];
                l = one_child.l;
                r = one_child.r;
            }
        }
        return result;
    }

    std::pair<int, sum_type> count_and_sum_less_bitwise_unchecked(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        if(upper == 0) return {0, 0};
        if(bitwise_upper_covers_domain(upper)){
            return {r - l, sum_all_unchecked(l, r)};
        }
        mask = normalize_bitwise(mask);
        std::vector<BitwiseState> states{{l, r}};
        std::vector<BitwiseState> next;
        int result_count = 0;
        sum_type result_sum = 0;
        for(int level = 0; level < BIT_WIDTH && !states.empty(); level++){
            int bit = BIT_WIDTH - 1 - level;
            bool upper_bit = static_cast<bool>((upper >> bit) & 1ULL);
            next.clear();
            for(const BitwiseState& state : states){
                const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
                if(upper_bit){
                    auto stats = bits.range_stats(state.l, state.r);
                    std::array<BitwiseState, 2> children{{
                        {state.l - stats.ones_l, state.r - stats.ones_r},
                        {
                            zero_count[static_cast<std::size_t>(level)] + stats.ones_l,
                            zero_count[static_cast<std::size_t>(level)] + stats.ones_r
                        }
                    }};
                    for(int encoded_bit_int = 0; encoded_bit_int < 2; encoded_bit_int++){
                        bool encoded_bit = encoded_bit_int != 0;
                        const BitwiseState& child = children[static_cast<std::size_t>(encoded_bit_int)];
                        int count = child.r - child.l;
                        if(count == 0) continue;
                        bool output_bit = transformed_bit(encoded_bit, bit, mask, operation);
                        if(!output_bit){
                            result_count += count;
                            result_sum += encoded_bit ? stats.one_sum : stats.zero_sum;
                        }
                        else{
                            next.push_back(child);
                        }
                    }
                }
                else{
                    auto ranks = bits.rank_pair(state.l, state.r);
                    std::array<BitwiseState, 2> children{{
                        {state.l - ranks.ones_l, state.r - ranks.ones_r},
                        {
                            zero_count[static_cast<std::size_t>(level)] + ranks.ones_l,
                            zero_count[static_cast<std::size_t>(level)] + ranks.ones_r
                        }
                    }};
                    for(int encoded_bit_int = 0; encoded_bit_int < 2; encoded_bit_int++){
                        bool encoded_bit = encoded_bit_int != 0;
                        const BitwiseState& child = children[static_cast<std::size_t>(encoded_bit_int)];
                        if(child.l == child.r) continue;
                        if(!transformed_bit(encoded_bit, bit, mask, operation)){
                            next.push_back(child);
                        }
                    }
                }
            }
            states.swap(next);
        }
        return {result_count, result_sum};
    }

    unsigned long long kth_smallest_bitwise_unchecked(
        int l,
        int r,
        int k,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        mask = normalize_bitwise(mask);
        std::vector<BitwiseState> states{{l, r}};
        std::vector<BitwiseState> zero_states;
        std::vector<BitwiseState> one_states;
        unsigned long long result = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            int bit = BIT_WIDTH - 1 - level;
            zero_states.clear();
            one_states.clear();
            int zero_frequency = 0;
            for(const BitwiseState& state : states){
                const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
                auto ranks = bits.rank_pair(state.l, state.r);
                std::array<BitwiseState, 2> children{{
                    {state.l - ranks.ones_l, state.r - ranks.ones_r},
                    {
                        zero_count[static_cast<std::size_t>(level)] + ranks.ones_l,
                        zero_count[static_cast<std::size_t>(level)] + ranks.ones_r
                    }
                }};
                for(int encoded_bit_int = 0; encoded_bit_int < 2; encoded_bit_int++){
                    bool encoded_bit = encoded_bit_int != 0;
                    const BitwiseState& child = children[static_cast<std::size_t>(encoded_bit_int)];
                    if(child.l == child.r) continue;
                    bool output_bit = transformed_bit(encoded_bit, bit, mask, operation);
                    if(output_bit){
                        one_states.push_back(child);
                    }
                    else{
                        zero_frequency += child.r - child.l;
                        zero_states.push_back(child);
                    }
                }
            }
            if(k < zero_frequency){
                states.swap(zero_states);
            }
            else{
                k -= zero_frequency;
                result |= 1ULL << bit;
                states.swap(one_states);
            }
        }
        return result;
    }

    static T raw_to_value(U raw){
        if constexpr(std::is_signed_v<T>){
            return std::bit_cast<T>(raw);
        }
        else{
            return static_cast<T>(raw);
        }
    }

    static unsigned long long apply_bitwise_value(
        U raw,
        unsigned long long mask,
        BitwiseOperation operation
    ){
        unsigned long long value = static_cast<unsigned long long>(raw) & bitwise_value_mask();
        mask = normalize_bitwise(mask);
        switch(operation){
            case BitwiseOperation::Xor: return (value ^ mask) & bitwise_value_mask();
            case BitwiseOperation::Or: return (value | mask) & bitwise_value_mask();
            case BitwiseOperation::And: return (value & mask) & bitwise_value_mask();
        }
        throw std::logic_error("DynamicWaveletMatrix invalid bitwise operation.");
    }

public:
    DynamicWaveletMatrix() = default;

    explicit DynamicWaveletMatrix(const std::vector<T>& sequence):
        _n(static_cast<int>(sequence.size())),
        encoded_values(sequence.size()),
        original_values(sequence){
        if(sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        std::vector<U> current(sequence.size());
        for(int i = 0; i < _n; i++){
            U value = encode_checked(
                sequence[static_cast<std::size_t>(i)],
                "library assertion fault: bit width violation (constructor)."
            );
            encoded_values[static_cast<std::size_t>(i)] = value;
            current[static_cast<std::size_t>(i)] = value;
            global_frequency[sequence[static_cast<std::size_t>(i)]]++;
        }

        std::vector<U> next(sequence.size());
        std::vector<unsigned char> bits(sequence.size());
        std::vector<T> level_weights(sequence.size());

        for(int level = 0; level < BIT_WIDTH; level++){
            int bit = BIT_WIDTH - 1 - level;
            int zeros = 0;

            for(int i = 0; i < _n; i++){
                bool value = bit_at(current[static_cast<std::size_t>(i)], bit);
                bits[static_cast<std::size_t>(i)] = static_cast<unsigned char>(value);
                level_weights[static_cast<std::size_t>(i)] = decode(current[static_cast<std::size_t>(i)]);
                if(!value) zeros++;
            }

            zero_count[static_cast<std::size_t>(level)] = zeros;
            bit_vectors[static_cast<std::size_t>(level)].assign(bits, level_weights);

            stable_partition_values(current, next, bit, zeros);
            current.swap(next);
        }
    }

    template<std::size_t N>
    explicit DynamicWaveletMatrix(const std::array<T, N>& sequence):
        DynamicWaveletMatrix(std::vector<T>(sequence.begin(), sequence.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    DynamicWaveletMatrix(const DynamicWaveletMatrix&) = default;
    DynamicWaveletMatrix& operator=(const DynamicWaveletMatrix&) = default;
    DynamicWaveletMatrix(DynamicWaveletMatrix&&) noexcept = default;
    DynamicWaveletMatrix& operator=(DynamicWaveletMatrix&&) noexcept = default;

    void build(const std::vector<T>& data){
        *this = DynamicWaveletMatrix(data);
    }

    int size() const{
        return _n;
    }

    bool empty() const{
        return _n == 0;
    }

    int distinct_size() const{
        return static_cast<int>(global_frequency.size());
    }

    int bit_size() const{
        return _n == 0 ? 0 : BIT_WIDTH;
    }

    std::vector<T> values() const{
        rebuild_distinct_cache();
        return distinct_cache;
    }

    bool contains(const T& value) const{
        return global_frequency.find(value) != global_frequency.end();
    }

    int index_of(const T& value) const{
        rebuild_distinct_cache();
        auto iterator = std::lower_bound(distinct_cache.begin(), distinct_cache.end(), value);
        if(iterator == distinct_cache.end() || *iterator != value) return -1;
        return static_cast<int>(iterator - distinct_cache.begin());
    }

    T access(int k) const{
        check_index(k, "library assertion fault: range violation (access).");
        return original_values[static_cast<std::size_t>(k)];
    }

    T operator[](int k) const{
        return access(k);
    }

    void set(int k, T value){
        check_index(k, "library assertion fault: range violation (set).");
        U encoded = encode_checked(
            value,
            "library assertion fault: bit width violation (set)."
        );
        U old = encoded_values[static_cast<std::size_t>(k)];
        if(old == encoded) return;

        T old_value = original_values[static_cast<std::size_t>(k)];
        erase_encoded(k, old);
        insert_encoded(k, encoded);
        encoded_values[static_cast<std::size_t>(k)] = encoded;
        original_values[static_cast<std::size_t>(k)] = value;

        auto iterator = global_frequency.find(old_value);
        if(iterator == global_frequency.end()){
            throw std::logic_error("DynamicWaveletMatrix frequency inconsistency.");
        }
        if(--iterator->second == 0) global_frequency.erase(iterator);
        global_frequency[value]++;
        distinct_cache_dirty = true;
    }

    int rank(T value, int r) const{
        return rank(value, 0, r);
    }

    int rank(T value, int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (rank).");
        U key = encode(value);
        if(!fits(key)) return 0;

        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto ranks = bits.rank_pair(l, r);
            int ones_l = ranks.ones_l;
            int ones_r = ranks.ones_r;
            bool bit = bit_at(key, BIT_WIDTH - 1 - level);

            if(bit){
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }
            else{
                l -= ones_l;
                r -= ones_r;
            }
        }

        return r - l;
    }

    int select(T value, int k) const{
        if(k < 0) return -1;

        U key = encode(value);
        if(!fits(key)) return -1;

        int l = 0;
        int r = _n;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto ranks = bits.rank_pair(l, r);
            int ones_l = ranks.ones_l;
            int ones_r = ranks.ones_r;
            bool bit = bit_at(key, BIT_WIDTH - 1 - level);

            if(bit){
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }
            else{
                l -= ones_l;
                r -= ones_r;
            }
        }

        if(r - l <= k) return -1;

        int position = l + k;
        for(int level = BIT_WIDTH - 1; level >= 0; level--){
            bool bit = bit_at(key, BIT_WIDTH - 1 - level);
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];

            if(bit){
                position = bits.select1(
                    position - zero_count[static_cast<std::size_t>(level)]
                );
            }
            else{
                position = bits.select0(position);
            }
        }

        return position;
    }

    T kth_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_smallest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_smallest)."
            );
        }

        U value = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto ranks = bits.rank_pair(l, r);
            int ones_l = ranks.ones_l;
            int ones_r = ranks.ones_r;
            int zeros_l = l - ones_l;
            int zeros_r = r - ones_r;
            int zero_frequency = zeros_r - zeros_l;

            if(k < zero_frequency){
                l = zeros_l;
                r = zeros_r;
            }
            else{
                k -= zero_frequency;
                value |= U{1} << (BIT_WIDTH - 1 - level);
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }
        }

        return decode(value);
    }

    T kth_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_largest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_largest)."
            );
        }
        return kth_smallest(l, r, r - l - 1 - k);
    }

    int range_freq(int l, int r, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        return count_less_value(l, r, upper);
    }

    int range_freq(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (range_freq)."
            );
        }
        return count_less_value(l, r, upper) - count_less_value(l, r, lower);
    }

    std::optional<T> prev_value(int l, int r, T upper) const{
        int count = range_freq(l, r, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(l, r, count - 1);
    }

    std::optional<T> next_value(int l, int r, T lower) const{
        int count = range_freq(l, r, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest(l, r, count);
    }

    int count(const T& value) const{
        return rank(value, _n);
    }

    int count(const T& value, int l, int r) const{
        return rank(value, l, r);
    }

    T quantile(int l, int r, int k) const{
        return kth_smallest(l, r, k);
    }

    std::optional<T> min_value(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (min_value).");
        if(l == r) return std::nullopt;
        return kth_smallest(l, r, 0);
    }

    std::optional<T> max_value(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (max_value).");
        if(l == r) return std::nullopt;
        return kth_largest(l, r, 0);
    }

    std::optional<T> median_lower(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (median_lower).");
        if(l == r) return std::nullopt;
        return kth_smallest(l, r, (r - l - 1) / 2);
    }

    std::optional<T> median_upper(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (median_upper).");
        if(l == r) return std::nullopt;
        return kth_smallest(l, r, (r - l) / 2);
    }

    int count_less(int l, int r, const T& upper) const{
        return range_freq(l, r, upper);
    }

    int count_less_equal(int l, int r, const T& upper) const{
        check_range(l, r, "library assertion fault: range violation (count_less_equal).");
        if(upper == std::numeric_limits<T>::max()) return r - l;
        return count_less_value(l, r, static_cast<T>(upper + T{1}));
    }

    int count_greater(int l, int r, const T& lower) const{
        check_range(l, r, "library assertion fault: range violation (count_greater).");
        return (r - l) - count_less_equal(l, r, lower);
    }

    int count_greater_equal(int l, int r, const T& lower) const{
        check_range(l, r, "library assertion fault: range violation (count_greater_equal).");
        return (r - l) - count_less_value(l, r, lower);
    }

    std::optional<T> next_value_ge(int l, int r, const T& lower) const{
        return next_value(l, r, lower);
    }

    std::optional<T> next_value_gt(int l, int r, const T& lower) const{
        int count = count_less_equal(l, r, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest(l, r, count);
    }

    std::optional<T> prev_value_lt(int l, int r, const T& upper) const{
        return prev_value(l, r, upper);
    }

    std::optional<T> prev_value_le(int l, int r, const T& upper) const{
        int count = count_less_equal(l, r, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(l, r, count - 1);
    }

    std::pair<int, sum_type> count_and_sum_less(
        int l,
        int r,
        const T& upper
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_less).");
        return count_and_sum_less_value(l, r, upper);
    }

    std::pair<int, sum_type> count_and_sum_less_equal(
        int l,
        int r,
        const T& upper
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_less_equal).");
        if(upper == std::numeric_limits<T>::max()){
            return {r - l, sum_all_unchecked(l, r)};
        }
        return count_and_sum_less_value(l, r, static_cast<T>(upper + T{1}));
    }

    std::pair<int, sum_type> count_and_sum_greater(
        int l,
        int r,
        const T& lower
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_greater).");
        auto excluded = count_and_sum_less_equal(l, r, lower);
        return {r - l - excluded.first, sum_all_unchecked(l, r) - excluded.second};
    }

    std::pair<int, sum_type> count_and_sum_greater_equal(
        int l,
        int r,
        const T& lower
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_greater_equal).");
        auto excluded = count_and_sum_less_value(l, r, lower);
        return {r - l - excluded.first, sum_all_unchecked(l, r) - excluded.second};
    }

    sum_type sum_all(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (sum_all).");
        return sum_all_unchecked(l, r);
    }

    sum_type sum_less(int l, int r, const T& upper) const{
        return count_and_sum_less(l, r, upper).second;
    }

    sum_type sum_less_equal(int l, int r, const T& upper) const{
        return count_and_sum_less_equal(l, r, upper).second;
    }

    sum_type sum_range(int l, int r, const T& lower, const T& upper) const{
        check_range(l, r, "library assertion fault: range violation (sum_range).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (sum_range).");
        }
        return count_and_sum_less_value(l, r, upper).second
            - count_and_sum_less_value(l, r, lower).second;
    }

    sum_type sum_equal(int l, int r, const T& value) const{
        return static_cast<sum_type>(value) * rank(value, l, r);
    }

    sum_type sum_greater(int l, int r, const T& lower) const{
        return count_and_sum_greater(l, r, lower).second;
    }

    sum_type sum_greater_equal(int l, int r, const T& lower) const{
        return count_and_sum_greater_equal(l, r, lower).second;
    }

    sum_type sum_k_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_smallest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (sum_k_smallest).");
        }
        if(k == 0) return 0;
        if(k == r - l) return sum_all_unchecked(l, r);

        sum_type result = 0;
        U value = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            int ones_l = stats.ones_l;
            int ones_r = stats.ones_r;
            int zeros_l = l - ones_l;
            int zeros_r = r - ones_r;
            int zero_frequency = zeros_r - zeros_l;
            if(k <= zero_frequency){
                l = zeros_l;
                r = zeros_r;
            }
            else{
                result += stats.zero_sum;
                k -= zero_frequency;
                value |= U{1} << (BIT_WIDTH - 1 - level);
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }
        }
        result += static_cast<sum_type>(decode(value)) * k;
        return result;
    }

    sum_type sum_k_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_largest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (sum_k_largest).");
        }
        return sum_all_unchecked(l, r) - sum_k_smallest(l, r, r - l - k);
    }

    int min_count_for_sum_smallest_ge(int l, int r, sum_type x) const{
        check_range(
            l,
            r,
            "library assertion fault: range violation (min_count_for_sum_smallest_ge)."
        );
        if(x <= 0) return 0;

        int selected = 0;
        sum_type accumulated = 0;
        const sum_type target = x;
        U value = 0;

        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            int ones_l = stats.ones_l;
            int ones_r = stats.ones_r;
            int zeros_l = l - ones_l;
            int zeros_r = r - ones_r;
            int zero_frequency = zeros_r - zeros_l;
            sum_type zero_sum = stats.zero_sum;

            if(accumulated + zero_sum >= target){
                l = zeros_l;
                r = zeros_r;
            }
            else{
                accumulated += zero_sum;
                selected += zero_frequency;
                value |= U{1} << (BIT_WIDTH - 1 - level);
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }
        }

        const sum_type leaf_value = static_cast<sum_type>(decode(value));
        if(leaf_value <= 0) return -1;

        const sum_type required = target - accumulated;
        const sum_type take = required / leaf_value + (required % leaf_value != 0);
        if(take <= 0) return selected;
        if(take > static_cast<sum_type>(r - l)) return -1;
        return selected + static_cast<int>(take);
    }

    int min_count_for_sum_largest_ge(int l, int r, sum_type x) const{
        check_range(
            l,
            r,
            "library assertion fault: range violation (min_count_for_sum_largest_ge)."
        );
        if(x <= 0) return 0;

        int selected = 0;
        sum_type accumulated = 0;
        const sum_type target = x;
        U value = 0;

        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            int ones_l = stats.ones_l;
            int ones_r = stats.ones_r;
            int one_frequency = ones_r - ones_l;
            sum_type one_sum = stats.one_sum;

            if(accumulated + one_sum >= target){
                value |= U{1} << (BIT_WIDTH - 1 - level);
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }
            else{
                accumulated += one_sum;
                selected += one_frequency;
                l -= ones_l;
                r -= ones_r;
            }
        }

        const sum_type leaf_value = static_cast<sum_type>(decode(value));
        if(leaf_value <= 0) return -1;

        const sum_type required = target - accumulated;
        const sum_type take = required / leaf_value + (required % leaf_value != 0);
        if(take <= 0) return selected;
        if(take > static_cast<sum_type>(r - l)) return -1;
        return selected + static_cast<int>(take);
    }

    std::vector<std::pair<T, int>> list_frequencies(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (list_frequencies).");
        std::vector<std::pair<T, int>> result;
        list_frequencies_encoded_recursive(0, l, r, U{0}, result);
        return result;
    }

    std::vector<std::pair<T, int>> list_frequencies(
        int l,
        int r,
        const T& lower,
        const T& upper
    ) const{
        check_range(l, r, "library assertion fault: range violation (list_frequencies).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (list_frequencies).");
        }
        std::vector<std::pair<T, int>> result;
        list_frequencies_encoded_range_recursive(
            0, l, r, U{0}, lower, upper, result
        );
        return result;
    }

    std::vector<T> distinct_values(int l, int r) const{
        auto frequencies = list_frequencies(l, r);
        std::vector<T> result;
        result.reserve(frequencies.size());
        for(const auto& [value, frequency] : frequencies){
            static_cast<void>(frequency);
            result.push_back(value);
        }
        return result;
    }

    std::vector<std::pair<T, int>> top_k_frequent(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (top_k_frequent).");
        if(k <= 0 || l == r) return {};

        struct State{
            int level;
            int l;
            int r;
            U prefix;
        };
        struct Compare{
            bool operator()(const State& left, const State& right) const{
                int left_frequency = left.r - left.l;
                int right_frequency = right.r - right.l;
                if(left_frequency != right_frequency){
                    return left_frequency < right_frequency;
                }
                return left.prefix > right.prefix;
            }
        };

        std::priority_queue<State, std::vector<State>, Compare> queue;
        queue.push({0, l, r, U{0}});
        std::vector<std::pair<T, int>> result;
        result.reserve(static_cast<std::size_t>(std::min(k, r - l)));

        while(!queue.empty() && static_cast<int>(result.size()) < k){
            State state = queue.top();
            queue.pop();
            if(state.level == BIT_WIDTH){
                result.emplace_back(decode(state.prefix), state.r - state.l);
                continue;
            }

            const auto& bits = bit_vectors[static_cast<std::size_t>(state.level)];
            auto ranks = bits.rank_pair(state.l, state.r);
            int zeros_l = state.l - ranks.ones_l;
            int zeros_r = state.r - ranks.ones_r;
            if(zeros_l < zeros_r){
                queue.push({state.level + 1, zeros_l, zeros_r, state.prefix});
            }
            int ones_l = zero_count[static_cast<std::size_t>(state.level)] + ranks.ones_l;
            int ones_r = zero_count[static_cast<std::size_t>(state.level)] + ranks.ones_r;
            if(ones_l < ones_r){
                queue.push({
                    state.level + 1,
                    ones_l,
                    ones_r,
                    state.prefix | (U{1} << (BIT_WIDTH - 1 - state.level))
                });
            }
        }

        std::sort(result.begin(), result.end(), [](const auto& left, const auto& right){
            if(left.second != right.second) return left.second > right.second;
            return left.first < right.first;
        });
        return result;
    }

    std::optional<std::pair<T, int>> mode(int l, int r) const{
        auto result = top_k_frequent(l, r, 1);
        if(result.empty()) return std::nullopt;
        return result.front();
    }

    std::vector<std::tuple<T, int, int>> intersect(
        int l1,
        int r1,
        int l2,
        int r2
    ) const{
        check_range(l1, r1, "library assertion fault: range violation (intersect).");
        check_range(l2, r2, "library assertion fault: range violation (intersect).");
        std::vector<std::tuple<T, int, int>> result;
        intersect_encoded_recursive(0, l1, r1, l2, r2, U{0}, result);
        return result;
    }

    int count_less_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_less_bitwise).");
        return count_and_sum_less_bitwise_unchecked(l, r, mask, upper, operation).first;
    }

    int range_freq_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (range_freq_bitwise).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_freq_bitwise).");
        }
        return count_less_bitwise(l, r, mask, upper, operation)
            - count_less_bitwise(l, r, mask, lower, operation);
    }

    int count_less_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_less_equal_bitwise).");
        if(!bitwise_value_fits(upper) || upper == bitwise_value_mask()) return r - l;
        return count_less_bitwise(l, r, mask, upper + 1, operation);
    }

    int count_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long value,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_equal_bitwise).");
        if(!bitwise_value_fits(value)) return 0;
        int less = count_less_bitwise(l, r, mask, value, operation);
        int less_equal = value == bitwise_value_mask()
            ? r - l
            : count_less_bitwise(l, r, mask, value + 1, operation);
        return less_equal - less;
    }

    int count_greater_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        BitwiseOperation operation
    ) const{
        return (r - l) - count_less_equal_bitwise(l, r, mask, lower, operation);
    }

    int count_greater_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_greater_equal_bitwise).");
        return (r - l) - count_less_bitwise(l, r, mask, lower, operation);
    }

    std::pair<int, sum_type> count_and_sum_less_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_less_bitwise).");
        return count_and_sum_less_bitwise_unchecked(l, r, mask, upper, operation);
    }

    std::pair<int, sum_type> count_and_sum_less_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_less_equal_bitwise).");
        if(!bitwise_value_fits(upper) || upper == bitwise_value_mask()){
            return {r - l, sum_all_unchecked(l, r)};
        }
        return count_and_sum_less_bitwise_unchecked(l, r, mask, upper + 1, operation);
    }

    std::pair<int, sum_type> count_and_sum_range_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_range_bitwise).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (count_and_sum_range_bitwise).");
        }
        auto high = count_and_sum_less_bitwise_unchecked(l, r, mask, upper, operation);
        auto low = count_and_sum_less_bitwise_unchecked(l, r, mask, lower, operation);
        return {high.first - low.first, high.second - low.second};
    }

    std::pair<int, sum_type> count_and_sum_greater_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_greater_bitwise).");
        auto excluded = count_and_sum_less_equal_bitwise(l, r, mask, lower, operation);
        return {r - l - excluded.first, sum_all_unchecked(l, r) - excluded.second};
    }

    std::pair<int, sum_type> count_and_sum_greater_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (count_and_sum_greater_equal_bitwise).");
        auto excluded = count_and_sum_less_bitwise_unchecked(l, r, mask, lower, operation);
        return {r - l - excluded.first, sum_all_unchecked(l, r) - excluded.second};
    }

    sum_type sum_less_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        return count_and_sum_less_bitwise(l, r, mask, upper, operation).second;
    }

    sum_type sum_less_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        return count_and_sum_less_equal_bitwise(l, r, mask, upper, operation).second;
    }

    sum_type sum_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long value,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (sum_equal_bitwise).");
        if(!bitwise_value_fits(value)) return 0;
        auto high = value == bitwise_value_mask()
            ? std::pair<int, sum_type>{r - l, sum_all_unchecked(l, r)}
            : count_and_sum_less_bitwise_unchecked(l, r, mask, value + 1, operation);
        auto low = count_and_sum_less_bitwise_unchecked(l, r, mask, value, operation);
        return high.second - low.second;
    }

    sum_type sum_range_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        unsigned long long upper,
        BitwiseOperation operation
    ) const{
        return count_and_sum_range_bitwise(l, r, mask, lower, upper, operation).second;
    }

    sum_type sum_greater_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        BitwiseOperation operation
    ) const{
        return count_and_sum_greater_bitwise(l, r, mask, lower, operation).second;
    }

    sum_type sum_greater_equal_bitwise(
        int l,
        int r,
        unsigned long long mask,
        unsigned long long lower,
        BitwiseOperation operation
    ) const{
        return count_and_sum_greater_equal_bitwise(l, r, mask, lower, operation).second;
    }

    unsigned long long access_bitwise(
        int i,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        check_index(i, "library assertion fault: range violation (access_bitwise).");
        return apply_bitwise_value(static_cast<U>(original_values[static_cast<std::size_t>(i)]), mask, operation);
    }

    int rank_bitwise(
        unsigned long long value,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return rank_bitwise(value, 0, r, mask, operation);
    }

    int rank_bitwise(
        unsigned long long value,
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return count_equal_bitwise(l, r, mask, value, operation);
    }

    int count_bitwise(
        unsigned long long value,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return rank_bitwise(value, _n, mask, operation);
    }

    int count_bitwise(
        unsigned long long value,
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return rank_bitwise(value, l, r, mask, operation);
    }

    bool contains_bitwise(
        unsigned long long value,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return count_bitwise(value, mask, operation) != 0;
    }

    int select_bitwise(
        unsigned long long value,
        int kth,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        if(kth < 0 || count_bitwise(value, mask, operation) <= kth) return -1;
        int low = 0;
        int high = _n - 1;
        while(low < high){
            int middle = low + (high - low) / 2;
            if(rank_bitwise(value, middle + 1, mask, operation) > kth) high = middle;
            else low = middle + 1;
        }
        return low;
    }

    unsigned long long kth_smallest_bitwise(
        int l,
        int r,
        int k,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (kth_smallest_bitwise).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_smallest_bitwise).");
        }
        return kth_smallest_bitwise_unchecked(l, r, k, mask, operation);
    }

    unsigned long long kth_largest_bitwise(
        int l,
        int r,
        int k,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (kth_largest_bitwise).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_largest_bitwise).");
        }
        return kth_smallest_bitwise_unchecked(l, r, r - l - 1 - k, mask, operation);
    }

    unsigned long long quantile_bitwise(
        int l,
        int r,
        int k,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return kth_smallest_bitwise(l, r, k, mask, operation);
    }

    std::optional<unsigned long long> min_value_bitwise(
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (min_value_bitwise).");
        if(l == r) return std::nullopt;
        return kth_smallest_bitwise_unchecked(l, r, 0, mask, operation);
    }

    std::optional<unsigned long long> max_value_bitwise(
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (max_value_bitwise).");
        if(l == r) return std::nullopt;
        return kth_smallest_bitwise_unchecked(l, r, r - l - 1, mask, operation);
    }

    std::optional<unsigned long long> median_lower_bitwise(
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (median_lower_bitwise).");
        if(l == r) return std::nullopt;
        return kth_smallest_bitwise_unchecked(l, r, (r - l - 1) / 2, mask, operation);
    }

    std::optional<unsigned long long> median_upper_bitwise(
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        check_range(l, r, "library assertion fault: range violation (median_upper_bitwise).");
        if(l == r) return std::nullopt;
        return kth_smallest_bitwise_unchecked(l, r, (r - l) / 2, mask, operation);
    }

    std::optional<unsigned long long> next_value_ge_bitwise(
        int l,
        int r,
        unsigned long long lower,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        int count = count_less_bitwise(l, r, mask, lower, operation);
        if(count == r - l) return std::nullopt;
        return kth_smallest_bitwise(l, r, count, mask, operation);
    }

    std::optional<unsigned long long> next_value_gt_bitwise(
        int l,
        int r,
        unsigned long long lower,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        int count = count_less_equal_bitwise(l, r, mask, lower, operation);
        if(count == r - l) return std::nullopt;
        return kth_smallest_bitwise(l, r, count, mask, operation);
    }

    std::optional<unsigned long long> prev_value_lt_bitwise(
        int l,
        int r,
        unsigned long long upper,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        int count = count_less_bitwise(l, r, mask, upper, operation);
        if(count == 0) return std::nullopt;
        return kth_smallest_bitwise(l, r, count - 1, mask, operation);
    }

    std::optional<unsigned long long> prev_value_le_bitwise(
        int l,
        int r,
        unsigned long long upper,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        int count = count_less_equal_bitwise(l, r, mask, upper, operation);
        if(count == 0) return std::nullopt;
        return kth_smallest_bitwise(l, r, count - 1, mask, operation);
    }

    std::optional<unsigned long long> next_value_bitwise(
        int l,
        int r,
        unsigned long long lower,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return next_value_ge_bitwise(l, r, lower, mask, operation);
    }

    std::optional<unsigned long long> prev_value_bitwise(
        int l,
        int r,
        unsigned long long upper,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return prev_value_lt_bitwise(l, r, upper, mask, operation);
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_bitwise(
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        auto source = list_frequencies(l, r);
        std::map<unsigned long long, int> frequencies;
        for(const auto& [value, frequency] : source){
            frequencies[apply_bitwise_value(static_cast<U>(value), mask, operation)] += frequency;
        }
        return {frequencies.begin(), frequencies.end()};
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_bitwise(
        int l,
        int r,
        unsigned long long lower,
        unsigned long long upper,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (list_frequencies_bitwise).");
        }
        auto all = list_frequencies_bitwise(l, r, mask, operation);
        std::vector<std::pair<unsigned long long, int>> result;
        for(const auto& entry : all){
            if(lower <= entry.first && entry.first < upper) result.push_back(entry);
        }
        return result;
    }

    std::vector<unsigned long long> distinct_values_bitwise(
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        auto frequencies = list_frequencies_bitwise(l, r, mask, operation);
        std::vector<unsigned long long> result;
        result.reserve(frequencies.size());
        for(const auto& [value, frequency] : frequencies){
            static_cast<void>(frequency);
            result.push_back(value);
        }
        return result;
    }

    std::vector<unsigned long long> values_bitwise(
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return distinct_values_bitwise(0, _n, mask, operation);
    }

    int distinct_size_bitwise(
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        return static_cast<int>(values_bitwise(mask, operation).size());
    }

    int index_of_bitwise(
        unsigned long long value,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        auto transformed_values = values_bitwise(mask, operation);
        auto iterator = std::lower_bound(transformed_values.begin(), transformed_values.end(), value);
        if(iterator == transformed_values.end() || *iterator != value) return -1;
        return static_cast<int>(iterator - transformed_values.begin());
    }

    std::vector<std::pair<unsigned long long, int>> top_k_frequent_bitwise(
        int l,
        int r,
        int k,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        if(k <= 0) return {};
        auto result = list_frequencies_bitwise(l, r, mask, operation);
        std::sort(result.begin(), result.end(), [](const auto& left, const auto& right){
            if(left.second != right.second) return left.second > right.second;
            return left.first < right.first;
        });
        if(static_cast<int>(result.size()) > k) result.resize(static_cast<std::size_t>(k));
        return result;
    }

    std::optional<std::pair<unsigned long long, int>> mode_bitwise(
        int l,
        int r,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        auto result = top_k_frequent_bitwise(l, r, 1, mask, operation);
        if(result.empty()) return std::nullopt;
        return result.front();
    }

    std::vector<std::tuple<unsigned long long, int, int>> intersect_bitwise(
        int l1,
        int r1,
        int l2,
        int r2,
        unsigned long long mask,
        BitwiseOperation operation
    ) const{
        auto first = list_frequencies_bitwise(l1, r1, mask, operation);
        auto second = list_frequencies_bitwise(l2, r2, mask, operation);
        std::vector<std::tuple<unsigned long long, int, int>> result;
        std::size_t i = 0;
        std::size_t j = 0;
        while(i < first.size() && j < second.size()){
            if(first[i].first < second[j].first){
                i++;
            }
            else if(second[j].first < first[i].first){
                j++;
            }
            else{
                result.emplace_back(first[i].first, first[i].second, second[j].second);
                i++;
                j++;
            }
        }
        return result;
    }


    int count_less_xor(int l, int r, unsigned long long mask, unsigned long long upper) const{
        check_range(l, r, "library assertion fault: range violation (count_less_xor).");
        return count_less_xor_unchecked(l, r, mask, upper);
    }

    int range_freq_xor(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq_xor).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_freq_xor).");
        }
        return count_less_xor_unchecked(l, r, mask, upper)
            - count_less_xor_unchecked(l, r, mask, lower);
    }

    int count_less_equal_xor(int l, int r, unsigned long long mask, unsigned long long upper) const{
        check_range(l, r, "library assertion fault: range violation (count_less_equal_xor).");
        if(!bitwise_value_fits(upper) || upper == bitwise_value_mask()) return r - l;
        return count_less_xor_unchecked(l, r, mask, upper + 1);
    }

    int count_equal_xor(int l, int r, unsigned long long mask, unsigned long long value) const{
        check_range(l, r, "library assertion fault: range violation (count_equal_xor).");
        if(!bitwise_value_fits(value)) return 0;
        U raw = static_cast<U>(normalize_bitwise(value) ^ normalize_bitwise(mask));
        return rank(raw_to_value(raw), l, r);
    }

    int count_greater_xor(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return (r - l) - count_less_equal_xor(l, r, mask, lower);
    }

    int count_greater_equal_xor(int l, int r, unsigned long long mask, unsigned long long lower) const{
        check_range(l, r, "library assertion fault: range violation (count_greater_equal_xor).");
        return (r - l) - count_less_xor_unchecked(l, r, mask, lower);
    }

    std::pair<int, sum_type> count_and_sum_less_xor(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_and_sum_less_bitwise(l, r, mask, upper, BitwiseOperation::Xor);
    }

    std::pair<int, sum_type> count_and_sum_less_equal_xor(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_and_sum_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::Xor);
    }

    std::pair<int, sum_type> count_and_sum_range_xor(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return count_and_sum_range_bitwise(l, r, mask, lower, upper, BitwiseOperation::Xor);
    }

    std::pair<int, sum_type> count_and_sum_greater_xor(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_and_sum_greater_bitwise(l, r, mask, lower, BitwiseOperation::Xor);
    }

    std::pair<int, sum_type> count_and_sum_greater_equal_xor(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_and_sum_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::Xor);
    }

    sum_type sum_less_xor(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return sum_less_bitwise(l, r, mask, upper, BitwiseOperation::Xor);
    }

    sum_type sum_less_equal_xor(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return sum_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::Xor);
    }

    sum_type sum_equal_xor(int l, int r, unsigned long long mask, unsigned long long value) const{
        return sum_equal_bitwise(l, r, mask, value, BitwiseOperation::Xor);
    }

    sum_type sum_range_xor(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return sum_range_bitwise(l, r, mask, lower, upper, BitwiseOperation::Xor);
    }

    sum_type sum_greater_xor(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return sum_greater_bitwise(l, r, mask, lower, BitwiseOperation::Xor);
    }

    sum_type sum_greater_equal_xor(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return sum_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::Xor);
    }

    unsigned long long access_xor(int i, unsigned long long mask) const{
        return access_bitwise(i, mask, BitwiseOperation::Xor);
    }

    int rank_xor(unsigned long long value, int r, unsigned long long mask) const{
        return rank_xor(value, 0, r, mask);
    }

    int rank_xor(unsigned long long value, int l, int r, unsigned long long mask) const{
        return count_equal_xor(l, r, mask, value);
    }

    int count_xor(unsigned long long value, unsigned long long mask) const{
        return rank_xor(value, _n, mask);
    }

    int count_xor(unsigned long long value, int l, int r, unsigned long long mask) const{
        return rank_xor(value, l, r, mask);
    }

    bool contains_xor(unsigned long long value, unsigned long long mask) const{
        return count_xor(value, mask) != 0;
    }

    std::vector<unsigned long long> values_xor(unsigned long long mask) const{
        return values_bitwise(mask, BitwiseOperation::Xor);
    }

    int distinct_size_xor(unsigned long long mask) const{
        static_cast<void>(mask);
        return distinct_size();
    }

    int index_of_xor(unsigned long long value, unsigned long long mask) const{
        return index_of_bitwise(value, mask, BitwiseOperation::Xor);
    }

    int select_xor(unsigned long long value, int kth, unsigned long long mask) const{
        if(!bitwise_value_fits(value)) return -1;
        U raw = static_cast<U>(normalize_bitwise(value) ^ normalize_bitwise(mask));
        return select(raw_to_value(raw), kth);
    }

    unsigned long long kth_smallest_xor(int l, int r, int k, unsigned long long mask) const{
        check_range(l, r, "library assertion fault: range violation (kth_smallest_xor).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_smallest_xor).");
        }
        return kth_smallest_xor_unchecked(l, r, k, mask);
    }

    unsigned long long kth_largest_xor(int l, int r, int k, unsigned long long mask) const{
        check_range(l, r, "library assertion fault: range violation (kth_largest_xor).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_largest_xor).");
        }
        return kth_smallest_xor_unchecked(l, r, r - l - 1 - k, mask);
    }

    unsigned long long quantile_xor(int l, int r, int k, unsigned long long mask) const{
        return kth_smallest_xor(l, r, k, mask);
    }

    std::optional<unsigned long long> min_value_xor(int l, int r, unsigned long long mask) const{
        check_range(l, r, "library assertion fault: range violation (min_value_xor).");
        if(l == r) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, 0, mask);
    }

    std::optional<unsigned long long> max_value_xor(int l, int r, unsigned long long mask) const{
        check_range(l, r, "library assertion fault: range violation (max_value_xor).");
        if(l == r) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, r - l - 1, mask);
    }

    std::optional<unsigned long long> median_lower_xor(int l, int r, unsigned long long mask) const{
        check_range(l, r, "library assertion fault: range violation (median_lower_xor).");
        if(l == r) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, (r - l - 1) / 2, mask);
    }

    std::optional<unsigned long long> median_upper_xor(int l, int r, unsigned long long mask) const{
        check_range(l, r, "library assertion fault: range violation (median_upper_xor).");
        if(l == r) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, (r - l) / 2, mask);
    }

    std::optional<unsigned long long> next_value_ge_xor(int l, int r, unsigned long long lower, unsigned long long mask) const{
        int count = count_less_xor(l, r, mask, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, count, mask);
    }

    std::optional<unsigned long long> next_value_gt_xor(int l, int r, unsigned long long lower, unsigned long long mask) const{
        int count = count_less_equal_xor(l, r, mask, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, count, mask);
    }

    std::optional<unsigned long long> prev_value_lt_xor(int l, int r, unsigned long long upper, unsigned long long mask) const{
        int count = count_less_xor(l, r, mask, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, count - 1, mask);
    }

    std::optional<unsigned long long> prev_value_le_xor(int l, int r, unsigned long long upper, unsigned long long mask) const{
        int count = count_less_equal_xor(l, r, mask, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest_xor_unchecked(l, r, count - 1, mask);
    }

    std::optional<unsigned long long> next_value_xor(int l, int r, unsigned long long lower, unsigned long long mask) const{
        return next_value_ge_xor(l, r, lower, mask);
    }

    std::optional<unsigned long long> prev_value_xor(int l, int r, unsigned long long upper, unsigned long long mask) const{
        return prev_value_lt_xor(l, r, upper, mask);
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_xor(int l, int r, unsigned long long mask) const{
        return list_frequencies_bitwise(l, r, mask, BitwiseOperation::Xor);
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_xor(int l, int r, unsigned long long lower, unsigned long long upper, unsigned long long mask) const{
        return list_frequencies_bitwise(l, r, lower, upper, mask, BitwiseOperation::Xor);
    }

    std::vector<unsigned long long> distinct_values_xor(int l, int r, unsigned long long mask) const{
        return distinct_values_bitwise(l, r, mask, BitwiseOperation::Xor);
    }

    std::vector<std::pair<unsigned long long, int>> top_k_frequent_xor(int l, int r, int k, unsigned long long mask) const{
        return top_k_frequent_bitwise(l, r, k, mask, BitwiseOperation::Xor);
    }

    std::optional<std::pair<unsigned long long, int>> mode_xor(int l, int r, unsigned long long mask) const{
        return mode_bitwise(l, r, mask, BitwiseOperation::Xor);
    }

    std::vector<std::tuple<unsigned long long, int, int>> intersect_xor(int l1, int r1, int l2, int r2, unsigned long long mask) const{
        return intersect_bitwise(l1, r1, l2, r2, mask, BitwiseOperation::Xor);
    }


    int count_less_or(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_less_bitwise(l, r, mask, upper, BitwiseOperation::Or);
    }

    int range_freq_or(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return range_freq_bitwise(l, r, mask, lower, upper, BitwiseOperation::Or);
    }

    int count_less_equal_or(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::Or);
    }

    int count_equal_or(int l, int r, unsigned long long mask, unsigned long long value) const{
        return count_equal_bitwise(l, r, mask, value, BitwiseOperation::Or);
    }

    int count_greater_or(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_greater_bitwise(l, r, mask, lower, BitwiseOperation::Or);
    }

    int count_greater_equal_or(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::Or);
    }

    std::pair<int, sum_type> count_and_sum_less_or(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_and_sum_less_bitwise(l, r, mask, upper, BitwiseOperation::Or);
    }

    std::pair<int, sum_type> count_and_sum_less_equal_or(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_and_sum_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::Or);
    }

    std::pair<int, sum_type> count_and_sum_range_or(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return count_and_sum_range_bitwise(l, r, mask, lower, upper, BitwiseOperation::Or);
    }

    std::pair<int, sum_type> count_and_sum_greater_or(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_and_sum_greater_bitwise(l, r, mask, lower, BitwiseOperation::Or);
    }

    std::pair<int, sum_type> count_and_sum_greater_equal_or(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_and_sum_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::Or);
    }

    sum_type sum_less_or(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return sum_less_bitwise(l, r, mask, upper, BitwiseOperation::Or);
    }

    sum_type sum_less_equal_or(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return sum_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::Or);
    }

    sum_type sum_equal_or(int l, int r, unsigned long long mask, unsigned long long value) const{
        return sum_equal_bitwise(l, r, mask, value, BitwiseOperation::Or);
    }

    sum_type sum_range_or(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return sum_range_bitwise(l, r, mask, lower, upper, BitwiseOperation::Or);
    }

    sum_type sum_greater_or(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return sum_greater_bitwise(l, r, mask, lower, BitwiseOperation::Or);
    }

    sum_type sum_greater_equal_or(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return sum_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::Or);
    }

    unsigned long long access_or(int i, unsigned long long mask) const{
        return access_bitwise(i, mask, BitwiseOperation::Or);
    }

    int rank_or(unsigned long long value, int r, unsigned long long mask) const{
        return rank_bitwise(value, r, mask, BitwiseOperation::Or);
    }

    int rank_or(unsigned long long value, int l, int r, unsigned long long mask) const{
        return rank_bitwise(value, l, r, mask, BitwiseOperation::Or);
    }

    int count_or(unsigned long long value, unsigned long long mask) const{
        return count_bitwise(value, mask, BitwiseOperation::Or);
    }

    int count_or(unsigned long long value, int l, int r, unsigned long long mask) const{
        return count_bitwise(value, l, r, mask, BitwiseOperation::Or);
    }

    bool contains_or(unsigned long long value, unsigned long long mask) const{
        return contains_bitwise(value, mask, BitwiseOperation::Or);
    }

    std::vector<unsigned long long> values_or(unsigned long long mask) const{
        return values_bitwise(mask, BitwiseOperation::Or);
    }

    int distinct_size_or(unsigned long long mask) const{
        return distinct_size_bitwise(mask, BitwiseOperation::Or);
    }

    int index_of_or(unsigned long long value, unsigned long long mask) const{
        return index_of_bitwise(value, mask, BitwiseOperation::Or);
    }

    int select_or(unsigned long long value, int kth, unsigned long long mask) const{
        return select_bitwise(value, kth, mask, BitwiseOperation::Or);
    }

    unsigned long long kth_smallest_or(int l, int r, int k, unsigned long long mask) const{
        return kth_smallest_bitwise(l, r, k, mask, BitwiseOperation::Or);
    }

    unsigned long long kth_largest_or(int l, int r, int k, unsigned long long mask) const{
        return kth_largest_bitwise(l, r, k, mask, BitwiseOperation::Or);
    }

    unsigned long long quantile_or(int l, int r, int k, unsigned long long mask) const{
        return quantile_bitwise(l, r, k, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> min_value_or(int l, int r, unsigned long long mask) const{
        return min_value_bitwise(l, r, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> max_value_or(int l, int r, unsigned long long mask) const{
        return max_value_bitwise(l, r, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> median_lower_or(int l, int r, unsigned long long mask) const{
        return median_lower_bitwise(l, r, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> median_upper_or(int l, int r, unsigned long long mask) const{
        return median_upper_bitwise(l, r, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> next_value_ge_or(int l, int r, unsigned long long lower, unsigned long long mask) const{
        return next_value_ge_bitwise(l, r, lower, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> next_value_gt_or(int l, int r, unsigned long long lower, unsigned long long mask) const{
        return next_value_gt_bitwise(l, r, lower, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> prev_value_lt_or(int l, int r, unsigned long long upper, unsigned long long mask) const{
        return prev_value_lt_bitwise(l, r, upper, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> prev_value_le_or(int l, int r, unsigned long long upper, unsigned long long mask) const{
        return prev_value_le_bitwise(l, r, upper, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> next_value_or(int l, int r, unsigned long long lower, unsigned long long mask) const{
        return next_value_bitwise(l, r, lower, mask, BitwiseOperation::Or);
    }

    std::optional<unsigned long long> prev_value_or(int l, int r, unsigned long long upper, unsigned long long mask) const{
        return prev_value_bitwise(l, r, upper, mask, BitwiseOperation::Or);
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_or(int l, int r, unsigned long long mask) const{
        return list_frequencies_bitwise(l, r, mask, BitwiseOperation::Or);
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_or(int l, int r, unsigned long long lower, unsigned long long upper, unsigned long long mask) const{
        return list_frequencies_bitwise(l, r, lower, upper, mask, BitwiseOperation::Or);
    }

    std::vector<unsigned long long> distinct_values_or(int l, int r, unsigned long long mask) const{
        return distinct_values_bitwise(l, r, mask, BitwiseOperation::Or);
    }

    std::vector<std::pair<unsigned long long, int>> top_k_frequent_or(int l, int r, int k, unsigned long long mask) const{
        return top_k_frequent_bitwise(l, r, k, mask, BitwiseOperation::Or);
    }

    std::optional<std::pair<unsigned long long, int>> mode_or(int l, int r, unsigned long long mask) const{
        return mode_bitwise(l, r, mask, BitwiseOperation::Or);
    }

    std::vector<std::tuple<unsigned long long, int, int>> intersect_or(int l1, int r1, int l2, int r2, unsigned long long mask) const{
        return intersect_bitwise(l1, r1, l2, r2, mask, BitwiseOperation::Or);
    }


    int count_less_and(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_less_bitwise(l, r, mask, upper, BitwiseOperation::And);
    }

    int range_freq_and(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return range_freq_bitwise(l, r, mask, lower, upper, BitwiseOperation::And);
    }

    int count_less_equal_and(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::And);
    }

    int count_equal_and(int l, int r, unsigned long long mask, unsigned long long value) const{
        return count_equal_bitwise(l, r, mask, value, BitwiseOperation::And);
    }

    int count_greater_and(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_greater_bitwise(l, r, mask, lower, BitwiseOperation::And);
    }

    int count_greater_equal_and(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::And);
    }

    std::pair<int, sum_type> count_and_sum_less_and(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_and_sum_less_bitwise(l, r, mask, upper, BitwiseOperation::And);
    }

    std::pair<int, sum_type> count_and_sum_less_equal_and(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return count_and_sum_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::And);
    }

    std::pair<int, sum_type> count_and_sum_range_and(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return count_and_sum_range_bitwise(l, r, mask, lower, upper, BitwiseOperation::And);
    }

    std::pair<int, sum_type> count_and_sum_greater_and(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_and_sum_greater_bitwise(l, r, mask, lower, BitwiseOperation::And);
    }

    std::pair<int, sum_type> count_and_sum_greater_equal_and(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return count_and_sum_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::And);
    }

    sum_type sum_less_and(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return sum_less_bitwise(l, r, mask, upper, BitwiseOperation::And);
    }

    sum_type sum_less_equal_and(int l, int r, unsigned long long mask, unsigned long long upper) const{
        return sum_less_equal_bitwise(l, r, mask, upper, BitwiseOperation::And);
    }

    sum_type sum_equal_and(int l, int r, unsigned long long mask, unsigned long long value) const{
        return sum_equal_bitwise(l, r, mask, value, BitwiseOperation::And);
    }

    sum_type sum_range_and(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const{
        return sum_range_bitwise(l, r, mask, lower, upper, BitwiseOperation::And);
    }

    sum_type sum_greater_and(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return sum_greater_bitwise(l, r, mask, lower, BitwiseOperation::And);
    }

    sum_type sum_greater_equal_and(int l, int r, unsigned long long mask, unsigned long long lower) const{
        return sum_greater_equal_bitwise(l, r, mask, lower, BitwiseOperation::And);
    }

    unsigned long long access_and(int i, unsigned long long mask) const{
        return access_bitwise(i, mask, BitwiseOperation::And);
    }

    int rank_and(unsigned long long value, int r, unsigned long long mask) const{
        return rank_bitwise(value, r, mask, BitwiseOperation::And);
    }

    int rank_and(unsigned long long value, int l, int r, unsigned long long mask) const{
        return rank_bitwise(value, l, r, mask, BitwiseOperation::And);
    }

    int count_and(unsigned long long value, unsigned long long mask) const{
        return count_bitwise(value, mask, BitwiseOperation::And);
    }

    int count_and(unsigned long long value, int l, int r, unsigned long long mask) const{
        return count_bitwise(value, l, r, mask, BitwiseOperation::And);
    }

    bool contains_and(unsigned long long value, unsigned long long mask) const{
        return contains_bitwise(value, mask, BitwiseOperation::And);
    }

    std::vector<unsigned long long> values_and(unsigned long long mask) const{
        return values_bitwise(mask, BitwiseOperation::And);
    }

    int distinct_size_and(unsigned long long mask) const{
        return distinct_size_bitwise(mask, BitwiseOperation::And);
    }

    int index_of_and(unsigned long long value, unsigned long long mask) const{
        return index_of_bitwise(value, mask, BitwiseOperation::And);
    }

    int select_and(unsigned long long value, int kth, unsigned long long mask) const{
        return select_bitwise(value, kth, mask, BitwiseOperation::And);
    }

    unsigned long long kth_smallest_and(int l, int r, int k, unsigned long long mask) const{
        return kth_smallest_bitwise(l, r, k, mask, BitwiseOperation::And);
    }

    unsigned long long kth_largest_and(int l, int r, int k, unsigned long long mask) const{
        return kth_largest_bitwise(l, r, k, mask, BitwiseOperation::And);
    }

    unsigned long long quantile_and(int l, int r, int k, unsigned long long mask) const{
        return quantile_bitwise(l, r, k, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> min_value_and(int l, int r, unsigned long long mask) const{
        return min_value_bitwise(l, r, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> max_value_and(int l, int r, unsigned long long mask) const{
        return max_value_bitwise(l, r, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> median_lower_and(int l, int r, unsigned long long mask) const{
        return median_lower_bitwise(l, r, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> median_upper_and(int l, int r, unsigned long long mask) const{
        return median_upper_bitwise(l, r, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> next_value_ge_and(int l, int r, unsigned long long lower, unsigned long long mask) const{
        return next_value_ge_bitwise(l, r, lower, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> next_value_gt_and(int l, int r, unsigned long long lower, unsigned long long mask) const{
        return next_value_gt_bitwise(l, r, lower, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> prev_value_lt_and(int l, int r, unsigned long long upper, unsigned long long mask) const{
        return prev_value_lt_bitwise(l, r, upper, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> prev_value_le_and(int l, int r, unsigned long long upper, unsigned long long mask) const{
        return prev_value_le_bitwise(l, r, upper, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> next_value_and(int l, int r, unsigned long long lower, unsigned long long mask) const{
        return next_value_bitwise(l, r, lower, mask, BitwiseOperation::And);
    }

    std::optional<unsigned long long> prev_value_and(int l, int r, unsigned long long upper, unsigned long long mask) const{
        return prev_value_bitwise(l, r, upper, mask, BitwiseOperation::And);
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_and(int l, int r, unsigned long long mask) const{
        return list_frequencies_bitwise(l, r, mask, BitwiseOperation::And);
    }

    std::vector<std::pair<unsigned long long, int>> list_frequencies_and(int l, int r, unsigned long long lower, unsigned long long upper, unsigned long long mask) const{
        return list_frequencies_bitwise(l, r, lower, upper, mask, BitwiseOperation::And);
    }

    std::vector<unsigned long long> distinct_values_and(int l, int r, unsigned long long mask) const{
        return distinct_values_bitwise(l, r, mask, BitwiseOperation::And);
    }

    std::vector<std::pair<unsigned long long, int>> top_k_frequent_and(int l, int r, int k, unsigned long long mask) const{
        return top_k_frequent_bitwise(l, r, k, mask, BitwiseOperation::And);
    }

    std::optional<std::pair<unsigned long long, int>> mode_and(int l, int r, unsigned long long mask) const{
        return mode_bitwise(l, r, mask, BitwiseOperation::And);
    }

    std::vector<std::tuple<unsigned long long, int, int>> intersect_and(int l1, int r1, int l2, int r2, unsigned long long mask) const{
        return intersect_bitwise(l1, r1, l2, r2, mask, BitwiseOperation::And);
    }

};

#undef DYNAMIC_WAVELET_MATRIX_X86
