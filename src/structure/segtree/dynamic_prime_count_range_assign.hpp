#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_DYNAMIC_PRIME_COUNT_RANGE_ASSIGN_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_DYNAMIC_PRIME_COUNT_RANGE_ASSIGN_HPP_INCLUDED

#include <memory>
#include <vector>

#include "../../algorithm/math/number_theory/prime_sieve.hpp"
#include "dynamic_lazysegtree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE, int MAX_VALUE>
class DynamicPrimeCountRangeAssign{
    static constexpr AssignSumMonoidAct<int> action_{};
    std::unique_ptr<math::PrimeSieve<MAX_VALUE>> sieve_;
    std::unique_ptr<DynamicLazySegtree<action_, MAX_SIZE>> tree_;

public:
    explicit DynamicPrimeCountRangeAssign(const std::vector<int>& values)
        : sieve_(std::make_unique<math::PrimeSieve<MAX_VALUE>>()),
          tree_(std::make_unique<
              DynamicLazySegtree<action_, MAX_SIZE>
          >(static_cast<int>(values.size()))){
        for(int index = 0; index < static_cast<int>(values.size()); ++index){
            tree_->set(
                index,
                static_cast<int>(sieve_->is_prime(values[static_cast<std::size_t>(index)]))
            );
        }
    }

    int size() const{ return tree_->size(); }

    void assign(int left, int right, int value){
        tree_->apply(
            left,
            right,
            MonoidAssignment<int>{
                true,
                static_cast<int>(sieve_->is_prime(value))
            }
        );
    }

    int count_primes(int left, int right) const{
        return tree_->prod(left, right);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_DYNAMIC_PRIME_COUNT_RANGE_ASSIGN_HPP_INCLUDED
