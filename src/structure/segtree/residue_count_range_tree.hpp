#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_RESIDUE_COUNT_RANGE_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_RESIDUE_COUNT_RANGE_TREE_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "lazysegtree.hpp"
#include "../types/residue_count_monoid.hpp"

template<class Count, int MOD, int MAX_SIZE>
class ResidueCountRangeTree{
    static_assert(MOD > 0);

    using Aggregate = ResidueCountAggregate<Count, MOD>;
    static constexpr AddResidueCountMonoidAct<Count, MOD> action_{};

    int size_;
    LazySegtree<action_, MAX_SIZE> tree_;

    static std::vector<Aggregate> zero_initial(int size){
        if(size < 0 || MAX_SIZE < size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        return std::vector<Aggregate>(
            static_cast<std::size_t>(size),
            residue_count_singleton<Count, MOD>(0)
        );
    }

    static std::vector<Aggregate> make_initial(
        const std::vector<long long>& values
    ){
        std::vector<Aggregate> initial;
        initial.reserve(values.size());
        for(const long long value: values){
            initial.push_back(residue_count_singleton<Count, MOD>(value));
        }
        return initial;
    }

public:
    explicit ResidueCountRangeTree(int size = MAX_SIZE):
        size_(size), tree_(zero_initial(size)){}

    explicit ResidueCountRangeTree(const std::vector<long long>& values):
        size_(static_cast<int>(values.size())), tree_(make_initial(values)){}

    int size() const{
        return size_;
    }

    void increment(int left, int right){
        tree_.apply(left, right, ResidueShift<MOD>{1});
    }

    Count count_residue(int left, int right, long long residue){
        const int normalized = ResidueShift<MOD>{residue}.shift;
        return tree_.prod(left, right).count[
            static_cast<std::size_t>(normalized)
        ];
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_RESIDUE_COUNT_RANGE_TREE_HPP_INCLUDED
