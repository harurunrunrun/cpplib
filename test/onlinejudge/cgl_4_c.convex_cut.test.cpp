// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_4_C
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    int size;
    std::cin >> size;
    std::vector<Point> polygon(static_cast<std::size_t>(size));
    for(auto& point: polygon) std::cin >> point.x >> point.y;
    int query_count;
    std::cin >> query_count;
    std::cout << std::fixed << std::setprecision(10);
    while(query_count--){
        Line line;
        std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        std::cout << area(convex_cut(polygon, line)) << '\n';
    }
}
