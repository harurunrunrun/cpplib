#ifndef CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_PRODUCT_LESS_EQUAL_MOD_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_PRODUCT_LESS_EQUAL_MOD_HPP_INCLUDED

#include <memory>
#include <vector>

#include "monoid_merge_sort_tree.hpp"

template<int MOD>
struct ProductModMonoid{
    static_assert(MOD > 0);
    using S = int;
    static constexpr int op(int left, int right){
        return static_cast<int>(
            static_cast<long long>(left) * right % MOD
        );
    }
    static constexpr int e(){ return 1 % MOD; }
};

template<int MOD, int MAX_SIZE>
class StaticRangeProductLessEqualMod{
    static constexpr ProductModMonoid<MOD> monoid_{};
    std::unique_ptr<MonoidMergeSortTree<int, monoid_, MAX_SIZE>> tree_;

public:
    explicit StaticRangeProductLessEqualMod(const std::vector<int>& values)
        : tree_(std::make_unique<
            MonoidMergeSortTree<int, monoid_, MAX_SIZE>
        >(values, values)){}

    int size() const{ return tree_->size(); }

    int product_less_equal(int left, int right, int upper) const{
        return tree_->fold_less_equal(left, right, upper);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_PRODUCT_LESS_EQUAL_MOD_HPP_INCLUDED
