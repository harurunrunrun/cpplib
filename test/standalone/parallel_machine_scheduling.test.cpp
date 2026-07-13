#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/scheduling/parallel_machine.hpp"
#include "../../src/approximate/scheduling/single_machine.hpp"

namespace {

template<class Time>
bool valid_schedule(
    const approximate::scheduling::ParallelMachineSchedule<Time>& schedule,
    const std::vector<Time>& processing_time,
    std::size_t machine_count
){
    if(schedule.machine_of_job.size() != processing_time.size()) return false;
    if(schedule.jobs_on_machine.size() != machine_count) return false;
    if(schedule.machine_load.size() != machine_count) return false;
    std::vector<unsigned char> seen(processing_time.size(), 0);
    std::vector<Time> load(machine_count, Time{});
    for(std::size_t machine = 0; machine < machine_count; ++machine){
        for(const std::size_t job: schedule.jobs_on_machine[machine]){
            if(job >= processing_time.size() || seen[job] != 0) return false;
            seen[job] = 1;
            if(schedule.machine_of_job[job] != machine) return false;
            load[machine] += processing_time[job];
        }
    }
    Time makespan{};
    for(std::size_t machine = 0; machine < machine_count; ++machine){
        if(load[machine] != schedule.machine_load[machine]) return false;
        makespan = std::max(makespan, load[machine]);
    }
    return std::all_of(seen.begin(), seen.end(), [](unsigned char value){
        return value != 0;
    }) && makespan == schedule.makespan;
}

template<class Range>
void print_range(const Range& range){
    std::cout << range.size();
    for(const auto& value: range) std::cout << ' ' << value;
    std::cout << '\n';
}

}  // namespace

int main(){
    using approximate::scheduling::earliest_due_date_order;
    using approximate::scheduling::fifo_schedule;
    using approximate::scheduling::lifo_schedule;
    using approximate::scheduling::lpt_schedule;
    using approximate::scheduling::spt_schedule;
    using approximate::scheduling::weighted_shortest_processing_time_order;

    bool rejected = false;
    try{
        (void)fifo_schedule(std::vector<long long>{1}, 0);
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 2;
    rejected = false;
    try{
        (void)weighted_shortest_processing_time_order(
            std::vector<long long>{1}, std::vector<long long>{0}
        );
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 3;

    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t job_count = 0;
        std::size_t machine_count = 0;
        std::cin >> job_count >> machine_count;
        std::vector<long long> processing_time(job_count);
        std::vector<long long> weight(job_count);
        std::vector<long long> due_date(job_count);
        for(long long& value: processing_time) std::cin >> value;
        for(long long& value: weight) std::cin >> value;
        for(long long& value: due_date) std::cin >> value;

        const auto fifo = fifo_schedule(processing_time, machine_count);
        const auto lifo = lifo_schedule(processing_time, machine_count);
        const auto lpt = lpt_schedule(processing_time, machine_count);
        const auto spt = spt_schedule(processing_time, machine_count);
        if(!valid_schedule(fifo, processing_time, machine_count)
            || !valid_schedule(lifo, processing_time, machine_count)
            || !valid_schedule(lpt, processing_time, machine_count)
            || !valid_schedule(spt, processing_time, machine_count)) return 4;

        std::cout << fifo.makespan << ' ' << lifo.makespan << ' '
                  << lpt.makespan << ' ' << spt.makespan << '\n';
        print_range(weighted_shortest_processing_time_order(processing_time, weight));
        print_range(earliest_due_date_order(due_date));
    }
}
