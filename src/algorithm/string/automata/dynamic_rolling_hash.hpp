#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_DYNAMIC_ROLLING_HASH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_DYNAMIC_ROLLING_HASH_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct DynamicRollingHash{
    using u64 = std::uint64_t;
    using u128 = __uint128_t;

    static constexpr u64 modulus = (u64{1} << 61) - 1;
    static constexpr u64 default_base = 1'000'003;

    struct Node{
        u64 hash = 0;
        int length = 0;
    };

    int length;
    int leaf_count;
    u64 base;
    std::vector<u64> power;
    std::vector<Node> tree;

    static u64 add_mod(u64 left, u64 right){
        const u64 sum = left + right;
        return sum >= modulus ? sum - modulus : sum;
    }

    static u64 multiply_mod(u64 left, u64 right){
        u128 product = static_cast<u128>(left) * right;
        product = (product >> 61) + (product & modulus);
        if(product >= modulus) product -= modulus;
        return static_cast<u64>(product);
    }

    Node merge(const Node& left, const Node& right) const{
        return {
            add_mod(
                multiply_mod(
                    left.hash,
                    power[static_cast<std::size_t>(right.length)]
                ),
                right.hash
            ),
            left.length + right.length
        };
    }

    explicit DynamicRollingHash(
        const std::string& text,
        u64 base_ = default_base
    )
        : length(static_cast<int>(text.size())),
          leaf_count(1),
          base(base_){
        if(base == 0 || base >= modulus)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid base "
                "(DynamicRollingHash)."
            );
        }
        while(leaf_count < std::max(1, length)) leaf_count <<= 1;
        power.assign(static_cast<std::size_t>(length + 1), 1);
        for(int index = 0; index < length; ++index){
            power[static_cast<std::size_t>(index + 1)] =
                multiply_mod(power[static_cast<std::size_t>(index)], base);
        }
        tree.assign(static_cast<std::size_t>(leaf_count * 2), {});
        for(int index = 0; index < length; ++index){
            tree[static_cast<std::size_t>(leaf_count + index)] = {
                static_cast<unsigned char>(
                    text[static_cast<std::size_t>(index)]
                ) + u64(1),
                1
            };
        }
        for(int node = leaf_count; node-- > 1;){
            tree[static_cast<std::size_t>(node)] = merge(
                tree[static_cast<std::size_t>(node * 2)],
                tree[static_cast<std::size_t>(node * 2 + 1)]
            );
        }
    }

    int size() const noexcept{
        return length;
    }

    void set(int index, unsigned char symbol){
        if(index < 0 || length <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(DynamicRollingHash::set)."
            );
        }
        int node = leaf_count + index;
        tree[static_cast<std::size_t>(node)] = {symbol + u64(1), 1};
        while((node >>= 1) != 0){
            tree[static_cast<std::size_t>(node)] = merge(
                tree[static_cast<std::size_t>(node * 2)],
                tree[static_cast<std::size_t>(node * 2 + 1)]
            );
        }
    }

    Node get(int left, int right) const{
        if(left < 0 || right < left || length < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(DynamicRollingHash::get)."
            );
        }
        Node left_result;
        Node right_result;
        for(left += leaf_count, right += leaf_count; left < right;
            left >>= 1, right >>= 1){
            if((left & 1) != 0){
                left_result = merge(
                    left_result,
                    tree[static_cast<std::size_t>(left++)]
                );
            }
            if((right & 1) != 0){
                right_result = merge(
                    tree[static_cast<std::size_t>(--right)],
                    right_result
                );
            }
        }
        return merge(left_result, right_result);
    }

    bool equal(int first, int second, int substring_length) const{
        if(substring_length < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative length "
                "(DynamicRollingHash::equal)."
            );
        }
        return get(first, first + substring_length).hash
            == get(second, second + substring_length).hash;
    }

    int lcp(int first, int second) const{
        if(first < 0 || length < first || second < 0 || length < second)
            [[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(DynamicRollingHash::lcp)."
            );
        }
        int low = 0;
        int high = std::min(length - first, length - second) + 1;
        while(high - low > 1){
            const int middle = low + (high - low) / 2;
            if(equal(first, second, middle)){
                low = middle;
            }else{
                high = middle;
            }
        }
        return low;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_DYNAMIC_ROLLING_HASH_HPP_INCLUDED
