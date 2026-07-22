#ifndef CPPLIB_SRC_APPROXIMATE_SCHEDULING_PARALLEL_MACHINE_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SCHEDULING_PARALLEL_MACHINE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::scheduling {

template<class Time>
struct ParallelMachineSchedule{
    std::vector<std::size_t> machine_of_job;
    std::vector<std::vector<std::size_t>> jobs_on_machine;
    std::vector<Time> machine_load;
    Time makespan{};
};

namespace internal {

inline void validate_order(
    const std::vector<std::size_t>& order,
    std::size_t job_count
){
    if(order.size() != job_count){
        throw std::invalid_argument("schedule order must contain every job");
    }
    std::vector<unsigned char> used(job_count, 0);
    for(const std::size_t job: order){
        if(job >= job_count || used[job] != 0){
            throw std::invalid_argument("schedule order is not a permutation");
        }
        used[job] = 1;
    }
}

}  // namespace internal

template<class Time>
ParallelMachineSchedule<Time> list_schedule(
    const std::vector<Time>& processing_time,
    std::size_t machine_count,
    const std::vector<std::size_t>& order
){
    if(machine_count == 0)[[unlikely]]{
        throw std::invalid_argument("list_schedule requires a machine");
    }
    internal::validate_order(order, processing_time.size());
    for(const Time& duration: processing_time){
        if(duration < Time{})[[unlikely]]{
            throw std::invalid_argument("processing time must be nonnegative");
        }
    }

    using Entry = std::pair<Time, std::size_t>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    ParallelMachineSchedule<Time> result;
    result.machine_of_job.resize(processing_time.size());
    result.jobs_on_machine.resize(machine_count);
    result.machine_load.assign(machine_count, Time{});
    for(std::size_t machine = 0; machine < machine_count; ++machine){
        queue.emplace(Time{}, machine);
    }
    for(const std::size_t job: order){
        const auto [load, machine] = queue.top();
        queue.pop();
        result.machine_of_job[job] = machine;
        result.jobs_on_machine[machine].push_back(job);
        result.machine_load[machine] = load + processing_time[job];
        queue.emplace(result.machine_load[machine], machine);
    }
    for(const Time& load: result.machine_load){
        if(result.makespan < load) result.makespan = load;
    }
    return result;
}

template<class Time>
ParallelMachineSchedule<Time> fifo_schedule(
    const std::vector<Time>& processing_time,
    std::size_t machine_count
){
    std::vector<std::size_t> order(processing_time.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    return list_schedule(processing_time, machine_count, order);
}

template<class Time>
ParallelMachineSchedule<Time> lifo_schedule(
    const std::vector<Time>& processing_time,
    std::size_t machine_count
){
    std::vector<std::size_t> order(processing_time.size());
    std::iota(order.rbegin(), order.rend(), std::size_t{0});
    return list_schedule(processing_time, machine_count, order);
}

template<class Time>
ParallelMachineSchedule<Time> lpt_schedule(
    const std::vector<Time>& processing_time,
    std::size_t machine_count
){
    std::vector<std::size_t> order(processing_time.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        return processing_time[second] < processing_time[first];
    });
    return list_schedule(processing_time, machine_count, order);
}

template<class Time>
ParallelMachineSchedule<Time> spt_schedule(
    const std::vector<Time>& processing_time,
    std::size_t machine_count
){
    std::vector<std::size_t> order(processing_time.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        return processing_time[first] < processing_time[second];
    });
    return list_schedule(processing_time, machine_count, order);
}

}  // namespace approximate::scheduling

#endif  // CPPLIB_SRC_APPROXIMATE_SCHEDULING_PARALLEL_MACHINE_HPP_INCLUDED
