#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_SPECIALIZED_DYNAMIC_PRIME_COUNT_RANGE_ASSIGN_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_SPECIALIZED_DYNAMIC_PRIME_COUNT_RANGE_ASSIGN_HPP_INCLUDED

#include <memory>
#include <vector>

#include "../../../algorithm/math/number_theory/prime/prime_sieve.hpp"
#include "../../../algorithm/math/number_theory/prime/prime_factorization.hpp"
#include "../dynamic/dynamic_lazysegtree.hpp"
#include "../../types/monoid/common_monoids.hpp"

template<int MAX_SIZE = -1, int MAX_VALUE = -1>
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

template<>
class DynamicPrimeCountRangeAssign<-1, -1>{
    static constexpr AssignSumMonoidAct<int> action_{};
    DynamicLazySegtree<action_> tree_;

    static int prime_indicator(int value){
        return value >= 2 && math::is_prime_miller_rabin(static_cast<unsigned long long>(value));
    }

public:
    explicit DynamicPrimeCountRangeAssign(const std::vector<int>& values)
        : tree_(static_cast<long long>(values.size())){
        for(int index = 0; index < static_cast<int>(values.size()); ++index){
            tree_.set(index, prime_indicator(values[static_cast<std::size_t>(index)]));
        }
    }

    int size() const{ return static_cast<int>(tree_.size()); }
    void assign(int left, int right, int value){
        tree_.apply(left, right, MonoidAssignment<int>{true, prime_indicator(value)});
    }
    int count_primes(int left, int right) const{ return tree_.prod(left, right); }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_SPECIALIZED_DYNAMIC_PRIME_COUNT_RANGE_ASSIGN_HPP_INCLUDED
