#ifndef CPPLIB_SRC_APPROXIMATE_SCHEDULING_ADVANCED_SCHEDULING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SCHEDULING_ADVANCED_SCHEDULING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <queue>
#include <random>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "parallel_machine.hpp"
#include "scheduling_common.hpp"

namespace approximate::scheduling {

template<class Time>
struct JobShopOperation {
    std::size_t machine = 0;
    Time processing_time{};
};

template<class Time>
struct ShiftingBottleneckSchedule {
    std::vector<std::vector<Time>> start_time;
    std::vector<std::vector<Time>> completion_time;
    std::vector<std::vector<std::pair<std::size_t, std::size_t>>>
        machine_order;
    std::vector<std::size_t> bottleneck_order;
    Time makespan{};
};

template<class Time>
struct MultiFitScheduleResult {
    ParallelMachineSchedule<Time> schedule;
    long double fitted_capacity = 0.0L;
    std::size_t iterations = 0;
};

namespace advanced_scheduling_internal {

template<class Time>
struct JobShopGraph {
    std::vector<std::vector<std::size_t>> outgoing;
    std::vector<std::vector<std::size_t>> incoming;
    std::vector<Time> duration;
};

template<class Time>
bool topological_times(
    const JobShopGraph<Time>& graph,
    std::vector<std::size_t>& order,
    std::vector<Time>* start_time = nullptr
) {
    const std::size_t size = graph.outgoing.size();
    std::vector<std::size_t> indegree(size);
    for(std::size_t node = 0; node < size; ++node){
        indegree[node] = graph.incoming[node].size();
    }
    std::priority_queue<
        std::size_t, std::vector<std::size_t>, std::greater<std::size_t>
    > ready;
    for(std::size_t node = 0; node < size; ++node){
        if(indegree[node] == 0) ready.push(node);
    }
    order.clear();
    order.reserve(size);
    if(start_time != nullptr) start_time->assign(size, Time{});
    while(!ready.empty()){
        const std::size_t node = ready.top();
        ready.pop();
        order.push_back(node);
        Time finish{};
        if(start_time != nullptr){
            finish = scheduling_internal::checked_time_add((*start_time)[node], graph.duration[node]);
        }
        for(const std::size_t successor : graph.outgoing[node]){
            if(start_time != nullptr
               && (*start_time)[successor] < finish){
                (*start_time)[successor] = finish;
            }
            --indegree[successor];
            if(indegree[successor] == 0) ready.push(successor);
        }
    }
    return order.size() == size;
}

template<class Time>
std::vector<std::size_t> schrage_machine_order(
    const std::vector<std::size_t>& operations,
    const std::vector<Time>& release,
    const std::vector<Time>& tail,
    const std::vector<Time>& duration
) {
    std::vector<std::size_t> arrivals = operations;
    std::stable_sort(
        arrivals.begin(), arrivals.end(),
        [&](std::size_t first, std::size_t second){
            return std::tie(release[first], first)
                < std::tie(release[second], second);
        }
    );
    struct TailLess {
        const std::vector<Time>* tail = nullptr;
        bool operator()(std::size_t first, std::size_t second) const {
            if((*tail)[first] != (*tail)[second]){
                return (*tail)[first] < (*tail)[second];
            }
            return second < first;
        }
    };
    std::priority_queue<
        std::size_t, std::vector<std::size_t>, TailLess
    > available(TailLess{&tail});
    std::vector<std::size_t> result;
    result.reserve(operations.size());
    std::size_t next = 0;
    Time current{};
    while(result.size() < operations.size()){
        if(available.empty() && next < arrivals.size()
           && current < release[arrivals[next]]){
            current = release[arrivals[next]];
        }
        while(next < arrivals.size()
              && release[arrivals[next]] <= current){
            available.push(arrivals[next]);
            ++next;
        }
        const std::size_t operation = available.top();
        available.pop();
        result.push_back(operation);
        current = scheduling_internal::checked_time_add(current, duration[operation]);
    }
    return result;
}

template<class Time>
long double sequence_bound(
    const std::vector<std::size_t>& order,
    const std::vector<Time>& release,
    const std::vector<Time>& tail,
    const std::vector<Time>& duration
) {
    long double current = 0.0L;
    long double bound = 0.0L;
    for(const std::size_t operation : order){
        current = std::max(current, scheduling_internal::as_long_double(release[operation]));
        current += scheduling_internal::as_long_double(duration[operation]);
        bound = std::max(
            bound, current + scheduling_internal::as_long_double(tail[operation])
        );
    }
    return bound;
}

template<class Time>
bool first_fit_decreasing_with_capacity(
    const std::vector<Time>& processing_time,
    std::size_t machine_count,
    long double capacity,
    ParallelMachineSchedule<Time>& schedule
) {
    std::vector<std::size_t> order(processing_time.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(
        order.begin(), order.end(),
        [&](std::size_t first, std::size_t second){
            return processing_time[second] < processing_time[first];
        }
    );
    std::vector<long double> normalized_load;
    std::vector<std::size_t> assignment(processing_time.size());
    for(const std::size_t job : order){
        const long double duration = scheduling_internal::as_long_double(processing_time[job]);
        std::size_t selected = normalized_load.size();
        for(std::size_t machine = 0; machine < normalized_load.size();
            ++machine){
            if(normalized_load[machine] + duration <= capacity){
                selected = machine;
                break;
            }
        }
        if(selected == normalized_load.size()){
            if(normalized_load.size() == machine_count) return false;
            normalized_load.push_back(0.0L);
        }
        normalized_load[selected] += duration;
        assignment[job] = selected;
    }
    schedule.machine_of_job = std::move(assignment);
    schedule.jobs_on_machine.assign(machine_count, {});
    schedule.machine_load.assign(machine_count, Time{});
    for(const std::size_t job : order){
        const std::size_t machine = schedule.machine_of_job[job];
        schedule.jobs_on_machine[machine].push_back(job);
        schedule.machine_load[machine] = scheduling_internal::checked_time_add(
            schedule.machine_load[machine], processing_time[job]
        );
    }
    schedule.makespan = Time{};
    for(const Time& load : schedule.machine_load){
        if(schedule.makespan < load) schedule.makespan = load;
    }
    return true;
}

}  // namespace advanced_scheduling_internal

template<class Time>
[[nodiscard]] ShiftingBottleneckSchedule<Time>
shifting_bottleneck_schedule(
    const std::vector<std::vector<JobShopOperation<Time>>>& jobs,
    std::size_t machine_count
) {
    using namespace advanced_scheduling_internal;
    if(machine_count == 0 && !jobs.empty()){
        throw std::invalid_argument(
            "job-shop scheduling requires at least one machine"
        );
    }
    std::vector<std::vector<std::size_t>> node_of(jobs.size());
    std::vector<std::pair<std::size_t, std::size_t>> operation_of;
    std::vector<std::vector<std::size_t>> operations_on_machine(machine_count);
    std::size_t operation_count = 0;
    for(std::size_t job = 0; job < jobs.size(); ++job){
        node_of[job].resize(jobs[job].size());
        for(std::size_t operation = 0; operation < jobs[job].size();
            ++operation){
            const auto& value = jobs[job][operation];
            if(value.machine >= machine_count){
                throw std::out_of_range("job-shop machine index is out of range");
            }
            scheduling_internal::validate_nonnegative_time(
                value.processing_time,
                "job-shop processing time must be nonnegative and finite"
            );
            node_of[job][operation] = operation_count++;
            operation_of.emplace_back(job, operation);
            operations_on_machine[value.machine].push_back(
                node_of[job][operation]
            );
        }
    }
    JobShopGraph<Time> graph;
    graph.outgoing.resize(operation_count);
    graph.incoming.resize(operation_count);
    graph.duration.resize(operation_count);
    for(std::size_t job = 0; job < jobs.size(); ++job){
        for(std::size_t operation = 0; operation < jobs[job].size();
            ++operation){
            const std::size_t node = node_of[job][operation];
            graph.duration[node] = jobs[job][operation].processing_time;
            if(operation != 0){
                const std::size_t predecessor =
                    node_of[job][operation - 1];
                graph.outgoing[predecessor].push_back(node);
                graph.incoming[node].push_back(predecessor);
            }
        }
    }
    ShiftingBottleneckSchedule<Time> result;
    result.machine_order.resize(machine_count);
    std::vector<unsigned char> fixed(machine_count, 0);
    for(std::size_t fixed_count = 0; fixed_count < machine_count;
        ++fixed_count){
        std::vector<std::size_t> topological;
        std::vector<Time> release;
        if(!topological_times(graph, topological, &release)){
            throw std::logic_error("job-shop precedence graph became cyclic");
        }
        std::vector<Time> tail(operation_count, Time{});
        for(auto iterator = topological.rbegin();
            iterator != topological.rend(); ++iterator){
            const std::size_t node = *iterator;
            for(const std::size_t successor : graph.outgoing[node]){
                const Time candidate = scheduling_internal::checked_time_add(
                    graph.duration[successor], tail[successor]
                );
                if(tail[node] < candidate) tail[node] = candidate;
            }
        }
        std::size_t selected_machine = machine_count;
        long double selected_bound = -1.0L;
        std::vector<std::size_t> selected_order;
        for(std::size_t machine = 0; machine < machine_count; ++machine){
            if(fixed[machine] != 0) continue;
            auto candidate = schrage_machine_order(
                operations_on_machine[machine], release, tail,
                graph.duration
            );
            const long double bound = sequence_bound(
                candidate, release, tail, graph.duration
            );
            if(selected_bound < bound){
                selected_bound = bound;
                selected_machine = machine;
                selected_order = std::move(candidate);
            }
        }
        if(selected_machine == machine_count) break;

        JobShopGraph<Time> trial = graph;
        for(std::size_t index = 1; index < selected_order.size(); ++index){
            trial.outgoing[selected_order[index - 1]].push_back(
                selected_order[index]
            );
            trial.incoming[selected_order[index]].push_back(
                selected_order[index - 1]
            );
        }
        std::vector<std::size_t> ignored;
        if(!topological_times(trial, ignored)){
            std::vector<std::size_t> position(operation_count);
            for(std::size_t index = 0; index < topological.size(); ++index){
                position[topological[index]] = index;
            }
            selected_order = operations_on_machine[selected_machine];
            std::stable_sort(
                selected_order.begin(), selected_order.end(),
                [&](std::size_t first, std::size_t second){
                    return position[first] < position[second];
                }
            );
            trial = graph;
            for(std::size_t index = 1; index < selected_order.size(); ++index){
                trial.outgoing[selected_order[index - 1]].push_back(
                    selected_order[index]
                );
                trial.incoming[selected_order[index]].push_back(
                    selected_order[index - 1]
                );
            }
        }
        graph = std::move(trial);
        fixed[selected_machine] = 1;
        result.bottleneck_order.push_back(selected_machine);
        for(const std::size_t node : selected_order){
            result.machine_order[selected_machine].push_back(
                operation_of[node]
            );
        }
    }

    std::vector<std::size_t> topological;
    std::vector<Time> starts;
    if(!topological_times(graph, topological, &starts)){
        throw std::logic_error("job-shop result is cyclic");
    }
    result.start_time.resize(jobs.size());
    result.completion_time.resize(jobs.size());
    for(std::size_t job = 0; job < jobs.size(); ++job){
        result.start_time[job].resize(jobs[job].size());
        result.completion_time[job].resize(jobs[job].size());
        for(std::size_t operation = 0; operation < jobs[job].size();
            ++operation){
            const std::size_t node = node_of[job][operation];
            result.start_time[job][operation] = starts[node];
            result.completion_time[job][operation] = scheduling_internal::checked_time_add(
                starts[node], graph.duration[node]
            );
            if(result.makespan < result.completion_time[job][operation]){
                result.makespan = result.completion_time[job][operation];
            }
        }
    }
    return result;
}

template<class Time, class URBG>
[[nodiscard]] ParallelMachineSchedule<Time> randomized_list_schedule(
    const std::vector<Time>& processing_time,
    std::size_t machine_count,
    URBG& random
) {
    std::vector<std::size_t> order(processing_time.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::shuffle(order.begin(), order.end(), random);
    return list_schedule(processing_time, machine_count, order);
}

template<class Time>
[[nodiscard]] MultiFitScheduleResult<Time> multifit_schedule(
    const std::vector<Time>& processing_time,
    std::size_t machine_count,
    std::size_t iterations = 10
) {
    using namespace advanced_scheduling_internal;
    if(machine_count == 0){
        throw std::invalid_argument("MultiFit requires a machine");
    }
    long double total = 0.0L;
    long double maximum = 0.0L;
    for(const Time& value : processing_time){
        scheduling_internal::validate_nonnegative_time(
            value, "MultiFit processing times must be nonnegative and finite"
        );
        const long double normalized = scheduling_internal::as_long_double(value);
        total += normalized;
        maximum = std::max(maximum, normalized);
    }
    MultiFitScheduleResult<Time> result;
    result.schedule = lpt_schedule(processing_time, machine_count);
    long double upper = scheduling_internal::as_long_double(result.schedule.makespan);
    long double lower = std::max(
        maximum, total / static_cast<long double>(machine_count)
    );
    const long double multifit_factor =
        1.22L + 1.0L / static_cast<long double>(machine_count);
    lower = std::max(lower, upper / multifit_factor);
    result.fitted_capacity = upper;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const long double middle = (lower + upper) * 0.5L;
        ParallelMachineSchedule<Time> candidate;
        if(first_fit_decreasing_with_capacity(
            processing_time, machine_count, middle, candidate
        )){
            upper = middle;
            result.schedule = std::move(candidate);
            result.fitted_capacity = middle;
        }else{
            lower = middle;
        }
        ++result.iterations;
    }
    return result;
}

}  // namespace approximate::scheduling

#endif  // CPPLIB_SRC_APPROXIMATE_SCHEDULING_ADVANCED_SCHEDULING_HPP_INCLUDED
