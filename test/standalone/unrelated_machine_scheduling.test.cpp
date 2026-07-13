#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/scheduling/unrelated_machine.hpp"

namespace {

bool valid_schedule(
    const approximate::scheduling::UnrelatedMachineSchedule<long long>& schedule,
    const std::vector<std::vector<long long>>& processing_time,
    std::size_t machine_count
){
    const std::size_t job_count = processing_time.size();
    if(schedule.machine_of_job.size() != job_count
        || schedule.start_time.size() != job_count
        || schedule.completion_time.size() != job_count
        || schedule.jobs_on_machine.size() != machine_count
        || schedule.machine_load.size() != machine_count) return false;
    std::vector<unsigned char> used(job_count, 0);
    std::vector<long long> load(machine_count, 0);
    for(std::size_t machine = 0; machine < machine_count; ++machine){
        for(const std::size_t job: schedule.jobs_on_machine[machine]){
            if(job >= job_count || used[job] != 0
                || schedule.machine_of_job[job] != machine) return false;
            used[job] = 1;
            if(schedule.start_time[job] != load[machine]) return false;
            load[machine] += processing_time[job][machine];
            if(schedule.completion_time[job] != load[machine]) return false;
        }
    }
    if(!std::all_of(used.begin(), used.end(), [](unsigned char value){
        return value != 0;
    })) return false;
    long long makespan = 0;
    for(std::size_t machine = 0; machine < machine_count; ++machine){
        if(load[machine] != schedule.machine_load[machine]) return false;
        makespan = std::max(makespan, load[machine]);
    }
    return makespan == schedule.makespan;
}

template<class Function>
bool throws_invalid(Function&& function){
    try{ function(); }catch(const std::invalid_argument&){ return true; }
    return false;
}

bool boundary_tests(){
    using namespace approximate::scheduling;
    if(min_min_schedule(std::vector<std::vector<long long>>{}).makespan != 0) return false;
    if(!throws_invalid([]{ (void)max_min_schedule(std::vector<std::vector<long long>>{{}}); })) return false;
    if(!throws_invalid([]{ (void)sufferage_schedule(std::vector<std::vector<long long>>{{1, 2}, {3}}); })) return false;
    if(!throws_invalid([]{ (void)min_min_schedule(std::vector<std::vector<long long>>{{-1}}); })) return false;
    if(!throws_invalid([]{
        (void)unrelated_machine_schedule(
            std::vector<std::vector<long long>>{}, static_cast<UnrelatedMachineRule>(99)
        );
    })) return false;
    bool overflow = false;
    try{
        (void)min_min_schedule(std::vector<std::vector<long long>>{
            {std::numeric_limits<long long>::max()}, {1},
        });
    }catch(const std::overflow_error&){ overflow = true; }
    return overflow;
}

void print_schedule(const approximate::scheduling::UnrelatedMachineSchedule<long long>& schedule){
    std::cout << schedule.makespan << ' ' << schedule.machine_of_job.size();
    for(const std::size_t machine: schedule.machine_of_job) std::cout << ' ' << machine;
    std::cout << '\n';
}

}  // namespace

int main(){
    using namespace approximate::scheduling;
    if(!boundary_tests()) return 2;
    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t job_count = 0;
        std::size_t machine_count = 0;
        std::cin >> job_count >> machine_count;
        std::vector<std::vector<long long>> processing_time(job_count, std::vector<long long>(machine_count));
        for(auto& row: processing_time) for(long long& value: row) std::cin >> value;
        const auto min_min = min_min_schedule(processing_time);
        const auto max_min = max_min_schedule(processing_time);
        const auto sufferage = sufferage_schedule(processing_time);
        if(!valid_schedule(min_min, processing_time, machine_count)
            || !valid_schedule(max_min, processing_time, machine_count)
            || !valid_schedule(sufferage, processing_time, machine_count)) return 3;
        if(min_min.machine_of_job != unrelated_machine_schedule(
            processing_time, UnrelatedMachineRule::min_min
        ).machine_of_job) return 4;
        print_schedule(min_min);
        print_schedule(max_min);
        print_schedule(sufferage);
    }
}
