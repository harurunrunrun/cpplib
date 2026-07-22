// competitive-verifier: PROBLEM https://www.spoj.com/problems/RENT/
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/other/dynamic_programming/maximum_weight_nonoverlapping_intervals.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n; std::cin>>n;
        std::vector<WeightedInterval> intervals; intervals.reserve(n);
        for(int i=0;i<n;i++){
            long long begin,duration,weight; std::cin>>begin>>duration>>weight;
            const __int128 end=static_cast<__int128>(begin)+duration;
            if(end<std::numeric_limits<long long>::min() ||
               end>std::numeric_limits<long long>::max())
                throw std::runtime_error("interval endpoint overflow");
            intervals.push_back({begin,static_cast<long long>(end),weight});
        }
        std::cout<<maximum_weight_nonoverlapping_intervals(
            std::move(intervals)
        )<<'\n'; }
}
