#ifndef CPPLIB_SRC_APPROXIMATE_SCHEDULING_FLOW_SHOP_HEURISTICS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SCHEDULING_FLOW_SHOP_HEURISTICS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "scheduling_common.hpp"

namespace approximate::scheduling {

namespace scheduling_internal {

template<class Time>
[[nodiscard]] std::size_t validate_flow_shop_matrix(
    const std::vector<std::vector<Time>>& processing_time
){
    const std::size_t machine_count = processing_time.empty()
        ? 0
        : processing_time.front().size();
    for(const auto& row: processing_time){
        if(row.size() != machine_count)[[unlikely]]{
            throw std::invalid_argument("flow-shop matrix must be rectangular");
        }
        for(const Time& value: row){
            validate_nonnegative_time(
                value, "processing time must be nonnegative and finite"
            );
        }
    }
    return machine_count;
}

template<class Time>
[[nodiscard]] Time checked_flow_shop_makespan(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<std::size_t>& order,
    std::size_t machine_count
){
    std::vector<unsigned char> used(processing_time.size(), 0);
    std::vector<Time> completion(machine_count, Time{});
    for(const std::size_t job: order){
        if(job >= processing_time.size() || used[job] != 0)[[unlikely]]{
            throw std::invalid_argument("flow-shop order is not a partial permutation");
        }
        used[job] = 1;
        if(machine_count == 0) continue;
        completion[0] = checked_time_add(completion[0], processing_time[job][0]);
        for(std::size_t machine = 1; machine < machine_count; ++machine){
            if(completion[machine] < completion[machine - 1]){
                completion[machine] = completion[machine - 1];
            }
            completion[machine] = checked_time_add(
                completion[machine], processing_time[job][machine]
            );
        }
    }
    return machine_count == 0 ? Time{} : completion.back();
}

template<class Value>
[[nodiscard]] std::vector<std::size_t> johnson_order_unchecked(
    const std::vector<Value>& first_machine,
    const std::vector<Value>& second_machine
){
    std::vector<std::size_t> early;
    std::vector<std::size_t> late;
    early.reserve(first_machine.size());
    late.reserve(first_machine.size());
    for(std::size_t job = 0; job < first_machine.size(); ++job){
        (first_machine[job] <= second_machine[job] ? early : late).push_back(job);
    }
    std::stable_sort(early.begin(), early.end(), [&](std::size_t first, std::size_t second){
        return first_machine[first] < first_machine[second];
    });
    std::stable_sort(late.begin(), late.end(), [&](std::size_t first, std::size_t second){
        return second_machine[second] < second_machine[first];
    });
    early.insert(early.end(), late.begin(), late.end());
    return early;
}

}  // namespace scheduling_internal

template<class Time>
std::vector<std::size_t> cds_flow_shop_order(
    const std::vector<std::vector<Time>>& processing_time
){
    using namespace scheduling_internal;
    const std::size_t job_count = processing_time.size();
    const std::size_t machine_count = validate_flow_shop_matrix(processing_time);
    std::vector<std::size_t> identity(job_count);
    std::iota(identity.begin(), identity.end(), std::size_t{0});
    if(job_count <= 1 || machine_count == 0) return identity;
    if(machine_count == 1){
        std::stable_sort(identity.begin(), identity.end(), [&](std::size_t first, std::size_t second){
            return processing_time[first][0] < processing_time[second][0];
        });
        return identity;
    }

    std::vector<std::size_t> best;
    Time best_makespan{};
    bool initialized = false;
    for(std::size_t split = 1; split < machine_count; ++split){
        std::vector<std::size_t> candidate;
        if constexpr(std::is_integral_v<Time> && sizeof(Time) <= 8){
            std::vector<__uint128_t> first_machine(job_count, 0);
            std::vector<__uint128_t> second_machine(job_count, 0);
            for(std::size_t job = 0; job < job_count; ++job){
                for(std::size_t offset = 0; offset < split; ++offset){
                    first_machine[job] += static_cast<__uint128_t>(
                        processing_time[job][offset]
                    );
                    second_machine[job] += static_cast<__uint128_t>(
                        processing_time[job][machine_count - 1 - offset]
                    );
                }
            }
            candidate = johnson_order_unchecked(first_machine, second_machine);
        }else{
            std::vector<long double> first_machine(job_count, 0);
            std::vector<long double> second_machine(job_count, 0);
            for(std::size_t job = 0; job < job_count; ++job){
                for(std::size_t offset = 0; offset < split; ++offset){
                    const long double first = as_long_double(processing_time[job][offset]);
                    const long double second = as_long_double(
                        processing_time[job][machine_count - 1 - offset]
                    );
                    const long double count = static_cast<long double>(offset + 1);
                    first_machine[job] += (first - first_machine[job]) / count;
                    second_machine[job] += (second - second_machine[job]) / count;
                }
            }
            candidate = johnson_order_unchecked(first_machine, second_machine);
        }
        Time makespan{};
        try{
            makespan = checked_flow_shop_makespan(processing_time, candidate, machine_count);
        }catch(const std::overflow_error&){
            continue;
        }
        if(!initialized || makespan < best_makespan){
            initialized = true;
            best_makespan = makespan;
            best = std::move(candidate);
        }
    }
    if(!initialized)[[unlikely]]{
        throw std::overflow_error("every CDS candidate makespan overflows");
    }
    return best;
}

template<class Time>
std::vector<std::size_t> palmer_flow_shop_order(
    const std::vector<std::vector<Time>>& processing_time
){
    using namespace scheduling_internal;
    const std::size_t job_count = processing_time.size();
    const std::size_t machine_count = validate_flow_shop_matrix(processing_time);
    std::vector<std::size_t> order(job_count);
    std::iota(order.begin(), order.end(), std::size_t{0});
    if(machine_count <= 1) return order;
    if constexpr(std::is_integral_v<Time> && sizeof(Time) <= 8){
        constexpr std::size_t exact_machine_limit = std::size_t{1} << 30U;
        if(machine_count <= exact_machine_limit){
            std::vector<__int128_t> exact_slope(job_count, 0);
            for(std::size_t job = 0; job < job_count; ++job){
                for(std::size_t machine = 0; machine < machine_count; ++machine){
                    const __int128_t coefficient = static_cast<__int128_t>(machine_count - 1)
                        - 2 * static_cast<__int128_t>(machine);
                    exact_slope[job] += coefficient
                        * static_cast<__int128_t>(processing_time[job][machine]);
                }
            }
            std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
                return exact_slope[second] < exact_slope[first];
            });
            return order;
        }
    }
    std::vector<long double> slope(job_count, 0);
    long double scale = 0;
    for(const auto& row: processing_time){
        for(const Time& value: row){
            const long double converted = as_long_double(value);
            if(scale < converted) scale = converted;
        }
    }
    if(scale == 0) return order;
    const long double denominator = static_cast<long double>(machine_count);
    for(std::size_t job = 0; job < job_count; ++job){
        for(std::size_t machine = 0; machine < machine_count / 2; ++machine){
            const std::size_t opposite = machine_count - 1 - machine;
            const long double coefficient = (
                static_cast<long double>(machine_count - 1)
                    - 2 * static_cast<long double>(machine)
            ) / denominator;
            const long double difference =
                as_long_double(processing_time[job][machine]) / scale
                - as_long_double(processing_time[job][opposite]) / scale;
            slope[job] += coefficient * difference;
        }
    }
    std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        return slope[second] < slope[first];
    });
    return order;
}

