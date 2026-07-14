// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "../../src/structure/convex_hull_trick/offline_segment_add_get_min.hpp"

void self_test(){
    OfflineSegmentAddGetMin solver;
    solver.add_initial_segment(-2, 3, 2, 1);
    assert(solver.get_min(-2) == 0);
    assert(solver.get_min(3) == 1);
    solver.add_segment(3, 4, 0, 7);
    assert(solver.get_min(3) == 2);
    solver.add_segment(-1, -1, -100, -100);
    assert(solver.get_min(-1) == 3);
    const std::vector<std::optional<long long>> expected = {
        -3, std::nullopt, 7, -1
    };
    assert(solver.query_count() == expected.size());
    assert(solver.solve() == expected);
    assert(solver.solve() == expected);
}

int main(){
    self_test();

    int initial_count, operation_count;
    if(!(std::cin >> initial_count >> operation_count)) return 0;
    OfflineSegmentAddGetMin solver;
    while(initial_count-- > 0){
        long long left, right, slope, intercept;
        std::cin >> left >> right >> slope >> intercept;
        solver.add_initial_segment(left, right, slope, intercept);
    }
    while(operation_count-- > 0){
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
