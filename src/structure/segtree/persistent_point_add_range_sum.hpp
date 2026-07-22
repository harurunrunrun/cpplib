#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_POINT_ADD_RANGE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_POINT_ADD_RANGE_SUM_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <vector>

#include "persistent_segtree.hpp"
#include "../types/common_monoids.hpp"

namespace persistent_point_add_range_sum_internal{

template<class T>
inline constexpr AddMonoid<T> monoid{};

} // namespace persistent_point_add_range_sum_internal

template<class T, int MAX_SIZE, int MAX_VERSION>
class PersistentPointAddRangeSum{
    PersistentSegtree<
        persistent_point_add_range_sum_internal::monoid<T>,
        MAX_SIZE,
        MAX_VERSION
    > tree_;

public:
    explicit PersistentPointAddRangeSum(int size = MAX_SIZE): tree_(size){}

    explicit PersistentPointAddRangeSum(const std::vector<T>& values):
        tree_(values){}

    template<std::size_t N>
    explicit PersistentPointAddRangeSum(const std::array<T, N>& values):
        tree_(values){}

    int size() const{
        return tree_.size();
    }

    int versions() const{
        return tree_.versions();
    }

    int latest_version() const{
        return tree_.latest_version();
    }

    int add(int version, int position, const T& delta){
        return tree_.set(
            version,
            position,
            tree_.get(version, position) + delta
        );
    }

    T sum(int version, int left, int right) const{
        return tree_.prod(version, left, right);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_POINT_ADD_RANGE_SUM_HPP_INCLUDED
