// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/scheduling/dag_scheduling.hpp"

namespace {

using Time = long long;
using Edge = approximate::scheduling::DagCommunicationEdge<Time>;
using Schedule = approximate::scheduling::DagSchedule<Time>;

bool valid_schedule(
    const Schedule& schedule,
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<Edge>& edges,
    bool expect_critical_path
){
    const std::size_t task_count = processing_time.size();
    const std::size_t processor_count = task_count == 0 ? 0 : processing_time.front().size();
    if(schedule.processor_of_task.size() != task_count
        || schedule.start_time.size() != task_count
        || schedule.finish_time.size() != task_count
        || schedule.scheduling_order.size() != task_count
        || schedule.tasks_on_processor.size() != processor_count
        || schedule.priority.size() != task_count) return false;

    std::vector<unsigned char> used_in_order(task_count, 0);
    std::vector<unsigned char> used_in_slots(task_count, 0);
    for(const std::size_t task: schedule.scheduling_order){
        if(task >= task_count || used_in_order[task] != 0) return false;
        used_in_order[task] = 1;
    }
    Time makespan = 0;
    for(std::size_t processor = 0; processor < processor_count; ++processor){
        Time previous_finish = 0;
        bool first = true;
        for(const auto& slot: schedule.tasks_on_processor[processor]){
            if(slot.task >= task_count || used_in_slots[slot.task] != 0
                || schedule.processor_of_task[slot.task] != processor
                || schedule.start_time[slot.task] != slot.start
                || schedule.finish_time[slot.task] != slot.finish
                || slot.finish < slot.start
                || slot.finish - slot.start != processing_time[slot.task][processor]){
                return false;
            }
            if(!first && slot.start < previous_finish) return false;
            first = false;
            previous_finish = slot.finish;
            used_in_slots[slot.task] = 1;
            makespan = std::max(makespan, slot.finish);
        }
    }
    if(!std::all_of(used_in_slots.begin(), used_in_slots.end(), [](unsigned char value){
        return value != 0;
    })) return false;
    for(const auto& edge: edges){
        Time required = schedule.finish_time[edge.from];
        if(schedule.processor_of_task[edge.from] != schedule.processor_of_task[edge.to]){
            required += edge.communication_time;
        }
        if(schedule.start_time[edge.to] < required) return false;
    }
    if(schedule.makespan != makespan) return false;

    const std::size_t no_processor = std::numeric_limits<std::size_t>::max();
    if(!expect_critical_path){
        return schedule.critical_path.empty() && schedule.critical_processor == no_processor;
    }
    if(task_count == 0){
        return schedule.critical_path.empty() && schedule.critical_processor == no_processor;
    }
    if(schedule.critical_path.empty() || schedule.critical_processor >= processor_count){
        return false;
    }
    for(const std::size_t task: schedule.critical_path){
        if(task >= task_count || schedule.processor_of_task[task] != schedule.critical_processor){
            return false;
        }
    }
    for(std::size_t index = 1; index < schedule.critical_path.size(); ++index){
        const std::size_t from = schedule.critical_path[index - 1];
        const std::size_t to = schedule.critical_path[index];
        if(std::none_of(edges.begin(), edges.end(), [from, to](const Edge& edge){
            return edge.from == from && edge.to == to;
        })) return false;
    }
    return true;
}

template<class Function>
bool throws_invalid(Function&& function){
    try{ function(); }catch(const std::invalid_argument&){ return true; }
    return false;
}

template<class Function>
bool throws_overflow(Function&& function){
    try{ function(); }catch(const std::overflow_error&){ return true; }
    return false;
}

bool boundary_tests(){
    using namespace approximate::scheduling;
    const std::vector<std::vector<Time>> empty;
    const auto empty_cp = critical_path_priority_schedule(empty, std::vector<Edge>{});
    const auto empty_heft = heft_schedule(empty, std::vector<Edge>{});
    const auto empty_cpop = cpop_schedule(empty, std::vector<Edge>{});
    if(!valid_schedule(empty_cp, empty, {}, false)
        || !valid_schedule(empty_heft, empty, {}, false)
        || !valid_schedule(empty_cpop, empty, {}, true)) return false;

    if(!throws_invalid([]{
        (void)heft_schedule(std::vector<std::vector<Time>>{{}}, std::vector<Edge>{});
    }) || !throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<Time>>{{1, 2}, {3}}, std::vector<Edge>{}
        );
    }) || !throws_invalid([]{
        (void)heft_schedule(std::vector<std::vector<Time>>{{-1}}, std::vector<Edge>{});
    }) || !throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<Time>>{{1}}, std::vector<Edge>{{0, 1, 0}}
        );
    }) || !throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<Time>>{{1}}, std::vector<Edge>{{0, 0, 0}}
        );
    }) || !throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<Time>>{{1}, {1}},
            std::vector<Edge>{{0, 1, 0}, {1, 0, 0}}
        );
    }) || !throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<Time>>{{1}, {1}}, std::vector<Edge>{{0, 1, -1}}
        );
    })) return false;

    using DoubleEdge = DagCommunicationEdge<double>;
    if(!throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<double>>{{std::numeric_limits<double>::quiet_NaN()}},
            std::vector<DoubleEdge>{}
        );
    }) || !throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<double>>{{std::numeric_limits<double>::infinity()}},
            std::vector<DoubleEdge>{}
        );
    }) || !throws_invalid([]{
        (void)heft_schedule(
            std::vector<std::vector<double>>{{1}, {1}},
            std::vector<DoubleEdge>{{0, 1, std::numeric_limits<double>::quiet_NaN()}}
        );
    })) return false;

    const std::vector<std::vector<Time>> overflow_matrix{
        {std::numeric_limits<Time>::max()}, {1},
    };
    const std::vector<Edge> chain{{0, 1, 0}};
    if(!throws_overflow([&]{ (void)critical_path_priority_schedule(overflow_matrix, chain); })
        || !throws_overflow([&]{ (void)heft_schedule(overflow_matrix, chain); })
        || !throws_overflow([&]{ (void)cpop_schedule(overflow_matrix, chain); })) return false;
    if(!throws_overflow([]{
        (void)heft_schedule(
            std::vector<std::vector<double>>{
                {std::numeric_limits<double>::max()},
                {std::numeric_limits<double>::max()},
            },
            std::vector<DoubleEdge>{{0, 1, 0}}
        );
    })) return false;

    const std::vector<std::vector<Time>> one_feasible_processor{
        {std::numeric_limits<Time>::max() - 1, std::numeric_limits<Time>::max() - 1},
        {1, 1},
    };
    const auto feasible = heft_schedule(one_feasible_processor, std::vector<Edge>{{0, 1, 5}});
    if(feasible.processor_of_task != std::vector<std::size_t>({0, 0})
        || feasible.makespan != std::numeric_limits<Time>::max()) return false;

    const auto processor_tie = heft_schedule(
        std::vector<std::vector<Time>>{{4, 4, 4}}, std::vector<Edge>{}
    );
    if(processor_tie.processor_of_task != std::vector<std::size_t>({0})) return false;

    const std::vector<std::vector<Time>> gap_matrix{{100, 1}, {15, 100}, {10, 10}};
    const std::vector<Edge> gap_edges{{0, 1, 20}};
    for(const Schedule& schedule: {
        critical_path_priority_schedule(gap_matrix, gap_edges),
        heft_schedule(gap_matrix, gap_edges),
    }){
        if(schedule.scheduling_order != std::vector<std::size_t>({0, 1, 2})
            || schedule.tasks_on_processor[0].size() != 2
            || schedule.tasks_on_processor[0][0].task != 2
            || schedule.tasks_on_processor[0][1].task != 1
            || schedule.start_time[2] != 0
            || schedule.start_time[1] != 21) return false;
    }
    return true;
}

