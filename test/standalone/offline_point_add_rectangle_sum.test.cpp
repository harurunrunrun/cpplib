// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/offline_point_add_rectangle_sum.hpp"

void self_test(){
    OfflinePointAddRectangleSum<long long> solver;
    solver.add_initial_point(0, 0, 5);
    solver.add_initial_point(1, 1, 7);
    assert(solver.rectangle_sum(0, 0, 1, 1) == 0);
    assert(solver.rectangle_sum(0, 0, 2, 2) == 1);
    solver.add_point(0, 0, -3);
    solver.add_point(-1, 1, 4);
    assert(solver.rectangle_sum(-1, 0, 1, 2) == 2);
    assert(solver.rectangle_sum(2, 2, 2, 10) == 3);
    const std::vector<long long> expected = {5, 12, 6, 0};
    assert(solver.query_count() == expected.size());
    assert(solver.solve() == expected);
    assert(solver.solve() == expected);
}

int main(){
    self_test();

    int initial_count, operation_count;
    if(!(std::cin >> initial_count >> operation_count)) return 0;
    OfflinePointAddRectangleSum<long long> solver;
    while(initial_count-- > 0){
        long long x, y, value;
        std::cin >> x >> y >> value;
        solver.add_initial_point(x, y, value);
    }
    while(operation_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            long long x, y, value;
            std::cin >> x >> y >> value;
            solver.add_point(x, y, value);
        }else{
            long long x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            solver.rectangle_sum(x1, y1, x2, y2);
        }
    }
    for(const long long answer: solver.solve()) std::cout << answer << '\n';
}
