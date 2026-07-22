#ifndef CPPLIB_SRC_APPROXIMATE_FACILITY_ADD_DROP_K_MEDIAN_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_FACILITY_ADD_DROP_K_MEDIAN_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "add_k_median.hpp"
#include "teitz_bart.hpp"

namespace approximate::facility {

template<class T>
KMedianResult add_drop_k_median(
    const std::vector<std::vector<T>>& distance,
    std::size_t k,
    std::size_t maximum_iterations = 100
) {
    const auto checked = detail::checked_distance_matrix(distance);
    detail::validate_k(checked.size(), k);
    KMedianResult initial = detail::add_k_median_checked(checked, k);
    return detail::teitz_bart_checked(
        checked, std::move(initial.facilities), maximum_iterations
    );
}

}  // namespace approximate::facility

#endif  // CPPLIB_SRC_APPROXIMATE_FACILITY_ADD_DROP_K_MEDIAN_HPP_INCLUDED
