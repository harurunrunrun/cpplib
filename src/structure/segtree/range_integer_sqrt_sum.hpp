#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_INTEGER_SQRT_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_INTEGER_SQRT_SUM_HPP_INCLUDED

#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../algorithm/math/number_theory/basic_number_theory.hpp"
#include "segtree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE>
class RangeIntegerSqrtSum{
    static constexpr AddMonoid<long long> addition_{};
    std::unique_ptr<Segtree<addition_, MAX_SIZE>> tree_;
    std::set<int> unstable_;
    int size_ = 0;

public:
    explicit RangeIntegerSqrtSum(const std::vector<long long>& values)
        : tree_(std::make_unique<Segtree<addition_, MAX_SIZE>>(values)),
          size_(static_cast<int>(values.size())){
        for(int index = 0; index < static_cast<int>(values.size()); ++index){
            if(values[static_cast<std::size_t>(index)] > 1) unstable_.insert(index);
        }
    }

    int size() const{ return size_; }

    void apply_sqrt(int left, int right){
        if(left < 0 || right < left || size() < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid range "
                "(RangeIntegerSqrtSum::apply_sqrt)."
            );
        }
        auto iterator = unstable_.lower_bound(left);
        while(iterator != unstable_.end() && *iterator < right){
            const int index = *iterator;
            const long long value = math::isqrt(tree_->get(index));
            tree_->set(index, value);
            if(value <= 1) iterator = unstable_.erase(iterator);
            else ++iterator;
        }
    }

    long long sum(int left, int right) const{ return tree_->prod(left, right); }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_INTEGER_SQRT_SUM_HPP_INCLUDED
