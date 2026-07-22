// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_1_A
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/point/projection.hpp"

int main(){
    Line line;
    std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
    int query_count;
    std::cin >> query_count;
    std::cout << std::fixed << std::setprecision(10);
    while(query_count--){
        Point point;
        std::cin >> point.x >> point.y;
        const Point answer = projection(line, point);
        std::cout << answer.x << ' ' << answer.y << '\n';
    }
}
