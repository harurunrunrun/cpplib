#ifndef CPPLIB_SRC_APPROXIMATE_FACILITY_LOCAL_SEARCH_K_MEDIAN_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_FACILITY_LOCAL_SEARCH_K_MEDIAN_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "add_drop_k_median.hpp"

namespace approximate::facility {

template<class T>
KMedianResult local_search_k_median(
    const std::vector<std::vector<T>>& distance,
    std::size_t k,
    std::size_t maximum_iterations = 100
) {
    return add_drop_k_median(distance, k, maximum_iterations);
}

}  // namespace approximate::facility

#endif  // CPPLIB_SRC_APPROXIMATE_FACILITY_LOCAL_SEARCH_K_MEDIAN_HPP_INCLUDED
