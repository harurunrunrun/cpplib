// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_5_A
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    int size;
    std::cin >> size;
    std::vector<Point> points(static_cast<std::size_t>(size));
    for(auto& point: points) std::cin >> point.x >> point.y;
    std::cout << std::fixed << std::setprecision(10)
              << closest_pair(std::move(points)) << '\n';
}
