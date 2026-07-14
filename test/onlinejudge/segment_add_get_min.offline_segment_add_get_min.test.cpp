// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/segment_add_get_min

#include <iostream>
#include <string>

#include "../../src/structure/convex_hull_trick/offline_segment_add_get_min.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int initial_count, query_count;
    std::cin >> initial_count >> query_count;
    OfflineSegmentAddGetMin solver;
    while(initial_count-- > 0){
        long long left, right, slope, intercept;
        std::cin >> left >> right >> slope >> intercept;
        solver.add_initial_segment(left, right, slope, intercept);
    }
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            long long left, right, slope, intercept;
            std::cin >> left >> right >> slope >> intercept;
            solver.add_segment(left, right, slope, intercept);
        }else{
            long long x;
            std::cin >> x;
            solver.get_min(x);
        }
    }

    for(const auto answer: solver.solve()){
        if(answer) std::cout << *answer << '\n';
        else std::cout << "INFINITY\n";
    }
}
