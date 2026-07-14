// competitive-verifier: PROBLEM https://www.spoj.com/problems/RRSCHED/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/round_robin_completion_times.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int task_count;
    std::cin >> task_count;
    std::vector<long long> processing_times(static_cast<std::size_t>(task_count));
    for(long long& time: processing_times) std::cin >> time;
    for(const long long elapsed: round_robin_completion_times(processing_times)){
        std::cout << elapsed << '\n';
    }
}
