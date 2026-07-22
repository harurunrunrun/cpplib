#ifndef CPPLIB_SRC_APPROXIMATE_SCHEDULING_DAG_SCHEDULING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SCHEDULING_DAG_SCHEDULING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "scheduling_common.hpp"

namespace approximate::scheduling {

template<class Time>
struct DagCommunicationEdge{
    std::size_t from{};
    std::size_t to{};
    Time communication_time{};
};

template<class Time>
struct DagTaskSlot{
    std::size_t task{};
    Time start{};
    Time finish{};
};

template<class Time>
struct DagSchedule{
    std::vector<std::size_t> processor_of_task;
    std::vector<Time> start_time;
    std::vector<Time> finish_time;
    std::vector<std::size_t> scheduling_order;
    std::vector<std::vector<DagTaskSlot<Time>>> tasks_on_processor;
    std::vector<long double> priority;
    std::vector<std::size_t> critical_path;
    std::size_t critical_processor = std::numeric_limits<std::size_t>::max();
    Time makespan{};
};

namespace scheduling_internal {

enum class DagPriorityRule{
    critical_path,
    heft,
    cpop,
};

template<class Time>
struct ValidatedDag{
    std::size_t processor_count{};
    std::vector<std::vector<std::size_t>> outgoing_edges;
    std::vector<std::vector<std::size_t>> incoming_edges;
    std::vector<std::size_t> topological_order;
    long double scale = 1;
};

template<class Time>
[[nodiscard]] ValidatedDag<Time> validate_dag_input(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<DagCommunicationEdge<Time>>& edges
){
    const std::size_t task_count = processing_time.size();
    ValidatedDag<Time> result;
    result.processor_count = task_count == 0 ? 0 : processing_time.front().size();
    if(task_count != 0 && result.processor_count == 0)[[unlikely]]{
        throw std::invalid_argument("DAG scheduling requires a processor");
    }
    long double maximum = 0;
    for(const auto& row: processing_time){
        if(row.size() != result.processor_count)[[unlikely]]{
            throw std::invalid_argument("task-processor matrix must be rectangular");
        }
        for(const Time& value: row){
            validate_nonnegative_time(
                value, "processing time must be nonnegative and finite"
            );
            const long double converted = as_long_double(value);
            if(maximum < converted) maximum = converted;
        }
    }

    result.outgoing_edges.resize(task_count);
    result.incoming_edges.resize(task_count);
    std::vector<std::size_t> indegree(task_count, 0);
    for(std::size_t edge_index = 0; edge_index < edges.size(); ++edge_index){
        const auto& edge = edges[edge_index];
        if(edge.from >= task_count || edge.to >= task_count)[[unlikely]]{
            throw std::invalid_argument("DAG edge endpoint is out of range");
        }
        validate_nonnegative_time(
            edge.communication_time,
            "communication time must be nonnegative and finite"
        );
        const long double converted = as_long_double(edge.communication_time);
        if(maximum < converted) maximum = converted;
        result.outgoing_edges[edge.from].push_back(edge_index);
        result.incoming_edges[edge.to].push_back(edge_index);
        ++indegree[edge.to];
    }
    result.scale = maximum == 0 ? 1 : maximum;

    std::priority_queue<
        std::size_t,
        std::vector<std::size_t>,
        std::greater<std::size_t>
    > ready;
    for(std::size_t task = 0; task < task_count; ++task){
        if(indegree[task] == 0) ready.push(task);
    }
    result.topological_order.reserve(task_count);
    while(!ready.empty()){
        const std::size_t task = ready.top();
        ready.pop();
        result.topological_order.push_back(task);
        for(const std::size_t edge_index: result.outgoing_edges[task]){
            const std::size_t successor = edges[edge_index].to;
            --indegree[successor];
            if(indegree[successor] == 0) ready.push(successor);
        }
    }
    if(result.topological_order.size() != task_count)[[unlikely]]{
        throw std::invalid_argument("communication graph must be acyclic");
    }
    return result;
}

template<class Time>
[[nodiscard]] long double normalized_time(const Time& value, long double scale){
    return as_long_double(value) / scale;
}

template<class Time>
[[nodiscard]] std::vector<long double> average_processing_time(
    const std::vector<std::vector<Time>>& processing_time,
    long double scale
){
    std::vector<long double> average(processing_time.size(), 0);
    for(std::size_t task = 0; task < processing_time.size(); ++task){
        for(std::size_t processor = 0; processor < processing_time[task].size(); ++processor){
            const long double value = normalized_time(processing_time[task][processor], scale);
            const long double count = static_cast<long double>(processor + 1);
            average[task] += (value - average[task]) / count;
        }
    }
    return average;
}

template<class Time>
[[nodiscard]] std::vector<long double> upward_rank(
    const std::vector<DagCommunicationEdge<Time>>& edges,
    const ValidatedDag<Time>& dag,
    const std::vector<long double>& processing_rank
){
    std::vector<long double> rank(processing_rank.size(), 0);
    for(auto iterator = dag.topological_order.rbegin();
        iterator != dag.topological_order.rend(); ++iterator){
        const std::size_t task = *iterator;
        long double successor_rank = 0;
        for(const std::size_t edge_index: dag.outgoing_edges[task]){
            const auto& edge = edges[edge_index];
            const long double candidate = normalized_time(
                edge.communication_time, dag.scale
            ) + rank[edge.to];
            if(successor_rank < candidate) successor_rank = candidate;
        }
        rank[task] = processing_rank[task] + successor_rank;
    }
    return rank;
}

template<class Time>
[[nodiscard]] std::vector<long double> critical_path_priority(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<DagCommunicationEdge<Time>>& edges,
    const ValidatedDag<Time>& dag
){
    std::vector<long double> optimistic(processing_time.size(), 0);
    for(std::size_t task = 0; task < processing_time.size(); ++task){
        optimistic[task] = normalized_time(
            *std::min_element(processing_time[task].begin(), processing_time[task].end()),
            dag.scale
        );
    }
    return upward_rank(edges, dag, optimistic);
}

template<class Time>
[[nodiscard]] std::vector<long double> downward_rank(
    const std::vector<DagCommunicationEdge<Time>>& edges,
    const ValidatedDag<Time>& dag,
    const std::vector<long double>& average_processing
){
    std::vector<long double> rank(average_processing.size(), 0);
    for(const std::size_t task: dag.topological_order){
        for(const std::size_t edge_index: dag.outgoing_edges[task]){
            const auto& edge = edges[edge_index];
            const long double candidate = rank[task] + average_processing[task]
                + normalized_time(edge.communication_time, dag.scale);
            if(rank[edge.to] < candidate) rank[edge.to] = candidate;
        }
    }
    return rank;
}

template<class Time>
[[nodiscard]] std::vector<std::size_t> cpop_critical_path(
    const std::vector<DagCommunicationEdge<Time>>& edges,
    const ValidatedDag<Time>& dag,
    const std::vector<long double>& rank
){
    if(rank.empty()) return {};
    std::size_t current = rank.size();
    for(std::size_t task = 0; task < rank.size(); ++task){
        if(!dag.incoming_edges[task].empty()) continue;
        if(current == rank.size() || rank[current] < rank[task]
            || (rank[current] == rank[task] && task < current)) current = task;
    }
    std::vector<std::size_t> path;
    while(current != rank.size()){
        path.push_back(current);
        std::size_t next = rank.size();
        long double best = -1;
        for(const std::size_t edge_index: dag.outgoing_edges[current]){
            const auto& edge = edges[edge_index];
            const long double candidate = normalized_time(
                edge.communication_time, dag.scale
            ) + rank[edge.to];
            if(best < candidate || (best == candidate && edge.to < next)){
                best = candidate;
                next = edge.to;
            }
        }
        current = next;
    }
    return path;
}

template<class Time>
[[nodiscard]] bool try_dag_time_add(
    const Time& first,
    const Time& second,
    Time& result
){
    if constexpr(std::is_integral_v<Time>){
        if(second > std::numeric_limits<Time>::max() - first) return false;
        result = static_cast<Time>(first + second);
        return true;
    }else if constexpr(std::is_floating_point_v<Time>){
        result = first + second;
        return std::isfinite(result);
    }else{
        result = first + second;
        return true;
    }
}

template<class Time>
[[nodiscard]] bool earliest_insertion(
    const std::vector<DagTaskSlot<Time>>& slots,
    const Time& ready_time,
    const Time& duration,
    Time& start,
    Time& finish
){
    start = ready_time;
    for(const auto& slot: slots){
        if(start <= slot.start && duration <= slot.start - start){
            return try_dag_time_add(start, duration, finish);
        }
        if(start < slot.finish) start = slot.finish;
    }
    return try_dag_time_add(start, duration, finish);
}

template<class Time>
[[nodiscard]] bool processor_ready_time(
    std::size_t task,
    std::size_t processor,
    const std::vector<DagCommunicationEdge<Time>>& edges,
    const ValidatedDag<Time>& dag,
    const DagSchedule<Time>& schedule,
    Time& ready_time
){
    ready_time = Time{};
    for(const std::size_t edge_index: dag.incoming_edges[task]){
        const auto& edge = edges[edge_index];
        Time arrival = schedule.finish_time[edge.from];
        if(schedule.processor_of_task[edge.from] != processor){
            if(!try_dag_time_add(arrival, edge.communication_time, arrival)) return false;
        }
        if(ready_time < arrival) ready_time = arrival;
    }
    return true;
}

template<class Time>
DagSchedule<Time> schedule_dag_by_priority(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<DagCommunicationEdge<Time>>& edges,
    const ValidatedDag<Time>& dag,
    std::vector<long double> priority,
    const std::vector<std::size_t>& critical_path,
    std::size_t critical_processor
){
    const std::size_t task_count = processing_time.size();
    DagSchedule<Time> result;
    result.processor_of_task.assign(task_count, dag.processor_count);
    result.start_time.resize(task_count);
    result.finish_time.resize(task_count);
    result.scheduling_order.reserve(task_count);
    result.tasks_on_processor.resize(dag.processor_count);
    result.priority = std::move(priority);
    result.critical_path = critical_path;
    result.critical_processor = critical_processor;
    if(task_count == 0) return result;

    std::vector<unsigned char> is_critical(task_count, 0);
    for(const std::size_t task: critical_path) is_critical[task] = 1;
    std::vector<std::size_t> indegree(task_count, 0);
    for(std::size_t task = 0; task < task_count; ++task){
        indegree[task] = dag.incoming_edges[task].size();
    }
    struct ReadyCompare{
        const std::vector<long double>* priority{};
        bool operator()(std::size_t first, std::size_t second) const{
            if((*priority)[first] != (*priority)[second]){
                return (*priority)[first] < (*priority)[second];
            }
            return second < first;
        }
    };
    std::priority_queue<
        std::size_t,
        std::vector<std::size_t>,
        ReadyCompare
    > ready(ReadyCompare{&result.priority});
    for(std::size_t task = 0; task < task_count; ++task){
        if(indegree[task] == 0) ready.push(task);
    }

    while(!ready.empty()){
        const std::size_t task = ready.top();
        ready.pop();
        std::size_t best_processor = dag.processor_count;
        Time best_start{};
        Time best_finish{};
        const std::size_t processor_begin = is_critical[task] != 0
            ? critical_processor
            : 0;
        const std::size_t processor_end = is_critical[task] != 0
            ? critical_processor + 1
            : dag.processor_count;
        for(std::size_t processor = processor_begin; processor < processor_end; ++processor){
            Time task_ready{};
            if(!processor_ready_time(task, processor, edges, dag, result, task_ready)){
                continue;
            }
            Time start{};
            Time finish{};
            if(!earliest_insertion(
                result.tasks_on_processor[processor],
                task_ready,
                processing_time[task][processor],
                start,
                finish
            )) continue;
            if(best_processor == dag.processor_count
                || std::tie(finish, start, processor)
                    < std::tie(best_finish, best_start, best_processor)){
                best_processor = processor;
                best_start = start;
                best_finish = finish;
            }
        }
        if(best_processor == dag.processor_count)[[unlikely]]{
            throw std::overflow_error("task has no representable insertion slot");
        }
        result.processor_of_task[task] = best_processor;
        result.start_time[task] = best_start;
        result.finish_time[task] = best_finish;
        result.scheduling_order.push_back(task);
        auto& slots = result.tasks_on_processor[best_processor];
        const auto position = std::lower_bound(
            slots.begin(), slots.end(), std::tuple{best_start, best_finish, task},
            [](const DagTaskSlot<Time>& slot, const auto& key){
                return std::tie(slot.start, slot.finish, slot.task) < key;
            }
        );
        slots.insert(position, DagTaskSlot<Time>{task, best_start, best_finish});
        if(result.makespan < best_finish) result.makespan = best_finish;
        for(const std::size_t edge_index: dag.outgoing_edges[task]){
            const std::size_t successor = edges[edge_index].to;
            --indegree[successor];
            if(indegree[successor] == 0) ready.push(successor);
        }
    }
    return result;
}

template<class Time>
DagSchedule<Time> dag_schedule_impl(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<DagCommunicationEdge<Time>>& edges,
    DagPriorityRule rule
){
    const auto dag = validate_dag_input(processing_time, edges);
    if(processing_time.empty()){
        return schedule_dag_by_priority(
            processing_time, edges, dag, {}, {}, std::numeric_limits<std::size_t>::max()
        );
    }
    if(rule == DagPriorityRule::critical_path){
        auto priority = critical_path_priority(processing_time, edges, dag);
        return schedule_dag_by_priority(
            processing_time,
            edges,
            dag,
            std::move(priority),
            {},
            std::numeric_limits<std::size_t>::max()
        );
    }
    const auto average = average_processing_time(processing_time, dag.scale);
    auto upward = upward_rank(edges, dag, average);
    if(rule == DagPriorityRule::heft){
        return schedule_dag_by_priority(
            processing_time,
            edges,
            dag,
            std::move(upward),
            {},
            std::numeric_limits<std::size_t>::max()
        );
    }
    const auto downward = downward_rank(edges, dag, average);
    std::vector<long double> priority(upward.size());
    for(std::size_t task = 0; task < priority.size(); ++task){
        priority[task] = upward[task] + downward[task];
    }
    const auto path = cpop_critical_path(edges, dag, upward);
    std::size_t critical_processor = 0;
    long double best_cost = std::numeric_limits<long double>::infinity();
    for(std::size_t processor = 0; processor < dag.processor_count; ++processor){
        long double cost = 0;
        for(const std::size_t task: path){
            cost += normalized_time(processing_time[task][processor], dag.scale);
        }
        if(cost < best_cost){
            best_cost = cost;
            critical_processor = processor;
        }
    }
    return schedule_dag_by_priority(
        processing_time,
        edges,
        dag,
        std::move(priority),
        path,
        critical_processor
    );
}

}  // namespace scheduling_internal

template<class Time>
DagSchedule<Time> critical_path_priority_schedule(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<DagCommunicationEdge<Time>>& edges
){
    return scheduling_internal::dag_schedule_impl(
        processing_time, edges, scheduling_internal::DagPriorityRule::critical_path
    );
}

template<class Time>
DagSchedule<Time> heft_schedule(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<DagCommunicationEdge<Time>>& edges
){
    return scheduling_internal::dag_schedule_impl(
        processing_time, edges, scheduling_internal::DagPriorityRule::heft
    );
}

template<class Time>
DagSchedule<Time> cpop_schedule(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<DagCommunicationEdge<Time>>& edges
){
    return scheduling_internal::dag_schedule_impl(
        processing_time, edges, scheduling_internal::DagPriorityRule::cpop
    );
}

}  // namespace approximate::scheduling

#endif  // CPPLIB_SRC_APPROXIMATE_SCHEDULING_DAG_SCHEDULING_HPP_INCLUDED
