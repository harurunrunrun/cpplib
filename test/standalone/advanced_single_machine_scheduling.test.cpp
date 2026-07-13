#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/scheduling/single_machine_dispatching.hpp"

namespace {

using approximate::scheduling::PreemptiveSingleMachineSchedule;
using approximate::scheduling::SingleMachineSchedule;

bool valid_schedule(
    const SingleMachineSchedule<long long>& schedule,
    const std::vector<long long>& processing,
    const std::vector<long long>& release
){
    const std::size_t size = processing.size();
    if(schedule.order.size() != size || schedule.start_time.size() != size
        || schedule.completion_time.size() != size) return false;
    std::vector<unsigned char> used(size, 0);
    long long current = 0;
    for(const std::size_t job: schedule.order){
        if(job >= size || used[job] != 0) return false;
        used[job] = 1;
        current = std::max(current, release[job]);
        if(schedule.start_time[job] != current) return false;
        current += processing[job];
        if(schedule.completion_time[job] != current) return false;
    }
    return schedule.makespan == current;
}

bool valid_atcs_schedule(
    const SingleMachineSchedule<long long>& schedule,
    const std::vector<long long>& processing,
    const std::vector<long long>& release,
    const std::vector<long long>& initial_setup,
    const std::vector<std::vector<long long>>& setup
){
    const std::size_t size = processing.size();
    if(schedule.order.size() != size) return false;
    std::vector<unsigned char> used(size, 0);
    long long current = 0;
    std::size_t previous = size;
    for(const std::size_t job: schedule.order){
        if(job >= size || used[job] != 0) return false;
        used[job] = 1;
        current = std::max(current, release[job]);
        current += previous == size ? initial_setup[job] : setup[previous][job];
        if(schedule.start_time[job] != current) return false;
        current += processing[job];
        if(schedule.completion_time[job] != current) return false;
        previous = job;
    }
    return schedule.makespan == current;
}

bool valid_srpt(
    const PreemptiveSingleMachineSchedule<long long>& schedule,
    const std::vector<long long>& processing,
    const std::vector<long long>& release
){
    if(schedule.completion_time.size() != processing.size()) return false;
    std::vector<long long> executed(processing.size(), 0);
    std::vector<long long> last_end(processing.size(), -1);
    long long previous_end = 0;
    for(const auto& segment: schedule.segments){
        if(segment.job >= processing.size() || segment.end <= segment.start
            || segment.start < release[segment.job] || segment.start < previous_end) return false;
        executed[segment.job] += segment.end - segment.start;
        last_end[segment.job] = segment.end;
        previous_end = segment.end;
    }
    long long makespan = 0;
    for(std::size_t job = 0; job < processing.size(); ++job){
        if(executed[job] != processing[job]) return false;
        const long long completion = processing[job] == 0 ? release[job] : last_end[job];
        if(schedule.completion_time[job] != completion) return false;
        makespan = std::max(makespan, completion);
    }
    return schedule.makespan == makespan;
}

void print_schedule(const SingleMachineSchedule<long long>& schedule){
    std::cout << schedule.makespan << ' ' << schedule.order.size();
    for(const std::size_t job: schedule.order) std::cout << ' ' << job;
    std::cout << '\n';
}

template<class Function>
bool throws_invalid(Function&& function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

bool boundary_tests(){
    using namespace approximate::scheduling;
    if(!shortest_remaining_processing_time_schedule(std::vector<long long>{}).segments.empty()){
        return false;
    }
    if(!throws_invalid([]{
        (void)least_work_remaining_schedule(
            std::vector<long long>{1}, std::vector<long long>{0, 1}
        );
    })) return false;
    if(!throws_invalid([]{
        (void)minimum_slack_time_schedule(
            std::vector<long long>{1}, std::vector<long long>{}
        );
    })) return false;
    if(!throws_invalid([]{
        (void)critical_ratio_schedule(
            std::vector<long long>{-1}, std::vector<long long>{1}
        );
    })) return false;
    if(!throws_invalid([]{
        (void)apparent_tardiness_cost_schedule(
            std::vector<long long>{1},
            std::vector<long long>{1},
            std::vector<long long>{0}
        );
    })) return false;
    if(!throws_invalid([]{
        (void)apparent_tardiness_cost_with_setups_schedule(
            std::vector<long long>{1, 2},
            std::vector<long long>{3, 4},
            std::vector<long long>{1, 1},
            std::vector<std::vector<long long>>{{0}, {0, 0}},
            std::vector<long long>{0, 0}
        );
    })) return false;
    bool overflow = false;
    try{
        (void)least_work_remaining_schedule(std::vector<long long>{
            std::numeric_limits<long long>::max(), 1,
        });
    }catch(const std::overflow_error&){
        overflow = true;
    }
    if(!overflow) return false;

    const long long maximum = std::numeric_limits<long long>::max();
    const auto candidate_overflow = apparent_tardiness_cost_with_setups_schedule(
        std::vector<long long>{maximum - 10, 1, 1},
        std::vector<long long>{0, maximum, 0},
        std::vector<long long>{maximum, 1, 1},
        std::vector<std::vector<long long>>{
            {0, 20, 0},
            {0, 0, 0},
            {0, 0, 0},
        },
        std::vector<long long>{0, 0, 0}
    );
    return candidate_overflow.order == std::vector<std::size_t>{0, 2, 1}
        && candidate_overflow.makespan == maximum - 8;
}

}  // namespace

int main(){
    using namespace approximate::scheduling;
    if(!boundary_tests()) return 2;
    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t job_count = 0;
        std::cin >> job_count;
        std::vector<long long> processing(job_count);
        std::vector<long long> release(job_count);
        std::vector<long long> due(job_count);
        std::vector<long long> weight(job_count);
        std::vector<long long> initial_setup(job_count);
        for(long long& value: processing) std::cin >> value;
        for(long long& value: release) std::cin >> value;
        for(long long& value: due) std::cin >> value;
        for(long long& value: weight) std::cin >> value;
        for(long long& value: initial_setup) std::cin >> value;
        std::vector<std::vector<long long>> setup(
            job_count, std::vector<long long>(job_count)
        );
        for(auto& row: setup){
            for(long long& value: row) std::cin >> value;
        }

        const auto srpt = shortest_remaining_processing_time_schedule(processing, release);
        const auto lwr = least_work_remaining_schedule(processing, release);
        const auto slack = minimum_slack_time_schedule(processing, due, release);
        const auto ratio = critical_ratio_schedule(processing, due, release);
        const auto hrrn = highest_response_ratio_next_schedule(processing, release);
        const auto atc = apparent_tardiness_cost_schedule(
            processing, due, weight, 2, release
        );
        const auto atcs = apparent_tardiness_cost_with_setups_schedule(
            processing, due, weight, setup, initial_setup, 2, 1, release
        );
        if(!valid_srpt(srpt, processing, release)
            || !valid_schedule(lwr, processing, release)
            || !valid_schedule(slack, processing, release)
            || !valid_schedule(ratio, processing, release)
            || !valid_schedule(hrrn, processing, release)
            || !valid_schedule(atc, processing, release)
            || !valid_atcs_schedule(
                atcs, processing, release, initial_setup, setup
            )) return 3;

        std::cout << srpt.makespan << ' ' << srpt.segments.size();
        for(const auto& segment: srpt.segments){
            std::cout << ' ' << segment.job << ' ' << segment.start << ' ' << segment.end;
        }
        std::cout << '\n';
        print_schedule(lwr);
        print_schedule(slack);
        print_schedule(ratio);
        print_schedule(hrrn);
        print_schedule(atc);
        print_schedule(atcs);
    }
}
