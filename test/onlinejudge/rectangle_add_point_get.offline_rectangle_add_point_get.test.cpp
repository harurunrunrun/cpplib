// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/rectangle_add_point_get

#include <iostream>

#include "../../src/algorithm/other/offline_query/offline_rectangle_add_point_get.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    OfflineRectangleAddPointGet<long long> solver;
    while(n-- > 0){
        long long x1, y1, x2, y2, value;
        std::cin >> x1 >> y1 >> x2 >> y2 >> value;
        solver.add_initial_rectangle(x1, y1, x2, y2, value);
    }
    while(q--){
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
