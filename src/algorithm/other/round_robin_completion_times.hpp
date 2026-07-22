#ifndef CPPLIB_SRC_ALGORITHM_OTHER_ROUND_ROBIN_COMPLETION_TIMES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_ROUND_ROBIN_COMPLETION_TIMES_HPP_INCLUDED

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../structure/other/fenwick_tree.hpp"

template<int MAX_TASKS = 50000>
std::vector<long long> round_robin_completion_times(
    const std::vector<long long>& processing_times
){
    const int task_count = static_cast<int>(processing_times.size());
    std::vector<std::pair<long long, int>> tasks;
    tasks.reserve(processing_times.size());
    for(int index = 0; index < task_count; ++index){
        const long long time = processing_times[static_cast<std::size_t>(index)];
        if(time <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: processing time must be positive "
                "(round_robin_completion_times)."
            );
        }
        tasks.emplace_back(time, index);
    }
    std::sort(tasks.begin(), tasks.end());

    auto active = std::make_unique<FenwickTree<int, MAX_TASKS>>(task_count);
    for(int index = 0; index < task_count; ++index) active->add(index, 1);

    std::vector<long long> answer(processing_times.size());
    long long completed_work = 0;
    int active_count = task_count;
    for(int begin = 0; begin < task_count;){
        int end = begin + 1;
        const long long time = tasks[static_cast<std::size_t>(begin)].first;
        while(end < task_count
            && tasks[static_cast<std::size_t>(end)].first == time){
            ++end;
        }
        const long long before_last_round = completed_work
            + static_cast<long long>(active_count) * (time - 1);
        for(int position = begin; position < end; ++position){
            const int index = tasks[static_cast<std::size_t>(position)].second;
            answer[static_cast<std::size_t>(index)] =
                before_last_round + active->prefix_sum(index + 1);
        }
        for(int position = begin; position < end; ++position){
            active->add(tasks[static_cast<std::size_t>(position)].second, -1);
        }
        const int completed = end - begin;
        completed_work += time * static_cast<long long>(completed);
        active_count -= completed;
        begin = end;
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_ROUND_ROBIN_COMPLETION_TIMES_HPP_INCLUDED