void print_schedule(const Schedule& schedule){
    std::cout << schedule.makespan << ' ' << schedule.processor_of_task.size();
    for(const std::size_t processor: schedule.processor_of_task) std::cout << ' ' << processor;
    for(const Time value: schedule.start_time) std::cout << ' ' << value;
    for(const Time value: schedule.finish_time) std::cout << ' ' << value;
    for(const std::size_t task: schedule.scheduling_order) std::cout << ' ' << task;
    if(schedule.critical_processor == std::numeric_limits<std::size_t>::max()){
        std::cout << " -1";
    }else{
        std::cout << ' ' << schedule.critical_processor;
    }
    std::cout << ' ' << schedule.critical_path.size();
    for(const std::size_t task: schedule.critical_path) std::cout << ' ' << task;
    std::cout << '\n';
}

}  // namespace

int main(){
    using namespace approximate::scheduling;
    if(!boundary_tests()) return 2;
    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t task_count = 0;
        std::size_t processor_count = 0;
        std::size_t edge_count = 0;
        std::cin >> task_count >> processor_count >> edge_count;
        std::vector<std::vector<Time>> processing_time(
            task_count, std::vector<Time>(processor_count)
        );
        for(auto& row: processing_time){
            for(Time& value: row) std::cin >> value;
        }
        std::vector<Edge> edges(edge_count);
        for(auto& edge: edges){
            std::cin >> edge.from >> edge.to >> edge.communication_time;
        }
        const auto critical_path = critical_path_priority_schedule(processing_time, edges);
        const auto heft = heft_schedule(processing_time, edges);
        const auto cpop = cpop_schedule(processing_time, edges);
        if(!valid_schedule(critical_path, processing_time, edges, false)
            || !valid_schedule(heft, processing_time, edges, false)
            || !valid_schedule(cpop, processing_time, edges, true)) return 3;
        print_schedule(critical_path);
        print_schedule(heft);
        print_schedule(cpop);
    }
}
