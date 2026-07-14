// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_3_A

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/area.hpp"

int main(){
    int size;
    std::cin >> size;
    std::vector<Point> polygon(static_cast<std::size_t>(size));
    for(auto& point: polygon) std::cin >> point.x >> point.y;
    std::cout << std::fixed << std::setprecision(1) << area(polygon) << '\n';
}
