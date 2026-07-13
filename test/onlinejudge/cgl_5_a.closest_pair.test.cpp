#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/CGL_5_A"

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/geometry.hpp"

int main(){
    int size;
    std::cin >> size;
    std::vector<Point> points(static_cast<std::size_t>(size));
    for(auto& point: points) std::cin >> point.x >> point.y;
    std::cout << std::fixed << std::setprecision(12)
              << closest_pair(std::move(points)) << '\n';
}