template<class Time>
std::vector<std::size_t> gupta_flow_shop_order(
    const std::vector<std::vector<Time>>& processing_time
){
    using namespace scheduling_internal;
    const std::size_t job_count = processing_time.size();
    const std::size_t machine_count = validate_flow_shop_matrix(processing_time);
    std::vector<std::size_t> order(job_count);
    std::iota(order.begin(), order.end(), std::size_t{0});
    if(machine_count <= 1) return order;
    std::vector<unsigned char> early(job_count, 0);
    std::vector<long double> minimum_pair(job_count);
    for(std::size_t job = 0; job < job_count; ++job){
        early[job] = processing_time[job].front() < processing_time[job].back();
        minimum_pair[job] = std::numeric_limits<long double>::infinity();
        for(std::size_t machine = 0; machine + 1 < machine_count; ++machine){
            const long double average = as_long_double(processing_time[job][machine]) / 2
                + as_long_double(processing_time[job][machine + 1]) / 2;
            if(average < minimum_pair[job]) minimum_pair[job] = average;
        }
    }
    std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        if(early[first] != early[second]) return early[first] > early[second];
        if(minimum_pair[first] == minimum_pair[second]) return false;
        return early[first]
            ? minimum_pair[first] < minimum_pair[second]
            : minimum_pair[second] < minimum_pair[first];
    });
    return order;
}

}  // namespace approximate::scheduling

#endif  // CPPLIB_SRC_APPROXIMATE_SCHEDULING_FLOW_SHOP_HEURISTICS_HPP_INCLUDED
