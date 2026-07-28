#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_UNION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_UNION_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "matroid_partition.hpp"

namespace combinatorial_optimization {

template<class IndependenceOracle>
MatroidPartitionResult matroid_union(
    std::size_t ground_set_size,
    std::size_t copy_count,
    IndependenceOracle independent
) {
    if(copy_count == 0){
        return matroid_partition(
            ground_set_size, std::vector<IndependenceOracle>{}
        );
    }
    std::vector<IndependenceOracle> copies(
        copy_count, std::move(independent)
    );
    return matroid_partition(ground_set_size, copies);
}

}  // namespace combinatorial_optimization

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COMBINATORIAL_OPTIMIZATION_MATROID_MATROID_UNION_HPP_INCLUDED
