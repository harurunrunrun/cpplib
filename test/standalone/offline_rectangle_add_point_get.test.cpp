// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/offline_rectangle_add_point_get.hpp"

void self_test(){
    OfflineRectangleAddPointGet<long long> solver;
    solver.add_initial_rectangle(2, 2, -1, -1, 5);
    assert(solver.get_point(-1, -1) == 0);
    assert(solver.get_point(2, 0) == 1);
    solver.add_rectangle(0, 0, 0, 3, 100);
    solver.add_rectangle(0, 0, 1, 1, -2);
    assert(solver.get_point(0, 0) == 2);
    const std::vector<long long> expected = {5, 0, 3};
    assert(solver.query_count() == expected.size());
    assert(solver.solve() == expected);
    assert(solver.solve() == expected);
}

int main(){
    self_test();

    int initial_count, operation_count;
    if(!(std::cin >> initial_count >> operation_count)) return 0;
    OfflineRectangleAddPointGet<long long> solver;
    while(initial_count-- > 0){
        long long x1, y1, x2, y2, value;
        std::cin >> x1 >> y1 >> x2 >> y2 >> value;
        solver.add_initial_rectangle(x1, y1, x2, y2, value);
    }
    while(operation_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            long long x1, y1, x2, y2, value;
            std::cin >> x1 >> y1 >> x2 >> y2 >> value;
            solver.add_rectangle(x1, y1, x2, y2, value);
        }else{
            long long x, y;
            std::cin >> x >> y;
            solver.get_point(x, y);
        }
    }
    for(const long long answer: solver.solve()) std::cout << answer << '\n';
}
