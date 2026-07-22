#ifndef CPPLIB_SRC_APPROXIMATE_SCHEDULING_SINGLE_MACHINE_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SCHEDULING_SINGLE_MACHINE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace approximate::scheduling {

template<class DueDate>
std::vector<std::size_t> earliest_due_date_order(
    const std::vector<DueDate>& due_date
){
    std::vector<std::size_t> order(due_date.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        return due_date[first] < due_date[second];
    });
    return order;
}

template<class Time, class Weight>
std::vector<std::size_t> weighted_shortest_processing_time_order(
    const std::vector<Time>& processing_time,
    const std::vector<Weight>& weight
){
    if(processing_time.size() != weight.size())[[unlikely]]{
        throw std::invalid_argument("processing times and weights have different sizes");
    }
    for(std::size_t job = 0; job < processing_time.size(); ++job){
        if(processing_time[job] < Time{} || !(Weight{} < weight[job]))[[unlikely]]{
            throw std::invalid_argument(
                "WSPT requires nonnegative times and positive weights"
            );
        }
    }
    std::vector<std::size_t> order(processing_time.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        if constexpr(
            std::is_integral_v<Time> && std::is_integral_v<Weight>
            && sizeof(Time) <= sizeof(std::uint64_t)
            && sizeof(Weight) <= sizeof(std::uint64_t)
        ){
            return static_cast<__uint128_t>(processing_time[first])
                    * static_cast<__uint128_t>(weight[second])
                < static_cast<__uint128_t>(processing_time[second])
                    * static_cast<__uint128_t>(weight[first]);
        }else{
            return static_cast<long double>(processing_time[first])
                    / static_cast<long double>(weight[first])
                < static_cast<long double>(processing_time[second])
                    / static_cast<long double>(weight[second]);
        }
    });
    return order;
}

}  // namespace approximate::scheduling

#endif  // CPPLIB_SRC_APPROXIMATE_SCHEDULING_SINGLE_MACHINE_HPP_INCLUDED
