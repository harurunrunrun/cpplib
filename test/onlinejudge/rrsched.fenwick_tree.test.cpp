// competitive-verifier: PROBLEM https://www.spoj.com/problems/RRSCHED/

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int task_count;
    std::cin >> task_count;

    std::vector<std::pair<long long, int>> tasks;
    tasks.reserve(static_cast<std::size_t>(task_count));
    for(int index = 0; index < task_count; index++){
        long long processing_time;
        std::cin >> processing_time;
        tasks.emplace_back(processing_time, index);
    }
    std::sort(tasks.begin(), tasks.end());

    auto active = std::make_unique<FenwickTree<int, 50000>>(task_count);
    for(int index = 0; index < task_count; index++){
        active->add(index, 1);
    }

    std::vector<long long> answer(static_cast<std::size_t>(task_count));
    long long completed_work = 0;
    int active_count = task_count;
    int begin = 0;
    while(begin < task_count){
        int end = begin + 1;
        const long long processing_time = tasks[begin].first;
        while(end < task_count && tasks[end].first == processing_time){
            end++;
        }

        const long long before_round = completed_work
            + static_cast<long long>(active_count) * (processing_time - 1);
        for(int position = begin; position < end; position++){
            const int index = tasks[position].second;
            answer[index] = before_round + active->prefix_sum(index + 1);
        }
        for(int position = begin; position < end; position++){
            active->add(tasks[position].second, -1);
        }
        const int completed_count = end - begin;
        completed_work += processing_time * static_cast<long long>(completed_count);
        active_count -= completed_count;
        begin = end;
    }

    for(const long long elapsed : answer){
        std::cout << elapsed << '\n';
    }
}
