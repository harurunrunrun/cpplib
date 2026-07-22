#ifndef CPPLIB_SRC_APPROXIMATE_SCHEDULING_FLOW_SHOP_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SCHEDULING_FLOW_SHOP_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::scheduling {

template<class Time>
Time flow_shop_makespan(
    const std::vector<std::vector<Time>>& processing_time,
    const std::vector<std::size_t>& order
){
    std::vector<unsigned char> used(processing_time.size(), 0);
    for(const std::size_t job: order){
        if(job >= processing_time.size() || used[job] != 0)[[unlikely]]{
            throw std::invalid_argument(
                "flow-shop order contains an invalid or duplicate job"
            );
        }
        used[job] = 1;
    }
    const std::size_t machine_count = processing_time.empty()
        ? 0
        : processing_time.front().size();
    for(const auto& job: processing_time){
        if(job.size() != machine_count)[[unlikely]]{
            throw std::invalid_argument("flow-shop matrix must be rectangular");
        }
        for(const Time& duration: job){
            if(duration < Time{})[[unlikely]]{
                throw std::invalid_argument("processing time must be nonnegative");
            }
        }
    }
    if(machine_count == 0) return Time{};
    std::vector<Time> completion(machine_count, Time{});
    for(const std::size_t job: order){
        completion[0] = completion[0] + processing_time[job][0];
        for(std::size_t machine = 1; machine < machine_count; ++machine){
            if(completion[machine] < completion[machine - 1]){
                completion[machine] = completion[machine - 1];
            }
            completion[machine] = completion[machine]
                + processing_time[job][machine];
        }
    }
    return completion.back();
}

template<class Time>
std::vector<std::size_t> johnson_two_machine_order(
    const std::vector<Time>& first_machine,
    const std::vector<Time>& second_machine
){
    if(first_machine.size() != second_machine.size())[[unlikely]]{
        throw std::invalid_argument("Johnson inputs have different sizes");
    }
    std::vector<std::size_t> early;
    std::vector<std::size_t> late;
    early.reserve(first_machine.size());
    late.reserve(first_machine.size());
    for(std::size_t job = 0; job < first_machine.size(); ++job){
        if(first_machine[job] < Time{} || second_machine[job] < Time{})[[unlikely]]{
            throw std::invalid_argument("processing time must be nonnegative");
        }
        if(first_machine[job] <= second_machine[job]){
            early.push_back(job);
        }else{
            late.push_back(job);
        }
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

template<class Time>
std::vector<std::size_t> neh_flow_shop_order(
    const std::vector<std::vector<Time>>& processing_time
){
    const std::size_t job_count = processing_time.size();
    if(job_count == 0) return {};
    const std::size_t machine_count = processing_time.front().size();
    std::vector<Time> total(job_count, Time{});
    for(std::size_t job = 0; job < job_count; ++job){
        if(processing_time[job].size() != machine_count)[[unlikely]]{
            throw std::invalid_argument("flow-shop matrix must be rectangular");
        }
        for(const Time& duration: processing_time[job]){
            if(duration < Time{})[[unlikely]]{
                throw std::invalid_argument("processing time must be nonnegative");
            }
            total[job] = total[job] + duration;
        }
    }
    std::vector<std::size_t> jobs(job_count);
    std::iota(jobs.begin(), jobs.end(), std::size_t{0});
    std::stable_sort(jobs.begin(), jobs.end(), [&](std::size_t first, std::size_t second){
        return total[second] < total[first];
    });

    std::vector<std::size_t> order;
    order.reserve(job_count);
    for(const std::size_t job: jobs){
        std::vector<std::size_t> best;
        Time best_makespan{};
        bool initialized = false;
        for(std::size_t position = 0; position <= order.size(); ++position){
            std::vector<std::size_t> candidate;
            candidate.reserve(order.size() + 1);
            for(std::size_t index = 0; index < position; ++index){
                candidate.push_back(order[index]);
            }
            candidate.push_back(job);
            for(
                std::size_t index = position;
                index < order.size();
                ++index
            ){
                candidate.push_back(order[index]);
            }
            const Time value = flow_shop_makespan(processing_time, candidate);
            if(!initialized || value < best_makespan){
                initialized = true;
                best_makespan = value;
                best = std::move(candidate);
            }
        }
        order = std::move(best);
    }
    return order;
}

}  // namespace approximate::scheduling

#endif  // CPPLIB_SRC_APPROXIMATE_SCHEDULING_FLOW_SHOP_HPP_INCLUDED
