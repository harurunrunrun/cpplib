// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_add_rectangle_sum

#include <iostream>

#include "../../src/algorithm/other/offline_query/offline_point_add_rectangle_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    OfflinePointAddRectangleSum<long long> solver;
    while(n-- > 0){
        long long x, y, value;
        std::cin >> x >> y >> value;
        solver.add_initial_point(x, y, value);
    }
    while(q--){
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
