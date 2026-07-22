#ifndef CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_SUM_SQRT_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_SUM_SQRT_TREE_HPP_INCLUDED

#include <stdexcept>
#include <vector>

#include "sqrt_tree.hpp"
#include "../types/common_monoids.hpp"

template<class T>
inline constexpr AddMonoid<T> static_range_sum_add_monoid{};

template<class T, int MAX_SIZE>
class StaticRangeSumSqrtTree{
    SqrtTree<static_range_sum_add_monoid<T>, MAX_SIZE> tree;

public:
    StaticRangeSumSqrtTree() = default;

    explicit StaticRangeSumSqrtTree(const std::vector<T>& values):
        tree(values){}

    void build(const std::vector<T>& values){ tree.build(values); }

    int size() const{ return tree.size(); }
    bool empty() const{ return tree.empty(); }
    T get(int index) const{ return tree.get(index); }

    T sum(int left, int right) const{
        if(left < 0 || left > right || tree.size() < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (sum)."
            );
        }
        return left == right ? T(0) : tree.prod(left, right);
    }

    T all_sum() const{
        return tree.empty() ? T(0) : tree.all_prod();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_SUM_SQRT_TREE_HPP_INCLUDED
