#pragma once

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "scheduling_common.hpp"

namespace approximate::scheduling {

enum class UnrelatedMachineRule{
    min_min,
    max_min,
    sufferage,
};

template<class Time>
struct UnrelatedMachineSchedule{
    std::vector<std::size_t> machine_of_job;
    std::vector<std::vector<std::size_t>> jobs_on_machine;
    std::vector<Time> start_time;
    std::vector<Time> completion_time;
    std::vector<Time> machine_load;
    Time makespan{};
};

namespace scheduling_internal {

template<class Time>
[[nodiscard]] bool try_time_add(
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
[[nodiscard]] std::size_t validate_unrelated_matrix(
    const std::vector<std::vector<Time>>& processing_time
){
    if(processing_time.empty()) return 0;
    const std::size_t machine_count = processing_time.front().size();
    if(machine_count == 0)[[unlikely]]{
        throw std::invalid_argument("unrelated-machine scheduling requires a machine");
    }
    for(const auto& row: processing_time){
        if(row.size() != machine_count)[[unlikely]]{
            throw std::invalid_argument("unrelated-machine matrix must be rectangular");
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
struct UnrelatedJobChoice{
    std::size_t machine{};
    Time best_completion{};
    Time second_completion{};
    bool has_second{};
};

template<class Time>
[[nodiscard]] UnrelatedJobChoice<Time> unrelated_job_choice(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& machine_load
){
    UnrelatedJobChoice<Time> choice;
    bool initialized = false;
    for(std::size_t machine = 0; machine < machine_load.size(); ++machine){
        Time completion{};
        if(!try_time_add(machine_load[machine], processing_time[machine], completion)){
            continue;
        }
        if(!initialized || completion < choice.best_completion
            || (completion == choice.best_completion && machine < choice.machine)){
            if(initialized){
                choice.second_completion = choice.best_completion;
                choice.has_second = true;
            }
            choice.best_completion = completion;
            choice.machine = machine;
            initialized = true;
        }else if(!choice.has_second || completion < choice.second_completion){
            choice.second_completion = completion;
            choice.has_second = true;
        }
    }
    if(!initialized)[[unlikely]]{
        throw std::overflow_error("every completion time overflows");
    }
    return choice;
}

}  // namespace scheduling_internal

template<class Time>
UnrelatedMachineSchedule<Time> unrelated_machine_schedule(
    const std::vector<std::vector<Time>>& processing_time,
    UnrelatedMachineRule rule
){
    using namespace scheduling_internal;
    if(rule != UnrelatedMachineRule::min_min
        && rule != UnrelatedMachineRule::max_min
        && rule != UnrelatedMachineRule::sufferage)[[unlikely]]{
        throw std::invalid_argument("unknown unrelated-machine scheduling rule");
    }
    const std::size_t job_count = processing_time.size();
    const std::size_t machine_count = validate_unrelated_matrix(processing_time);
    UnrelatedMachineSchedule<Time> result;
    result.machine_of_job.resize(job_count);
    result.start_time.resize(job_count);
    result.completion_time.resize(job_count);
    result.jobs_on_machine.resize(machine_count);
    result.machine_load.assign(machine_count, Time{});
    if(job_count == 0) return result;

    std::vector<unsigned char> done(job_count, 0);
    for(std::size_t step = 0; step < job_count; ++step){
        std::size_t chosen = job_count;
        UnrelatedJobChoice<Time> chosen_choice;
        long double chosen_sufferage = -1;
        for(std::size_t job = 0; job < job_count; ++job){
            if(done[job] != 0) continue;
            const auto choice = unrelated_job_choice(processing_time[job], result.machine_load);
            long double sufferage = std::numeric_limits<long double>::infinity();
            if(choice.has_second){
                sufferage = as_long_double(choice.second_completion)
                    - as_long_double(choice.best_completion);
            }
            bool better = chosen == job_count;
            if(!better && rule == UnrelatedMachineRule::min_min){
                better = std::tie(choice.best_completion, job)
                    < std::tie(chosen_choice.best_completion, chosen);
            }else if(!better && rule == UnrelatedMachineRule::max_min){
                better = chosen_choice.best_completion < choice.best_completion
                    || (chosen_choice.best_completion == choice.best_completion && job < chosen);
            }else if(!better && rule == UnrelatedMachineRule::sufferage){
                better = chosen_sufferage < sufferage
                    || (chosen_sufferage == sufferage
                        && std::tie(choice.best_completion, job)
                            < std::tie(chosen_choice.best_completion, chosen));
            }
            if(better){
                chosen = job;
                chosen_choice = choice;
                chosen_sufferage = sufferage;
            }
        }
        done[chosen] = 1;
        const std::size_t machine = chosen_choice.machine;
        result.machine_of_job[chosen] = machine;
        result.start_time[chosen] = result.machine_load[machine];
        result.completion_time[chosen] = chosen_choice.best_completion;
        result.machine_load[machine] = chosen_choice.best_completion;
        result.jobs_on_machine[machine].push_back(chosen);
    }
    for(const Time& load: result.machine_load){
        if(result.makespan < load) result.makespan = load;
    }
    return result;
}

template<class Time>
UnrelatedMachineSchedule<Time> min_min_schedule(
    const std::vector<std::vector<Time>>& processing_time
){
    return unrelated_machine_schedule(processing_time, UnrelatedMachineRule::min_min);
}

template<class Time>
UnrelatedMachineSchedule<Time> max_min_schedule(
    const std::vector<std::vector<Time>>& processing_time
){
    return unrelated_machine_schedule(processing_time, UnrelatedMachineRule::max_min);
}

template<class Time>
UnrelatedMachineSchedule<Time> sufferage_schedule(
    const std::vector<std::vector<Time>>& processing_time
){
    return unrelated_machine_schedule(processing_time, UnrelatedMachineRule::sufferage);
}

}  // namespace approximate::scheduling
