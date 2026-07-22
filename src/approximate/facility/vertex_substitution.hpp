#ifndef CPPLIB_SRC_APPROXIMATE_FACILITY_VERTEX_SUBSTITUTION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_FACILITY_VERTEX_SUBSTITUTION_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "teitz_bart.hpp"

namespace approximate::facility {

template<class T>
KMedianResult vertex_substitution_k_median(
    const std::vector<std::vector<T>>& distance,
    std::vector<std::size_t> initial_facilities,
    std::size_t maximum_iterations = 100
) {
    return teitz_bart_k_median(
        distance, std::move(initial_facilities), maximum_iterations
    );
}

}  // namespace approximate::facility

#endif  // CPPLIB_SRC_APPROXIMATE_FACILITY_VERTEX_SUBSTITUTION_HPP_INCLUDED
