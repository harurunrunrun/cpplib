#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/CGL_4_B"

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/convex_polygon_diameter.hpp"

int main(){
    int count;
    std::cin >> count;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    std::cout << std::fixed << std::setprecision(12)
              << convex_polygon_diameter(std::move(polygon)).distance() << '\n';
}
