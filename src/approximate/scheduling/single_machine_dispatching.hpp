#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "scheduling_common.hpp"

namespace approximate::scheduling {

template<class Time>
struct SingleMachineSchedule{
    std::vector<std::size_t> order;
    std::vector<Time> start_time;
    std::vector<Time> completion_time;
    Time makespan{};
};

template<class Time>
struct PreemptiveSegment{
    std::size_t job{};
    Time start{};
    Time end{};
};

template<class Time>
struct PreemptiveSingleMachineSchedule{
    std::vector<PreemptiveSegment<Time>> segments;
    std::vector<Time> completion_time;
    Time makespan{};
};

namespace scheduling_internal {

template<class Time>
[[nodiscard]] std::vector<Time> normalized_release_time(
    const std::vector<Time>& release_time,
    std::size_t job_count
){
    if(release_time.empty()) return std::vector<Time>(job_count, Time{});
    if(release_time.size() != job_count)[[unlikely]]{
        throw std::invalid_argument("release times have a different size");
    }
    return release_time;
}

template<class Time>
void validate_processing_and_release(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& release_time
){
    if(processing_time.size() != release_time.size())[[unlikely]]{
        throw std::invalid_argument("processing and release times have different sizes");
    }
    for(std::size_t job = 0; job < processing_time.size(); ++job){
        validate_nonnegative_time(
            processing_time[job], "processing time must be nonnegative and finite"
        );
        validate_nonnegative_time(
            release_time[job], "release time must be nonnegative and finite"
        );
    }
}

template<class Time>
void validate_due_date(const std::vector<Time>& due_date, std::size_t job_count){
    if(due_date.size() != job_count)[[unlikely]]{
        throw std::invalid_argument("due dates have a different size");
    }
    for(const Time& value: due_date){
        if(!finite_value(value))[[unlikely]]{
            throw std::invalid_argument("due date must be finite");
        }
    }
}

template<class Time, class Better>
SingleMachineSchedule<Time> nonpreemptive_dispatch(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& release_time,
    Better&& better
){
    const std::size_t job_count = processing_time.size();
    SingleMachineSchedule<Time> result;
    result.order.reserve(job_count);
    result.start_time.resize(job_count);
    result.completion_time.resize(job_count);
    std::vector<unsigned char> done(job_count, 0);
    Time current{};
    for(std::size_t step = 0; step < job_count; ++step){
        std::size_t chosen = job_count;
        for(std::size_t job = 0; job < job_count; ++job){
            if(done[job] != 0 || current < release_time[job]) continue;
            if(chosen == job_count || better(job, chosen, current)) chosen = job;
        }
        if(chosen == job_count){
            for(std::size_t job = 0; job < job_count; ++job){
                if(done[job] == 0
                    && (chosen == job_count || release_time[job] < release_time[chosen])){
                    chosen = job;
                }
            }
            current = release_time[chosen];
            for(std::size_t job = 0; job < job_count; ++job){
                if(done[job] != 0 || current < release_time[job]) continue;
                if(better(job, chosen, current)) chosen = job;
            }
        }
        done[chosen] = 1;
        result.order.push_back(chosen);
        result.start_time[chosen] = current;
        current = checked_time_add(current, processing_time[chosen]);
        result.completion_time[chosen] = current;
    }
    result.makespan = current;
    return result;
}

template<class Time>
[[nodiscard]] long double available_processing_mean(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& release_time,
    const std::vector<unsigned char>& done,
    const Time& current
){
    long double mean = 0;
    std::size_t count = 0;
    for(std::size_t job = 0; job < processing_time.size(); ++job){
        if(done[job] != 0 || current < release_time[job]
            || !(Time{} < processing_time[job])) continue;
        ++count;
        const long double value = as_long_double(processing_time[job]);
        mean += (value - mean) / static_cast<long double>(count);
    }
    return mean;
}

}  // namespace scheduling_internal

template<class Time>
PreemptiveSingleMachineSchedule<Time> shortest_remaining_processing_time_schedule(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& release_time = {}
){
    using namespace scheduling_internal;
    const auto release = normalized_release_time(release_time, processing_time.size());
    validate_processing_and_release(processing_time, release);
    const std::size_t job_count = processing_time.size();
    PreemptiveSingleMachineSchedule<Time> result;
    result.completion_time.resize(job_count);
    if(job_count == 0) return result;

    std::vector<std::size_t> arrivals(job_count);
    std::iota(arrivals.begin(), arrivals.end(), std::size_t{0});
    std::stable_sort(arrivals.begin(), arrivals.end(), [&](std::size_t first, std::size_t second){
        return release[first] < release[second];
    });
    using Entry = std::pair<Time, std::size_t>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    std::vector<Time> remaining = processing_time;
    std::size_t next_arrival = 0;
    std::size_t completed = 0;
    Time current{};

    const auto push_arrivals = [&]{
        while(next_arrival < job_count && release[arrivals[next_arrival]] <= current){
            const std::size_t job = arrivals[next_arrival++];
            if(remaining[job] == Time{}){
                result.completion_time[job] = release[job];
                ++completed;
            }else{
                queue.emplace(remaining[job], job);
            }
        }
    };

    while(completed < job_count){
        if(queue.empty() && next_arrival < job_count && current < release[arrivals[next_arrival]]){
            current = release[arrivals[next_arrival]];
        }
        push_arrivals();
        if(queue.empty()) continue;
        const auto [queued_remaining, job] = queue.top();
        queue.pop();
        (void)queued_remaining;

        Time run = remaining[job];
        if(next_arrival < job_count){
            const Time until_arrival = release[arrivals[next_arrival]] - current;
            if(until_arrival < run) run = until_arrival;
        }
        const Time end = checked_time_add(current, run);
        if(!result.segments.empty() && result.segments.back().job == job
            && result.segments.back().end == current){
            result.segments.back().end = end;
        }else{
            result.segments.push_back({job, current, end});
        }
        remaining[job] -= run;
        current = end;
        push_arrivals();
        if(remaining[job] == Time{}){
            result.completion_time[job] = current;
            ++completed;
        }else{
            queue.emplace(remaining[job], job);
        }
    }
    for(const Time& completion: result.completion_time){
        if(result.makespan < completion) result.makespan = completion;
    }
    return result;
}

template<class Time>
SingleMachineSchedule<Time> least_work_remaining_schedule(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& release_time = {}
){
    using namespace scheduling_internal;
    const auto release = normalized_release_time(release_time, processing_time.size());
    validate_processing_and_release(processing_time, release);
    return nonpreemptive_dispatch(processing_time, release,
        [&](std::size_t first, std::size_t second, const Time&){
            return std::tie(processing_time[first], first)
                < std::tie(processing_time[second], second);
        }
    );
}

template<class Time>
SingleMachineSchedule<Time> minimum_slack_time_schedule(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& due_date,
    const std::vector<Time>& release_time = {}
){
    using namespace scheduling_internal;
    const auto release = normalized_release_time(release_time, processing_time.size());
    validate_processing_and_release(processing_time, release);
    validate_due_date(due_date, processing_time.size());
    return nonpreemptive_dispatch(processing_time, release,
        [&](std::size_t first, std::size_t second, const Time& current){
            const auto slack = [&](std::size_t job){
                return as_long_double(due_date[job]) - as_long_double(current)
                    - as_long_double(processing_time[job]);
            };
            return std::tuple{slack(first), first} < std::tuple{slack(second), second};
        }
    );
}

template<class Time>
SingleMachineSchedule<Time> critical_ratio_schedule(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& due_date,
    const std::vector<Time>& release_time = {}
){
    using namespace scheduling_internal;
    const auto release = normalized_release_time(release_time, processing_time.size());
    validate_processing_and_release(processing_time, release);
    validate_due_date(due_date, processing_time.size());
    return nonpreemptive_dispatch(processing_time, release,
        [&](std::size_t first, std::size_t second, const Time& current){
            if(processing_time[first] == Time{} || processing_time[second] == Time{}){
                if(processing_time[first] == Time{} && processing_time[second] != Time{}) return true;
                if(processing_time[first] != Time{} && processing_time[second] == Time{}) return false;
                return first < second;
            }
            const auto ratio = [&](std::size_t job){
                return (as_long_double(due_date[job]) - as_long_double(current))
                    / as_long_double(processing_time[job]);
            };
            return std::tuple{ratio(first), first} < std::tuple{ratio(second), second};
        }
    );
}

template<class Time>
SingleMachineSchedule<Time> highest_response_ratio_next_schedule(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& release_time = {}
){
    using namespace scheduling_internal;
    const auto release = normalized_release_time(release_time, processing_time.size());
    validate_processing_and_release(processing_time, release);
    return nonpreemptive_dispatch(processing_time, release,
        [&](std::size_t first, std::size_t second, const Time& current){
            if(processing_time[first] == Time{} || processing_time[second] == Time{}){
                if(processing_time[first] == Time{} && processing_time[second] != Time{}) return true;
                if(processing_time[first] != Time{} && processing_time[second] == Time{}) return false;
                return first < second;
            }
            const auto ratio = [&](std::size_t job){
                return (as_long_double(current) - as_long_double(release[job]))
                    / as_long_double(processing_time[job]);
            };
            const long double first_ratio = ratio(first);
            const long double second_ratio = ratio(second);
            return first_ratio != second_ratio ? second_ratio < first_ratio : first < second;
        }
    );
}

template<class Time, class Weight>
SingleMachineSchedule<Time> apparent_tardiness_cost_schedule(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& due_date,
    const std::vector<Weight>& weight,
    long double look_ahead = 2,
    const std::vector<Time>& release_time = {}
){
    using namespace scheduling_internal;
    const auto release = normalized_release_time(release_time, processing_time.size());
    validate_processing_and_release(processing_time, release);
    validate_due_date(due_date, processing_time.size());
    if(weight.size() != processing_time.size())[[unlikely]]{
        throw std::invalid_argument("ATC weights have a different size");
    }
    if(!(0 < look_ahead) || !std::isfinite(look_ahead))[[unlikely]]{
        throw std::invalid_argument("ATC look-ahead parameter must be positive and finite");
    }
    for(const Weight& value: weight){
        if(!positive_finite(value))[[unlikely]]{
            throw std::invalid_argument("ATC weights must be positive and finite");
        }
    }

    const std::size_t job_count = processing_time.size();
    SingleMachineSchedule<Time> result;
    result.order.reserve(job_count);
    result.start_time.resize(job_count);
    result.completion_time.resize(job_count);
    std::vector<unsigned char> done(job_count, 0);
    Time current{};
    for(std::size_t step = 0; step < job_count; ++step){
        bool any_available = false;
        for(std::size_t job = 0; job < job_count; ++job){
            if(done[job] == 0 && release[job] <= current) any_available = true;
        }
        if(!any_available){
            std::size_t next = job_count;
            for(std::size_t job = 0; job < job_count; ++job){
                if(done[job] == 0
                    && (next == job_count || release[job] < release[next])) next = job;
            }
            current = release[next];
        }
        const long double mean = available_processing_mean(
            processing_time, release, done, current
        );
        std::size_t chosen = job_count;
        long double chosen_score = -std::numeric_limits<long double>::infinity();
        for(std::size_t job = 0; job < job_count; ++job){
            if(done[job] != 0 || current < release[job]) continue;
            if(processing_time[job] == Time{}){
                if(chosen == job_count || processing_time[chosen] != Time{} || job < chosen){
                    chosen = job;
                }
                continue;
            }
            if(chosen != job_count && processing_time[chosen] == Time{}) continue;
            long double slack = as_long_double(due_date[job])
                - as_long_double(current) - as_long_double(processing_time[job]);
            if(!(0 < slack)) slack = 0;
            const long double penalty = mean == 0 ? 0 : slack / mean / look_ahead;
            const long double score = std::log(as_long_double(weight[job]))
                - std::log(as_long_double(processing_time[job])) - penalty;
            if(chosen == job_count || chosen_score < score
                || (chosen_score == score && job < chosen)){
                chosen = job;
                chosen_score = score;
            }
        }
        done[chosen] = 1;
        result.order.push_back(chosen);
        result.start_time[chosen] = current;
        current = checked_time_add(current, processing_time[chosen]);
        result.completion_time[chosen] = current;
    }
    result.makespan = current;
    return result;
}

template<class Time, class Weight>
SingleMachineSchedule<Time> apparent_tardiness_cost_with_setups_schedule(
    const std::vector<Time>& processing_time,
    const std::vector<Time>& due_date,
    const std::vector<Weight>& weight,
    const std::vector<std::vector<Time>>& setup_time,
    const std::vector<Time>& initial_setup_time,
    long double look_ahead = 2,
    long double setup_look_ahead = 1,
    const std::vector<Time>& release_time = {}
){
    using namespace scheduling_internal;
    const std::size_t job_count = processing_time.size();
    const auto release = normalized_release_time(release_time, job_count);
    validate_processing_and_release(processing_time, release);
    validate_due_date(due_date, job_count);
    if(weight.size() != job_count || initial_setup_time.size() != job_count
        || setup_time.size() != job_count)[[unlikely]]{
        throw std::invalid_argument("ATCS inputs have different sizes");
    }
    if(!(0 < look_ahead) || !(0 < setup_look_ahead)
        || !std::isfinite(look_ahead) || !std::isfinite(setup_look_ahead))[[unlikely]]{
        throw std::invalid_argument("ATCS parameters must be positive and finite");
    }
    for(std::size_t job = 0; job < job_count; ++job){
        if(!positive_finite(weight[job]))[[unlikely]]{
            throw std::invalid_argument("ATCS weights must be positive and finite");
        }
        validate_nonnegative_time(
            initial_setup_time[job], "ATCS setup time must be nonnegative and finite"
        );
        if(setup_time[job].size() != job_count)[[unlikely]]{
            throw std::invalid_argument("ATCS setup matrix must be square");
        }
        for(const Time& value: setup_time[job]){
            validate_nonnegative_time(
                value, "ATCS setup time must be nonnegative and finite"
            );
        }
    }

    SingleMachineSchedule<Time> result;
    result.order.reserve(job_count);
    result.start_time.resize(job_count);
    result.completion_time.resize(job_count);
    std::vector<unsigned char> done(job_count, 0);
    std::size_t previous = job_count;
    Time current{};
    for(std::size_t step = 0; step < job_count; ++step){
        bool any_available = false;
        for(std::size_t job = 0; job < job_count; ++job){
            if(done[job] == 0 && release[job] <= current) any_available = true;
        }
        if(!any_available){
            std::size_t next = job_count;
            for(std::size_t job = 0; job < job_count; ++job){
                if(done[job] == 0
                    && (next == job_count || release[job] < release[next])) next = job;
            }
            current = release[next];
        }
        const long double processing_mean = available_processing_mean(
            processing_time, release, done, current
        );
        long double setup_mean = 0;
        std::size_t setup_count = 0;
        for(std::size_t job = 0; job < job_count; ++job){
            if(done[job] != 0 || current < release[job]) continue;
            const Time& setup = previous == job_count
                ? initial_setup_time[job]
                : setup_time[previous][job];
            if(!(Time{} < setup)) continue;
            ++setup_count;
            const long double value = as_long_double(setup);
            setup_mean += (value - setup_mean) / static_cast<long double>(setup_count);
        }

        std::size_t chosen = job_count;
        long double chosen_score = -std::numeric_limits<long double>::infinity();
        Time chosen_setup{};
        for(std::size_t job = 0; job < job_count; ++job){
            if(done[job] != 0 || current < release[job]) continue;
            const Time& setup = previous == job_count
                ? initial_setup_time[job]
                : setup_time[previous][job];
            if(processing_time[job] == Time{}){
                if(chosen == job_count || processing_time[chosen] != Time{}
                    || setup < chosen_setup || (setup == chosen_setup && job < chosen)){
                    chosen = job;
                    chosen_setup = setup;
                }
                continue;
            }
            if(chosen != job_count && processing_time[chosen] == Time{}) continue;
            const long double ready = as_long_double(current) + as_long_double(setup);
            long double slack = as_long_double(due_date[job])
                - ready - as_long_double(processing_time[job]);
            if(!(0 < slack)) slack = 0;
            const long double tardiness_penalty = processing_mean == 0
                ? 0
                : slack / processing_mean / look_ahead;
            const long double setup_penalty = setup_mean == 0
                ? 0
                : as_long_double(setup) / setup_mean / setup_look_ahead;
            const long double score = std::log(as_long_double(weight[job]))
                - std::log(as_long_double(processing_time[job]))
                - tardiness_penalty - setup_penalty;
            if(chosen == job_count || chosen_score < score
                || (chosen_score == score && job < chosen)){
                chosen = job;
                chosen_score = score;
                chosen_setup = setup;
            }
        }
        done[chosen] = 1;
        result.order.push_back(chosen);
        current = checked_time_add(current, chosen_setup);
        result.start_time[chosen] = current;
        current = checked_time_add(current, processing_time[chosen]);
        result.completion_time[chosen] = current;
        previous = chosen;
    }
    result.makespan = current;
    return result;
}

}  // namespace approximate::scheduling
