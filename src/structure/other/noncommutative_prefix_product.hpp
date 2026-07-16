#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../types/monoid.hpp"

template<auto Monoid, int MAX_SIZE>
class NoncommutativePrefixProduct{
    static_assert(MAX_SIZE >= 0);

public:
    using value_type = std::decay_t<decltype(Monoid.e())>;

private:
    static constexpr std::size_t tree_base(){
        std::size_t result = 1;
        while(result < static_cast<std::size_t>(MAX_SIZE)) result <<= 1;
        return result;
    }

    static constexpr std::size_t BASE = tree_base();

    int element_count = 0;
    std::array<value_type, 2 * BASE> tree{};

    void check_index(int index, const char* message) const{
        if(index < 0 || index >= element_count)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_range(int left, int right, const char* message) const{
        if(left < 0 || right < left || right > element_count)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void initialize_identity(){
        const value_type identity = Monoid.e();
        tree.fill(identity);
    }

public:
    explicit NoncommutativePrefixProduct(int size = 0): element_count(size){
        if(size < 0 || size > MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation "
                "(NoncommutativePrefixProduct constructor)."
            );
        }
        initialize_identity();
    }

    explicit NoncommutativePrefixProduct(const std::vector<value_type>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation "
                "(NoncommutativePrefixProduct constructor)."
            );
        }
        element_count = static_cast<int>(values.size());
        initialize_identity();
        for(int index = 0; index < element_count; ++index){
            tree[BASE + static_cast<std::size_t>(index)] =
                values[static_cast<std::size_t>(index)];
        }
        for(std::size_t index = BASE; index-- > 1;){
            tree[index] = Monoid.op(tree[index << 1], tree[index << 1 | 1]);
        }
    }

    static constexpr int capacity() noexcept{ return MAX_SIZE; }
    int size() const noexcept{ return element_count; }
    bool empty() const noexcept{ return element_count == 0; }

    const value_type& get(int index) const{
        check_index(
            index,
            "library assertion fault: range violation "
            "(NoncommutativePrefixProduct::get)."
        );
        return tree[BASE + static_cast<std::size_t>(index)];
    }

    void set(int index, const value_type& value){
        check_index(
            index,
            "library assertion fault: range violation "
            "(NoncommutativePrefixProduct::set)."
        );
        std::size_t node = BASE + static_cast<std::size_t>(index);
        tree[node] = value;
        while((node >>= 1) != 0){
            tree[node] = Monoid.op(tree[node << 1], tree[node << 1 | 1]);
        }
    }

    value_type range_product(int left, int right) const{
        check_range(
            left,
            right,
            "library assertion fault: range violation "
            "(NoncommutativePrefixProduct::range_product)."
        );
        std::size_t l = BASE + static_cast<std::size_t>(left);
        std::size_t r = BASE + static_cast<std::size_t>(right);
        value_type left_product = Monoid.e();
        value_type right_product = Monoid.e();
        while(l < r){
            if(l & 1U) left_product = Monoid.op(left_product, tree[l++]);
            if(r & 1U) right_product = Monoid.op(tree[--r], right_product);
            l >>= 1;
            r >>= 1;
        }
        return Monoid.op(left_product, right_product);
    }

    value_type prefix_product(int right) const{
        return range_product(0, right);
    }

    value_type all_product() const{
        return tree[1];
    }
};
