#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<
    class Key,
    auto Monoid,
    int MAX_SIZE,
    class Compare = std::less<Key>
>
struct MonoidMergeSortTree{
    static_assert(MAX_SIZE >= 0);

    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    static constexpr int LEVEL_COUNT = []{
        int result = 1;
        unsigned int value = MAX_SIZE > 0
            ? static_cast<unsigned int>(MAX_SIZE) : 0U;
        while(value > 1U){
            value >>= 1U;
            result++;
        }
        return result;
    }();

    int _n = 0;
    int height = 0;
    std::size_t capacity = 1;
    Compare compare;
    std::array<std::vector<Key>, LEVEL_COUNT> sorted_keys;
    std::array<std::vector<S>, LEVEL_COUNT> prefix;

    void check_range(int left, int right, const char* message) const{
        if(left < 0 || right < left || _n < right)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void build(const std::vector<Key>& keys, const std::vector<S>& weights){
        if(keys.size() != weights.size())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        if(keys.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        _n = static_cast<int>(keys.size());
        while(capacity < keys.size()){
            capacity <<= 1U;
            height++;
        }
        if(keys.empty()) return;

        sorted_keys[0] = keys;
        prefix[0] = weights;
        std::vector<S> current_weights = weights;

        std::size_t block_size = 2;
        for(int level = 1; block_size <= keys.size(); level++, block_size <<= 1U){
            const std::size_t half = block_size >> 1U;
            const std::vector<Key>& previous_keys = sorted_keys[level - 1];
            std::vector<Key> next_keys = previous_keys;
            std::vector<S> next_weights = current_weights;

            for(std::size_t left = 0; left < keys.size(); left += block_size){
                const std::size_t middle = std::min(left + half, keys.size());
                const std::size_t right = std::min(left + block_size, keys.size());
                std::size_t left_index = left;
                std::size_t right_index = middle;
                std::size_t output = left;

                while(left_index < middle && right_index < right){
                    const bool take_right = compare(
                        previous_keys[right_index], previous_keys[left_index]
                    );
                    const std::size_t source = take_right
                        ? right_index++ : left_index++;
                    next_keys[output] = previous_keys[source];
                    next_weights[output] = current_weights[source];
                    output++;
                }
                while(left_index < middle){
                    next_keys[output] = previous_keys[left_index];
                    next_weights[output] = current_weights[left_index];
                    left_index++;
                    output++;
                }
                while(right_index < right){
                    next_keys[output] = previous_keys[right_index];
                    next_weights[output] = current_weights[right_index];
                    right_index++;
                    output++;
                }
            }

            sorted_keys[level] = std::move(next_keys);
            std::vector<S> next_prefix = next_weights;
            for(std::size_t left = 0; left < keys.size(); left += block_size){
                const std::size_t right = std::min(left + block_size, keys.size());
                S aggregate = Monoid.e();
                for(std::size_t index = left; index < right; index++){
                    aggregate = Monoid.op(aggregate, next_weights[index]);
                    next_prefix[index] = aggregate;
                }
            }
            prefix[level] = std::move(next_prefix);
            current_weights = std::move(next_weights);
        }
    }

    S node_fold(int level, std::size_t left, std::size_t length) const{
        return prefix[level][left + length - 1U];
    }

    S node_fold_bound(
        int level,
        std::size_t left,
        std::size_t length,
        const Key& upper,
        bool inclusive
    ) const{
        const auto first = sorted_keys[level].begin()
            + static_cast<std::ptrdiff_t>(left);
        const auto last = first + static_cast<std::ptrdiff_t>(length);
        const auto bound = inclusive
            ? std::upper_bound(first, last, upper, compare)
            : std::lower_bound(first, last, upper, compare);
        const std::size_t count = static_cast<std::size_t>(bound - first);
        if(count == 0) return Monoid.e();
        return prefix[level][left + count - 1U];
    }

    S fold_impl(
        std::size_t node_left,
        std::size_t node_right,
        int level,
        int query_left,
        int query_right
    ) const{
        if(node_right <= static_cast<std::size_t>(query_left)
            || static_cast<std::size_t>(query_right) <= node_left){
            return Monoid.e();
        }
        if(static_cast<std::size_t>(query_left) <= node_left
            && node_right <= static_cast<std::size_t>(query_right)
            && node_right <= static_cast<std::size_t>(_n)){
            return node_fold(level, node_left, node_right - node_left);
        }
        const std::size_t middle = (node_left + node_right) >> 1U;
        const S left_result = fold_impl(
            node_left, middle, level - 1, query_left, query_right
        );
        const S right_result = fold_impl(
            middle, node_right, level - 1, query_left, query_right
        );
        return Monoid.op(left_result, right_result);
    }

    S fold_bound_impl(
        std::size_t node_left,
        std::size_t node_right,
        int level,
        int query_left,
        int query_right,
        const Key& upper,
        bool inclusive
    ) const{
        if(node_right <= static_cast<std::size_t>(query_left)
            || static_cast<std::size_t>(query_right) <= node_left){
            return Monoid.e();
        }
        if(static_cast<std::size_t>(query_left) <= node_left
            && node_right <= static_cast<std::size_t>(query_right)
            && node_right <= static_cast<std::size_t>(_n)){
            return node_fold_bound(
                level, node_left, node_right - node_left, upper, inclusive
            );
        }
        const std::size_t middle = (node_left + node_right) >> 1U;
        const S left_result = fold_bound_impl(
            node_left, middle, level - 1,
            query_left, query_right, upper, inclusive
        );
        const S right_result = fold_bound_impl(
            middle, node_right, level - 1,
            query_left, query_right, upper, inclusive
        );
        return Monoid.op(left_result, right_result);
    }

public:
    explicit MonoidMergeSortTree(
        const std::vector<Key>& keys,
        const std::vector<S>& weights,
        Compare compare_ = Compare{}
    ): compare(std::move(compare_)){
        build(keys, weights);
    }

    template<std::size_t N>
    explicit MonoidMergeSortTree(
        const std::array<Key, N>& keys,
        const std::array<S, N>& weights,
        Compare compare_ = Compare{}
    ): MonoidMergeSortTree(
        std::vector<Key>(keys.begin(), keys.end()),
        std::vector<S>(weights.begin(), weights.end()),
        std::move(compare_)
    ){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int size() const{ return _n; }
    bool empty() const{ return _n == 0; }

    const Key& key(int index) const{
        if(index < 0 || _n <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (key)."
            );
        }
        return sorted_keys[0][static_cast<std::size_t>(index)];
    }

    const S& weight(int index) const{
        if(index < 0 || _n <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (weight)."
            );
        }
        return prefix[0][static_cast<std::size_t>(index)];
    }

    S fold(int left, int right) const{
        check_range(
            left, right,
            "library assertion fault: range violation (fold)."
        );
        if(left == right) return Monoid.e();
        return fold_impl(0, capacity, height, left, right);
    }

    S fold_less(int left, int right, const Key& upper) const{
        check_range(
            left, right,
            "library assertion fault: range violation (fold_less)."
        );
        if(left == right) return Monoid.e();
        return fold_bound_impl(
            0, capacity, height, left, right, upper, false
        );
    }

    S fold_less_equal(int left, int right, const Key& upper) const{
        check_range(
            left, right,
            "library assertion fault: range violation (fold_less_equal)."
        );
        if(left == right) return Monoid.e();
        return fold_bound_impl(
            0, capacity, height, left, right, upper, true
        );
    }
};
